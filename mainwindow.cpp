#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"

#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMap>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDateTime>
#include <QDateTime>
#include <QAbstractItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->date_filter->setDate(QDate::currentDate());

    docs_editor = new DocsEditor(this);
    visitor_editor = new VisitorEditor(this);
    record_editor = new RecordEditor(this);

    connect(docs_editor, &DocsEditor::updateListDocs, this, &MainWindow::setDocList);
    connect(docs_editor, &DocsEditor::changeDoc, this, &MainWindow::changeDocInRecord);
    connect(visitor_editor, &VisitorEditor::changeVisitor, this, &MainWindow::changeVisitorInRecord);
    connect(record_editor, &RecordEditor::setDocWorkTime, this, &MainWindow::setDateRecord);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeDocInRecord(QString old_name, QString new_name)
{
    for (int i = 0; i < model_records->rowCount(); i++)
    {
        if (model_records->item(i, 1)->text().contains(old_name))
        {
            model_records->setItem(i, 1, new QStandardItem(new_name));
        }
    }
}

void MainWindow::changeVisitorInRecord(QString old_name, QString new_name)
{
    for (int i = 0; i < model_records->rowCount(); i++)
    {
        if (model_records->item(i, 0)->text().contains(old_name))
        {
            model_records->setItem(i, 0, new QStandardItem(new_name));
        }
    }
}

void MainWindow::setDocList()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QStringList list;

        list.append("Всех");
        for (int i = 0; i < model_docs->rowCount(); i++)
        {
            list.append(model_docs->item(i, 0)->text() + " " + model_docs->item(i, 1)->text());
        }

        ui->docs_list->clear();
        ui->docs_list->addItems(list);
    }
}


void MainWindow::on_new_file_triggered()
{
    QString saveFileName = QFileDialog::getSaveFileName(this,
                                                            tr("Новый файл"),
                                                            QString(),
                                                            tr("JSON (*.json)"));
    fileInfo = saveFileName;
    QFileInfo fileInfo(saveFileName);
    QDir::setCurrent(fileInfo.path());
    QFile json_file(saveFileName);

    if (!json_file.open(QIODevice::WriteOnly))
    {
        return;
    }
    json_file.close();

    model_docs->clear();
    model_visitors->clear();
    model_records->clear();

    QStringList horizontalHeader;
    horizontalHeader.append({"Фамилия", "Имя", "Отчество", "Специальность", "Работает с", "Работает до"});
    model_docs->setHorizontalHeaderLabels(horizontalHeader);

    horizontalHeader.clear();
    horizontalHeader.append({"Фамилия", "Имя", "Отчество", "Дата рождения"});
    model_visitors->setHorizontalHeaderLabels(horizontalHeader);

    horizontalHeader.clear();
    horizontalHeader.append({"Клиент", "Врач", "Дата приёма"});
    model_records->setHorizontalHeaderLabels(horizontalHeader);

    ui->table_docs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_visitors->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_records->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->table_docs->setModel(model_docs);
    ui->table_visitors->setModel(model_visitors);
    ui->table_records->setModel(model_records);
    ui->status_line->showMessage("Открыт файл: " + fileInfo.baseName() + " (" + fileInfo.absoluteFilePath() + ")");
}

