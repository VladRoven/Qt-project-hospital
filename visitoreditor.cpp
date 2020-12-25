#include "visitoreditor.h"
#include "ui_visitoreditor.h"

#include <QMessageBox>

VisitorEditor::VisitorEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VisitorEditor)
{
    ui->setupUi(this);
    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

VisitorEditor::~VisitorEditor()
{
    delete ui;
}

void VisitorEditor::setModel(QAbstractItemModel *model, bool isEdit, QString old_name)
{
    clear();
    this->old_name = old_name;
    this->isEdit = isEdit;
    mapper->setModel(model);
    mapper->addMapping(ui->surname, 0);
    mapper->addMapping(ui->name, 1);
    mapper->addMapping(ui->patronymic, 2);
    mapper->addMapping(ui->date, 3);
}

void VisitorEditor::setModel(QStandardItemModel *model, bool isEdit)
{
    clear();
    this->isEdit = isEdit;
    mapper->setModel(model);
}

void VisitorEditor::on_btn_accept_clicked()
{
    QString error = "";
    QRegExp client_reg("([А-Я][а-яё]+)");
    if (!client_reg.exactMatch(ui->surname->text()))
        error += "Корректно введите фамилию!\n";

    if (!client_reg.exactMatch(ui->name->text()))
        error += "Корректно введите имя!\n";

    if (!client_reg.exactMatch(ui->patronymic->text()))
        error += "Корректно введите отчество!\n";

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
            emit changeVisitor(old_name, ui->surname->text() + " " + ui->name->text());
            close();
        }
        else
        {
            mapper->model()->insertRow(mapper->model()->rowCount());
            mapper->setCurrentModelIndex(mapper->model()->index(mapper->model()->rowCount() - 1, 0));

            mapper->addMapping(ui->surname, 0);
            mapper->addMapping(ui->name, 1);
            mapper->addMapping(ui->patronymic, 2);
            mapper->addMapping(ui->date, 3);

            mapper->submit();

            close();
        }
    }
}

void VisitorEditor::on_btn_cancle_clicked()
{
    close();
}

void VisitorEditor::clear()
{
    QString str_time_from = "2020-01-01";
    QString format = "yyyy-MM-dd";
    QDate date = QDate :: fromString (str_time_from, format);

    ui->surname->clear();
    ui->name->clear();
    ui->patronymic->clear();
    ui->date->setDate(date);
    mapper->clearMapping();
}
