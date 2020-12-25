#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QFileInfo>
#include <QMainWindow>
#include <QStandardItemModel>
#include "docseditor.h"
#include "visitoreditor.h"
#include "recordeditor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void changeDocInRecord(QString old_name, QString new_name);

    void changeVisitorInRecord(QString old_name, QString new_name);

    void setDocList();

    void on_new_file_triggered();

    void on_open_file_triggered();

    void on_save_triggered();

    void on_save_as_triggered();

    void on_search_doc_textChanged(const QString &arg1);

    void on_search_visitor_textChanged(const QString &arg1);

    void on_docs_list_currentTextChanged(const QString &arg1);

    void on_drop_filter_clicked();

    void on_date_filter_userDateChanged(const QDate &date);

    void on_table_docs_doubleClicked(const QModelIndex &index);

    void on_btn_add_doc_clicked();

    void on_table_visitors_doubleClicked(const QModelIndex &index);

    void on_add_visitor_clicked();

    void on_btn_del_doc_clicked();

    void on_del_visitor_clicked();

    void on_action_triggered();

    void on_to_excael_all_doc_triggered();

    void on_to_excel_all_vis_triggered();

    void on_to_excel_all_records_triggered();

    void on_add_record_clicked();

    void setDateRecord(QString doc);

    void on_table_records_doubleClicked(const QModelIndex &index);

    void on_del_record_clicked();

private:
    Ui::MainWindow *ui;
    QFileInfo fileInfo;
    QStandardItemModel *model_docs = new QStandardItemModel;
    QStandardItemModel *model_visitors = new QStandardItemModel;
    QStandardItemModel *model_records = new QStandardItemModel;
    DocsEditor *docs_editor;
    VisitorEditor *visitor_editor;
    RecordEditor *record_editor;
};
#endif // MAINWINDOW_H
