#ifndef VISITOREDITOR_H
#define VISITOREDITOR_H

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class VisitorEditor;
}

class VisitorEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit VisitorEditor(QWidget *parent = nullptr);
    ~VisitorEditor();

    void setModel(QAbstractItemModel *model, bool isEdit, QString old_name);
    void setModel(QStandardItemModel *model, bool isEdit);
    QDataWidgetMapper *mapper;

signals:
    void changeVisitor(QString old_name, QString new_name);

private slots:
    void on_btn_accept_clicked();

    void on_btn_cancle_clicked();

    void clear();

private:
    Ui::VisitorEditor *ui;
    bool isEdit;
    QString old_name;
};

#endif // VISITOREDITOR_H