void MainWindow::on_open_file_triggered()
{
    QString openFileName = QFileDialog::getOpenFileName(this, tr("Открыть файл"), QString(), tr("JSON (*.json)"));
    fileInfo = openFileName;
    QFile jsonFile(openFileName);

    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    model_docs->clear();
    model_records->clear();
    model_visitors->clear();
    QByteArray saveData = jsonFile.readAll();

    QJsonDocument json_doc = QJsonDocument::fromJson(saveData);

    QStringList horizontalHeader;
    horizontalHeader.append({"Фамилия", "Имя", "Отчество", "Специальность", "Работает с", "Работает до"});
    model_docs->setHorizontalHeaderLabels(horizontalHeader);

    horizontalHeader.clear();
    horizontalHeader.append({"Фамилия", "Имя", "Отчество", "Дата рождения"});
    model_visitors->setHorizontalHeaderLabels(horizontalHeader);

    horizontalHeader.clear();
    horizontalHeader.append({"Клиент", "Врач", "Дата приёма"});
    model_records->setHorizontalHeaderLabels(horizontalHeader);

    ui->table_docs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_visitors->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_records->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QJsonArray json_docs = json_doc.object()["docs"].toArray();
    QJsonArray json_visitors = json_doc.object()["visitors"].toArray();
    QJsonArray json_records = json_doc.object()["records"].toArray();

    QJsonArray temp_array;
    QList<QStandardItem *> temp_list;

    for (int i = 0; i < json_docs.count(); i++)
    {
        temp_list.clear();
        temp_array = json_docs[i].toArray();
        for (int j = 0; j < temp_array.count(); j++)
        {
             temp_list.append(new QStandardItem(temp_array[j].toString()));
        }
        model_docs->insertRow(i, temp_list);

        QString format = "HH:mm:ss.zzz";

        QString strValue = model_docs->data(model_docs->index(i, 4)).toString();
        QTime time = QTime :: fromString (strValue, format);
        model_docs->setData(model_docs->index(i, 4), time);

        strValue = model_docs->data(model_docs->index(i, 5)).toString();
        time = QTime :: fromString (strValue, format);
        model_docs->setData(model_docs->index(i, 5), time);
    }

    setDocList();

    for (int i = 0; i < json_visitors.count(); i++)
    {
        temp_list.clear();
        temp_array = json_visitors[i].toArray();
        for (int j = 0; j < temp_array.count(); j++)
        {
           temp_list.append(new QStandardItem(temp_array[j].toString()));
        }
        model_visitors->insertRow(i, temp_list);

        QString format = "yyyy-MM-dd";

        QString strValue = model_visitors->data(model_visitors->index(i, 3)).toString();
        QDate date = QDate :: fromString (strValue, format);
        model_visitors->setData(model_visitors->index(i, 3), date);
    }

    for (int i = 0; i < json_records.count(); i++)
    {
        temp_list.clear();
        temp_array = json_records[i].toArray();
        for (int j = 0; j < temp_array.count(); j++)
        {
           temp_list.append(new QStandardItem(temp_array[j].toString()));
        }
        model_records->insertRow(i, temp_list);

        QString strValue = model_records->data(model_records->index(i, 2)).toString();
        QString format = "yyyy-MM-ddTHH:mm:ss.zzz";
        QDateTime dt = QDateTime :: fromString (strValue, format);
        model_records->setData(model_records->index(i, 2), dt);
    }

    ui->table_visitors->setModel(model_visitors);
    ui->table_docs->setModel(model_docs);
    ui->table_records->setModel(model_records);
    ui->status_line->showMessage("Открыт файл: " + fileInfo.baseName() + " (" + fileInfo.absoluteFilePath() + ")");
}

void MainWindow::on_save_triggered()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QFile json_file(fileInfo.absoluteFilePath());
        QDir::setCurrent(fileInfo.path());

        if (!json_file.open(QIODevice::WriteOnly))
        {
            return;
        }

        QJsonObject json;
        QJsonArray data;
        for (int i = 0; i < model_docs->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_docs->columnCount(); j++)
            {
                row.append(QJsonValue(model_docs->item(i, j)->text()));
            }
            data.append(row);
        }

        json["docs"] = data;
        data = {};

        for (int i = 0; i < model_visitors->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_visitors->columnCount(); j++)
            {
                row.append(QJsonValue(model_visitors->item(i, j)->text()));
            }
            data.append(row);
        }
        json["visitors"] = data;
        data = {};

        for (int i = 0; i < model_records->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_records->columnCount(); j++)
            {
                row.append(QJsonValue(model_records->item(i, j)->text()));
            }
            data.append(row);
        }
        json["records"] = data;

        QJsonDocument saveDoc(json);
        json_file.write(saveDoc.toJson());
        json_file.close();
        ui->status_line->showMessage("Файл \"" + fileInfo.baseName() + "\" сохранён! Путь: " + fileInfo.absoluteFilePath());
    }
    else
        ui->status_line->showMessage("Файл не открыт!");
}

