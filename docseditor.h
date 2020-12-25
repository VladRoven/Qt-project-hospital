#ifndef DOCSEDITOR_H
#define DOCSEDITOR_H

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class DocsEditor;
}

class DocsEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit DocsEditor(QWidget *parent = nullptr);
    ~DocsEditor();

    void setModel(QAbstractItemModel *model, bool isEdit, QString old_name);
    void setModel(QStandardItemModel *model, bool isEdit);
    QDataWidgetMapper *mapper;

signals:
    void updateListDocs();
    void changeDoc(QString old_name, QString new_name);

private slots:
    void on_btn_accept_clicked();

    void on_btn_cancle_clicked();

    void clear();

private:
    Ui::DocsEditor *ui;
    bool isEdit;
    QString old_name;
};

#endif // DOCSEDITOR_H
