#include "fileedit.h"
#include "hkzip.h"
#include <QThread>
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMessageBox msgBox; // 阻塞？
    hkzip *zip;
    bool path_in_cor;
    bool path_out_cor;
    void initialize_all();
    QThread *zip_thread; // 在这里使用了多线程
    void zip_finished();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

signals:
    void make_zip();
    void process_zip();

private:
    Ui::MainWindow *ui;
};

#endif