void MainWindow::on_save_as_triggered()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Сохранить файл как"),
                                                                QString(),
                                                                tr("JSON (*.json)"));
        QFileInfo fileInfo(saveFileName);
        QDir::setCurrent(fileInfo.path());
        QFile json_file(saveFileName);

        if (!json_file.open(QIODevice::WriteOnly))
        {
            return;
        }

        QJsonObject json;
        QJsonArray data;
        for (int i = 0; i < model_docs->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_docs->columnCount(); j++)
            {
                row.append(QJsonValue(model_docs->item(i, j)->text()));
            }
            data.append(row);
        }

        json["docs"] = data;
        data = {};

        for (int i = 0; i < model_visitors->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_visitors->columnCount(); j++)
            {
                row.append(QJsonValue(model_visitors->item(i, j)->text()));
            }
            data.append(row);
        }
        json["visitors"] = data;
        data = {};

        for (int i = 0; i < model_records->rowCount(); i++)
        {
            QJsonArray row;

            for (int j = 0; j < model_records->columnCount(); j++)
            {
                row.append(QJsonValue(model_records->item(i, j)->text()));
            }
            data.append(row);
        }
        json["records"] = data;

        QJsonDocument saveDoc(json);
        json_file.write(saveDoc.toJson());
        json_file.close();
        ui->status_line->showMessage("Файл \"" + fileInfo.baseName() + "\" сохранён! Путь: " + fileInfo.absoluteFilePath());
    }
    else
        ui->status_line->showMessage("Файл не открыт!");
}

void MainWindow::on_search_doc_textChanged(const QString &arg1)
{
    if (fileInfo.absoluteFilePath() != "")
    {
        if (arg1.length())
        {
            QString str = arg1;
            str[ 0 ] = str[ 0 ].toUpper();
            for (int i = 1; i < str.length(); i++)
            {
                str[i] = str[i].toLower();
            }
            QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel();
            proxy_model->setSourceModel(model_docs);
            ui->table_docs->setModel(proxy_model);
            proxy_model->setFilterKeyColumn(0);
            proxy_model->setFilterRegExp(str);
            ui->status_line->showMessage("Найдено записей: " + QString::number(proxy_model->rowCount()));
        }
        else
        {
            ui->table_docs->setModel(model_docs);
            ui->status_line->showMessage("Всего записей: " + QString::number(model_docs->rowCount()));
        }
    }
}

void MainWindow::on_search_visitor_textChanged(const QString &arg1)
{
    if (fileInfo.absoluteFilePath() != "")
    {
        if (arg1.length())
        {
            QString str = arg1;
            str[ 0 ] = str[ 0 ].toUpper();
            for (int i = 1; i < str.length(); i++)
            {
                str[i] = str[i].toLower();
            }
            QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel();
            proxy_model->setSourceModel(model_visitors);
            ui->table_visitors->setModel(proxy_model);
            proxy_model->setFilterKeyColumn(0);
            proxy_model->setFilterRegExp(str);
            ui->status_line->showMessage("Найдено записей: " + QString::number(proxy_model->rowCount()));
        }
        else
        {
            ui->table_visitors->setModel(model_visitors);
            ui->status_line->showMessage("Всего записей: " + QString::number(model_visitors->rowCount()));
        }
    }
}

