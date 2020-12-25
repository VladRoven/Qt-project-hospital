#ifndef RECORDEDITOR_H
#define RECORDEDITOR_H

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QDateTime>
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class RecordEditor;
}

class RecordEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit RecordEditor(QWidget *parent = nullptr);
    ~RecordEditor();

    void setModel(QAbstractItemModel *model, bool isEdit, QDateTime current);
    void setModel(QStandardItemModel *model, bool isEdit);
    void setComboBoxAll(QStringList docs, QStringList visitors);
    void setDate(QDateTime min, QDateTime max);
    QDataWidgetMapper *mapper;

signals:
    void setDocWorkTime(QString doc);

private slots:
    void on_btn_accept_clicked();

    void on_btn_cancle_clicked();

    void on_doc_currentTextChanged(const QString &arg1);

private:
    Ui::RecordEditor *ui;
    bool isEdit;
    QDateTime current;
};

#endif // RECORDEDITOR_H
