#include "docseditor.h"
#include "ui_docseditor.h"

#include <QMessageBox>

DocsEditor::DocsEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DocsEditor)
{
    ui->setupUi(this);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

DocsEditor::~DocsEditor()
{
    delete ui;
}

void DocsEditor::setModel(QAbstractItemModel *model, bool isEdit, QString old_name)
{
    clear();
    this->old_name = old_name;
    this->isEdit = isEdit;
    mapper->clearMapping();
    mapper->setModel(model);
    mapper->addMapping(ui->surname, 0);
    mapper->addMapping(ui->name, 1);
    mapper->addMapping(ui->patronymic, 2);
    mapper->addMapping(ui->specialty, 3);
    mapper->addMapping(ui->time_from, 4);
    mapper->addMapping(ui->time_to, 5);
}

void DocsEditor::setModel(QStandardItemModel *model, bool isEdit)
{
    clear();
    this->isEdit = isEdit;
    mapper->clearMapping();
    mapper->setModel(model);
}

void DocsEditor::on_btn_accept_clicked()
{
    QString error = "";
    QRegExp client_reg("([А-Я][а-яё]+)");
    if (!client_reg.exactMatch(ui->surname->text()))
        error += "Корректно введите фамилию!\n";

    if (!client_reg.exactMatch(ui->name->text()))
        error += "Корректно введите имя!\n";

    if (!client_reg.exactMatch(ui->patronymic->text()))
        error += "Корректно введите отчество!\n";

    if (ui->time_from->time().hour() >= ui->time_to->time().hour())
        error += "Корректно укажите время работы!\n";

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
            emit updateListDocs();
            emit changeDoc(old_name, ui->surname->text() + " " + ui->name->text());
            close();
        }
        else
        {
            mapper->model()->insertRow(mapper->model()->rowCount());
            mapper->setCurrentModelIndex(mapper->model()->index(mapper->model()->rowCount() - 1, 0));

            mapper->addMapping(ui->surname, 0);
            mapper->addMapping(ui->name, 1);
            mapper->addMapping(ui->patronymic, 2);
            mapper->addMapping(ui->specialty, 3);
            mapper->addMapping(ui->time_from, 4);
            mapper->addMapping(ui->time_to, 5);

            mapper->submit();
            emit updateListDocs();

            close();
        }
    }
}

void DocsEditor::on_btn_cancle_clicked()
{
    close();
}

void DocsEditor::clear()
{
    QString str_time_from = "09:00:00.000";
    QString str_time_to = "12:00:00.000";
    QString format = "HH:mm:ss.zzz";
    QTime time_from = QTime :: fromString (str_time_from, format);
    QTime time_to = QTime :: fromString (str_time_to, format);

    ui->surname->clear();
    ui->name->clear();
    ui->patronymic->clear();
    ui->specialty->clear();
    ui->time_from->setTime(time_from);
    ui->time_to->setTime(time_to);
}