void MainWindow::on_docs_list_currentTextChanged(const QString &arg1)
{
    if (fileInfo.absoluteFilePath() != "")
    {
        if (arg1 != "Всех")
        {
            QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel();
            proxy_model->setSourceModel(model_records);
            ui->table_records->setModel(proxy_model);
            proxy_model->setFilterKeyColumn(1);
            proxy_model->setFilterRegExp(arg1);
            ui->status_line->showMessage("Найдено записей: " + QString::number(proxy_model->rowCount()));
        }
        else
        {
            ui->table_records->setModel(model_records);
            ui->status_line->showMessage("Всего записей: " + QString::number(model_records->rowCount()));
        }
    }
}

void MainWindow::on_date_filter_userDateChanged(const QDate &date)
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QString date_str = QString::number(date.year()) + "-" + ((QString::number(date.month()).length() == 1) ? "0" + QString::number(date.month()) : QString::number(date.month())) + "-" + ((QString::number(date.day()).length() == 1) ? "0" + QString::number(date.day()) : QString::number(date.day()));
        qDebug() << date_str;
        QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel();
        proxy_model->setSourceModel(model_records);
        ui->table_records->setModel(proxy_model);
        ui->table_records->sortByColumn(2, Qt::AscendingOrder);
        proxy_model->setFilterKeyColumn(2);
        proxy_model->setFilterRegExp(date_str);
        ui->status_line->showMessage("Найдено записей: " + QString::number(proxy_model->rowCount()));
    }
}

void MainWindow::on_drop_filter_clicked()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        ui->table_records->setModel(model_records);
        ui->status_line->showMessage("Всего записей: " + QString::number(model_records->rowCount()));
    }
}

void MainWindow::on_table_docs_doubleClicked(const QModelIndex &index)
{
    if (fileInfo.absoluteFilePath() != "")
    {
        docs_editor->setModel(ui->table_docs->model(), true, ui->table_docs->model()->index(index.row(), 0).data().toString() + " " + ui->table_docs->model()->index(index.row(), 1).data().toString());
        docs_editor->mapper->setCurrentModelIndex(index);
        docs_editor->setWindowModality(Qt::ApplicationModal);
        docs_editor->show();
    }
}

void MainWindow::on_btn_add_doc_clicked()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        docs_editor->setModel(model_docs, false);
        docs_editor->setWindowModality(Qt::ApplicationModal);
        docs_editor->show();
    }
}

void MainWindow::on_table_visitors_doubleClicked(const QModelIndex &index)
{
    if (fileInfo.absoluteFilePath() != "")
    {
        visitor_editor->setModel(ui->table_visitors->model(), true, ui->table_visitors->model()->index(index.row(), 0).data().toString() + " " + ui->table_visitors->model()->index(index.row(), 1).data().toString());
        visitor_editor->mapper->setCurrentModelIndex(index);
        visitor_editor->setWindowModality(Qt::ApplicationModal);
        visitor_editor->show();
    }
}

void MainWindow::on_add_visitor_clicked()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        visitor_editor->setModel(model_visitors, false);
        visitor_editor->setWindowModality(Qt::ApplicationModal);
        visitor_editor->show();
    }
}

void MainWindow::on_btn_del_doc_clicked()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QModelIndex index = ui->table_docs->currentIndex();
        auto model = ui->table_docs->model();

        if (index.row() >= 0)
        {
            QMessageBox *msg = new QMessageBox();
            msg->setIcon(QMessageBox::Information);
            msg->setWindowTitle("Подтверждение удаления");
            msg->setText("Вы действительно хотите удалить выбранного врача?");
            msg->setInformativeText("При удалении врача будет удалена иформация о записях, закреплённая за ним!");
            QPushButton *btn_ok = msg->addButton("Да", QMessageBox::AcceptRole);
            msg->addButton("Отмена", QMessageBox::RejectRole);
            msg->exec();

            if (msg->clickedButton() == btn_ok)
            {
                for (int i = model_records->rowCount() - 1; i >= 0; i--)
                {
                    if (model_records->item(i, 1)->text().contains(model->data(model->index(index.row(), 0)).toString() + " " + model->data(model->index(index.row(), 1)).toString()))
                    {
                        model_records->removeRow(i);
                    }
                }

                model->removeRow(index.row());
                ui->table_docs->setModel(model_docs);
                ui->table_records->setModel(model_records);
            }
        }
        else
        {
            ui->status_line->showMessage("Выберите доктора!");
        }
    }
}

