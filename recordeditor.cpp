#include "recordeditor.h"
#include "ui_recordeditor.h"
#include <QDebug>
#include <QMessageBox>

RecordEditor::RecordEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RecordEditor)
{
    ui->setupUi(this);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

RecordEditor::~RecordEditor()
{
    delete ui;
}

void RecordEditor::setModel(QAbstractItemModel *model, bool isEdit, QDateTime current)
{
    this->current = current;
    this->isEdit = isEdit;
    mapper->setModel(model);
    mapper->addMapping(ui->visitor, 0);
    mapper->addMapping(ui->doc, 1);
    mapper->addMapping(ui->date, 2);
}

void RecordEditor::setModel(QStandardItemModel *model, bool isEdit)
{
    this->isEdit = isEdit;
    mapper->setModel(model);
}

void RecordEditor::setComboBoxAll(QStringList docs, QStringList visitors)
{
    ui->doc->clear();
    ui->visitor->clear();

    ui->doc->addItems(docs);
    ui->visitor->addItems(visitors);

    ui->date->setDateTime(QDateTime::currentDateTime());
}

void RecordEditor::setDate(QDateTime min, QDateTime max)
{
    ui->date->setMinimumDateTime(min);
    ui->date->setMaximumDateTime(max);
}

void RecordEditor::on_btn_accept_clicked()
{
    QString error = "";

    if (ui->visitor->currentText().contains("Не выбрано"))
        error += "Выберите посетителя!\n";

    if (ui->doc->currentText().contains("Не выбрано"))
        error += "Выберите врача!\n";

    if (ui->date->dateTime() < QDateTime::currentDateTime())
        if (ui->date->dateTime() != current)
            error += "Корректно укажите дату!\n";

    if (error.length())
    {
        QMessageBox *msg = new QMessageBox();
        msg->setIcon(QMessageBox::Warning);
        msg->setWindowTitle("Ошибка");
        msg->setInformativeText(error);
        msg->addButton("Понял", QMessageBox::AcceptRole);
        msg->setAttribute(Qt::WA_QuitOnClose, false);
        msg->exec();
    }
    else
    {
        if (isEdit)
        {
            mapper->submit();
            close();
        }
        else
        {
            mapper->model()->insertRow(mapper->model()->rowCount());
            mapper->setCurrentModelIndex(mapper->model()->index(mapper->model()->rowCount() - 1, 0));

            mapper->addMapping(ui->visitor, 0);
            mapper->addMapping(ui->doc, 1);
            mapper->addMapping(ui->date, 2);

            mapper->submit();

            close();
        }
    }
}

void RecordEditor::on_btn_cancle_clicked()
{
    close();
}

void RecordEditor::on_doc_currentTextChanged(const QString &arg1)
{
    emit setDocWorkTime(arg1);
}