void MainWindow::on_del_visitor_clicked()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QModelIndex index = ui->table_visitors->currentIndex();
        auto model = ui->table_visitors->model();

        if (index.row() >= 0)
        {
            QMessageBox *msg = new QMessageBox();
            msg->setIcon(QMessageBox::Information);
            msg->setWindowTitle("Подтверждение удаления");
            msg->setText("Вы действительно хотите удалить выбранного посетителя?");
            msg->setInformativeText("При удалении посетителя будет удалена иформация о записях, закреплённая за ним!");
            QPushButton *btn_ok = msg->addButton("Да", QMessageBox::AcceptRole);
            msg->addButton("Отмена", QMessageBox::RejectRole);
            msg->exec();

            if (msg->clickedButton() == btn_ok)
            {
                for (int i = model_records->rowCount() - 1; i >= 0; i--)
                {
                    if (model_records->item(i, 0)->text().contains(model->data(model->index(index.row(), 0)).toString() + " " + model->data(model->index(index.row(), 1)).toString()))
                    {
                        model_records->removeRow(i);
                    }
                }

                model->removeRow(index.row());
                ui->table_visitors->setModel(model_visitors);
                ui->table_records->setModel(model_records);
            }
        }
        else
        {
            ui->status_line->showMessage("Выберите посетителя!");
        }
    }
}

void MainWindow::on_action_triggered()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QString doc = ui->table_docs->model()->index(ui->table_docs->currentIndex().row(), 0).data().toString() + " " + ui->table_docs->model()->index(ui->table_docs->currentIndex().row(), 1).data().toString();

        if (ui->table_docs->currentIndex().isValid())
        {
            QString time_from = ui->table_docs->model()->index(ui->table_docs->currentIndex().row(),4).data().toString();
            QString time_to = ui->table_docs->model()->index(ui->table_docs->currentIndex().row(), 5).data().toString();
            QString format = "HH:mm:ss.zzz";
            QTime t_from = QTime :: fromString (time_from, format);
            QTime t_to = QTime :: fromString (time_to, format);

            QXlsx::Document excel;
            QXlsx::Format excel_format;
            excel_format.setFontBold(true);
            excel.setColumnWidth(1, 6, 23);
            excel.setRowFormat(1, excel_format);
            excel.setRowFormat(4, excel_format);
            excel.write("A1", "Фамилия");
            excel.write("B1", "Имя");
            excel.write("C1", "Отчество");
            excel.write("D1", "Специальность");
            excel.write("E1", "Работает с");
            excel.write("F1", "Работает до");

            excel.write("A2", ui->table_docs->model()->index(ui->table_docs->currentIndex().row(), 0).data().toString());
            excel.write("B2", ui->table_docs->model()->index(ui->table_docs->currentIndex().row(), 1).data().toString());
            excel.write("C2", ui->table_docs->model()->index(ui->table_docs->currentIndex().row(), 2).data().toString());
            excel.write("D2", ui->table_docs->model()->index(ui->table_docs->currentIndex().row(), 3).data().toString());
            excel.write("E2", t_from.toString("HH:mm"));
            excel.write("F2", t_to.toString("HH:mm"));

            excel.write("A4", "Посетитель");
            excel.write("B4", "Дата записи");

            int itr = 5;
            QString date_record;
            QString format_record = "yyyy-MM-ddTHH:mm:ss.zzz";
            QDateTime dt_record;
            for (int i = 0; i < model_records->rowCount(); i++)
            {
                if (model_records->item(i, 1)->text().contains(doc))
                {
                    date_record = model_records->item(i, 2)->text();
                    dt_record = QDateTime :: fromString (date_record, format_record);
                    excel.write("A" + QString::number(itr), model_records->item(i, 0)->text());
                    excel.write("B" + QString::number(itr), dt_record.toString("dd.MM.yyyy HH:mm"));
                    ++itr;
                }
            }

            QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                    tr("Новый файл"),
                                                                    QString(doc),
                                                                    tr("Excel (*.xlsx)"));
            excel.saveAs(saveFileName);
        }
        else
            ui->status_line->showMessage("Выберите доктора!");
    }
}

void MainWindow::on_to_excael_all_doc_triggered()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QXlsx::Document excel;
        QXlsx::Format excel_format;
        excel_format.setFontBold(true);
        excel.setColumnWidth(1, 6, 23);
        excel.setRowFormat(1, excel_format);
        excel.write("A1", "Фамилия");
        excel.write("B1", "Имя");
        excel.write("C1", "Отчество");
        excel.write("D1", "Специальность");
        excel.write("E1", "Работает с");
        excel.write("F1", "Работает до");

        int itr = 2;
        for (int i = 0; i < model_docs->rowCount(); i++)
        {
            excel.write("A" + QString::number(itr), model_docs->item(i, 0)->text());
            excel.write("B" + QString::number(itr), model_docs->item(i, 1)->text());
            excel.write("C" + QString::number(itr), model_docs->item(i, 2)->text());
            excel.write("D" + QString::number(itr), model_docs->item(i, 3)->text());
            excel.write("E" + QString::number(itr), QTime::fromString(model_docs->item(i, 4)->text(), "HH:mm:ss.zzz").toString("HH:mm"));
            excel.write("F" + QString::number(itr), QTime::fromString(model_docs->item(i, 5)->text(), "HH:mm:ss.zzz").toString("HH:mm"));
            ++itr;
        }

        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Новый файл"),
                                                                QString("Все врачи"),
                                                                tr("Excel (*.xlsx)"));
        excel.saveAs(saveFileName);
    }
}

void MainWindow::on_to_excel_all_vis_triggered()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QXlsx::Document excel;
        QXlsx::Format excel_format;
        excel_format.setFontBold(true);
        excel.setColumnWidth(1, 4, 23);
        excel.setRowFormat(1, excel_format);
        excel.write("A1", "Фамилия");
        excel.write("B1", "Имя");
        excel.write("C1", "Отчество");
        excel.write("D1", "Дата рождения");

        int itr = 2;
        for (int i = 0; i < model_visitors->rowCount(); i++)
        {
            excel.write("A" + QString::number(itr), model_visitors->item(i, 0)->text());
            excel.write("B" + QString::number(itr), model_visitors->item(i, 1)->text());
            excel.write("C" + QString::number(itr), model_visitors->item(i, 2)->text());
            excel.write("D" + QString::number(itr), QDate::fromString(model_visitors->item(i, 3)->text(), "yyyy-MM-dd").toString("dd.MM.yyyy"));
            ++itr;
        }

        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Новый файл"),
                                                                QString("Все посетители"),
                                                                tr("Excel (*.xlsx)"));
        excel.saveAs(saveFileName);
    }
}

void MainWindow::on_to_excel_all_records_triggered()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QXlsx::Document excel;
        QXlsx::Format excel_format;
        excel_format.setFontBold(true);
        excel.setColumnWidth(1, 3, 23);
        excel.setRowFormat(1, excel_format);
        excel.write("A1", "Клиент");
        excel.write("B1", "Врач");
        excel.write("C1", "Дата записи");

        int itr = 2;
        for (int i = 0; i < model_records->rowCount(); i++)
        {
            excel.write("A" + QString::number(itr), model_records->item(i, 0)->text());
            excel.write("B" + QString::number(itr), model_records->item(i, 1)->text());
            excel.write("C" + QString::number(itr), QDateTime::fromString(model_records->item(i, 2)->text(), "yyyy-MM-ddTHH:mm:ss.zzz").toString("dd.MM.yyyy HH:mm"));;
            ++itr;
        }

        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                                tr("Новый файл"),
                                                                QString("Все записи"),
                                                                tr("Excel (*.xlsx)"));
        excel.saveAs(saveFileName);
    }
}

void MainWindow::on_add_record_clicked()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QStringList docs;
        QStringList visitors;

        docs.append("Не выбрано");
        visitors.append("Не выбрано");

        for (int i = 0; i < model_docs->rowCount(); i++)
        {
            docs.append(model_docs->item(i, 0)->text() + " " + model_docs->item(i, 1)->text());
        }

        for (int i = 0; i < model_visitors->rowCount(); i++)
        {
            visitors.append(model_visitors->item(i, 0)->text() + " " + model_visitors->item(i, 1)->text());
        }

        record_editor->setComboBoxAll(docs, visitors);
        record_editor->setModel(model_records, false);
        record_editor->setWindowModality(Qt::ApplicationModal);
        record_editor->show();
    }
}

void MainWindow::setDateRecord(QString doc)
{
    QString doc_in_model;
    for (int i = 0; i < model_docs->rowCount(); i++)
    {
        doc_in_model = model_docs->item(i, 0)->text() + " " + model_docs->item(i, 1)->text();
        if (doc_in_model.contains(doc))
        {
            QString min_dt = QDate::currentDate().toString("yyyy-MM-dd") + "T" + model_docs->item(i, 4)->text();

            QDate current_plus_day = QDate::currentDate().addDays(100);
            current_plus_day = current_plus_day.addMonths(12);
            current_plus_day = current_plus_day.addYears(1);
            QString max_dt = current_plus_day.toString("yyyy-MM-dd") + "T" + model_docs->item(i, 5)->text();
            record_editor->setDate(QDateTime::fromString(min_dt, "yyyy-MM-ddTHH:mm:ss.zzz"), QDateTime::fromString(max_dt, "yyyy-MM-ddTHH:mm:ss.zzz"));
            break;
        }
    }
}

void MainWindow::on_table_records_doubleClicked(const QModelIndex &index)
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QStringList docs;
        QStringList visitors;

        for (int i = 0; i < model_docs->rowCount(); i++)
        {
            docs.append(model_docs->item(i, 0)->text() + " " + model_docs->item(i, 1)->text());
        }

        for (int i = 0; i < model_visitors->rowCount(); i++)
        {
            visitors.append(model_visitors->item(i, 0)->text() + " " + model_visitors->item(i, 1)->text());
        }

        record_editor->setComboBoxAll(docs, visitors);
        record_editor->setModel(ui->table_records->model(), true, ui->table_records->model()->index(index.row(), 2).data().toDateTime());
        record_editor->mapper->setCurrentModelIndex(index);
        record_editor->setWindowModality(Qt::ApplicationModal);
        record_editor->show();
    }
}

void MainWindow::on_del_record_clicked()
{
    if (fileInfo.absoluteFilePath() != "")
    {
        QModelIndex index = ui->table_records->currentIndex();
        auto model = ui->table_records->model();

        if (index.row() >= 0)
        {
            QMessageBox *msg = new QMessageBox();
            msg->setIcon(QMessageBox::Information);
            msg->setWindowTitle("Подтверждение удаления");
            msg->setText("Вы действительно хотите удалить выбранную запись?");
            QPushButton *btn_ok = msg->addButton("Да", QMessageBox::AcceptRole);
            msg->addButton("Отмена", QMessageBox::RejectRole);
            msg->exec();

            if (msg->clickedButton() == btn_ok)
            {
                model->removeRow(index.row());
                ui->table_records->setModel(model_records);
            }
        }
        else
        {
            ui->status_line->showMessage("Выберите запись!");
        }
    }
}
