
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), msgBox(this) // 记住父窗口，
      ,
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化成员
    setWindowTitle("hkzip");
    path_in_cor = 0;
    path_out_cor = 0;
    zip = new hkzip;
    msgBox.setWindowTitle("请等待 ＞﹏＜");
    msgBox.setText("......");
    msgBox.setStandardButtons(QMessageBox::NoButton); // 重要（没了它可以尝试一下，会导致可以在子线程运行的时候更改zip->path_out,导致结果出错（数据竞争））

    zip_thread = new QThread;
    zip->moveToThread(zip_thread);
    ui->progressBar->hide();

    // 连接信号和槽，启用线程
    connect(ui->lineEdit_3, &FileEdit::set_in_path, [&](QString s)
            {
        zip->path_in=s;
        ui->lineEdit->setText(zip->path_in); });
    connect(ui->lineEdit_3, &FileEdit::set_out_path, [&](QString s)
            {
            path_in_cor=1;
            path_out_cor=1;
            zip->path_out=s;
            ui->lineEdit_2->setText(zip->path_out); });
    connect(this, &MainWindow::make_zip, zip, &hkzip::process_file_makezip);
    connect(this, &MainWindow::process_zip, zip, &hkzip::process_file);
    connect(zip, &hkzip::zip_finished, this, &MainWindow::zip_finished);
    zip_thread->start(); // 记住
}

void MainWindow::zip_finished()
{
    // 计算压缩比
    msgBox.setWindowTitle("已完成 (┓ ˘ω˘)┛)");
    msgBox.setText("请查看压缩率");
    if (ui->lineEdit_3->bt_show == 1)
    {
        qint64 originalSize = QFile(zip->path_in).size(); // byte
        qint64 compressedSize = QFile(zip->path_out).size();
        double compressionRatio = (double)compressedSize / originalSize;
        QString percent = QString::number(compressionRatio * 100, 'f', 5) + "%";
        /*
            压缩前:originalSize byte
            压缩后:compressedSize byte
            压缩率:percent
        */
        // 创建一个QMessageBox对象
        QMessageBox messageBox;
        // 设置对话框的标题
        messageBox.setWindowTitle("Compression Result");
        messageBox.setText("压缩前:" + QString::number(originalSize) + " byte\n" + "压缩后:" + QString::number(compressedSize) + " byte\n" + "压缩率:" + percent + "\n" + "   ╮(๑•́ ₃•̀๑)╭");
        // 设置对话框的图标，可以选择NoIcon、Information、Warning、Critical、Question等类型
        messageBox.setIcon(QMessageBox::Information);
        // 设置对话框的按钮，可以选择Ok、Yes、No、Cancel等类型，也可以自定义按钮
        messageBox.setStandardButtons(QMessageBox::Ok);
        // 显示对话框
        messageBox.exec();
    }

    msgBox.hide();
    msgBox.setWindowTitle("请等待 ＞﹏＜");
    msgBox.setText("......");
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    QMessageBox::information(this, "提示", " 程序已完成  <(￣︶￣)>", QMessageBox::Ok);
    ui->progressBar->hide();
    ui->pushButton->show();
    ui->progressBar->setValue(0);
    initialize_all();
}

void MainWindow::initialize_all() // bt_show不能改，因为选择按钮不变
{
    path_in_cor = 0;
    path_out_cor = 0;
    memset(zip->binary_coding, 0, sizeof(zip->binary_coding));
    memset(zip->count_char, 0, sizeof(zip->count_char));
    zip->jud_binary_out_finished = 0;
    zip->vec_char_final.clear();
    zip->path_in.clear();
    zip->path_out.clear();
    zip->char_final = std::priority_queue<node *, std::vector<node *>, decltype(&hkzip::compare)>();
    ui->lineEdit_2->setText(zip->path_out);
    ui->lineEdit->setText(zip->path_in);
}

MainWindow::~MainWindow()
{
    delete ui;
    // 调用quit()函数
    zip_thread->quit();
    // 调用wait()函数
    zip_thread->wait();
    delete zip_thread;
}

void MainWindow::on_pushButton_3_clicked()
{
    if (ui->lineEdit_3->bt_show == 1)
    {
        zip->path_in = QFileDialog::getOpenFileName(this, "打开文件", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
        ui->lineEdit->setText(zip->path_in);
        if (!zip->path_in.isEmpty())
        {
            QFileInfo temp(zip->path_in);
            // 获取文件的绝对路径
            QString path = temp.absoluteFilePath();
            // 获取文件的后缀
            QString suffix = temp.suffix();
            // 更改文件的后缀
            zip->path_out = path.replace(suffix, "hkzip");
            ui->lineEdit_2->setText(zip->path_out);
            path_in_cor = 1;
            path_out_cor = 1;
        }
        else
        {
            initialize_all();
        }
    }
    else
    {
        zip->path_in = QFileDialog::getOpenFileName(this, "打开.hkzip文件", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "HKZIP Files (*.hkzip)");
        ui->lineEdit->setText(zip->path_in);
        if (!zip->path_in.isEmpty())
        {
            QFileInfo temp(zip->path_in);
            // 获取文件的绝对路径
            QString path = temp.absoluteFilePath();
            // 获取文件的后缀
            QString suffix = temp.suffix();
            // 更改文件的后缀
            zip->path_out = path.replace(suffix, zip->get_suffix());
            ui->lineEdit_2->setText(zip->path_out);
            path_in_cor = 1;
            path_out_cor = 1;
        }
        else
        {
            initialize_all();
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if (ui->lineEdit_3->bt_show == 1)
    {
        if (path_in_cor == 0)
        {
            QMessageBox::critical(this, "错误", "未选定输入的文件", QMessageBox::Ok);
            ui->lineEdit_2->setText("未选定输入的文件");
        }
        else
        {
            zip->path_out = QFileDialog::getExistingDirectory(this, "选择文件夹", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
            if (!zip->path_out.isEmpty())
            {
                zip->path_out += +"/" + QFileInfo(zip->path_in).baseName() + ".hkzip";
                ui->lineEdit_2->setText(zip->path_out);
            }
            else
            {
                initialize_all();
            }
        }
    }

    else if (ui->lineEdit_3->bt_show == 0)
    {
        if (path_in_cor == 0)
        {
            QMessageBox::critical(this, "错误", "未选定输入的文件", QMessageBox::Ok);
            ui->lineEdit_2->setText("未选定输入的文件");
        }
        else
        {
            // zip->path_out = QFileDialog::getExistingDirectory(this, "选择文件夹", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation))+"/"+QFileInfo(zip->path_in).baseName()+"."+zip->get_suffix();
            zip->path_out = QFileDialog::getExistingDirectory(this, "选择文件夹", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
            if (!zip->path_out.isEmpty())
            {
                zip->path_out += "/" + QFileInfo(zip->path_in).baseName() + "." + zip->get_suffix();
                ui->lineEdit_2->setText(zip->path_out);
            }
            else
            {
                initialize_all();
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{

    if (path_in_cor == 0 || path_out_cor == 0)
    {
        QMessageBox::critical(this, "错误", "未选定输入或者输出的路径", QMessageBox::Ok);
        initialize_all();
        return;
    }
    if (!QFile(zip->path_in).open(QIODevice::ReadOnly)) // 当QFile匿名对象销毁的时候，qfile会自动关闭
    {
        QMessageBox::critical(this, "错误", "文件不可读", QMessageBox::Ok);
        initialize_all();
        return;
    }

    if (QFile(zip->path_in).size() > INT_MAX - 1)
    {
        QMessageBox::warning(this, "警告", "文件太大了，文件大小应小于2147483KB（2.1GB）！！", QMessageBox::Ok);
        initialize_all();
        return;
    }

    // 注意：这是三选
    // 如果用户取消了选择或者发生了错误，则返回QMessageBox::Cancel。因此，如果用户直接点击右上角的关闭按钮，也相当于取消了选择，所以也会返回QMessageBox::Cancel。
    if (QFile(zip->path_out).exists()) // exists()函数只能检查文件是否存在，而不能检查文件是否可写。文件是否可写取决于文件的权限和属性，以及操作,比如文件已经打开，不可写，但是存在
    {
        QMessageBox::StandardButton result = QMessageBox::warning(this, "警告", "文件已存在，是否覆盖", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result == QMessageBox::Cancel)
        {
            initialize_all();
            return;
        }
    }

    // 在这里文件已经创建，如果放在上面的上面，每次都会"警告", "文件已存在，是否覆盖"，但是上面的程序检测了if(!QFile(zip->path_out).open(QIODevice::WriteOnly))吗
    if (!QFile(zip->path_out).open(QIODevice::WriteOnly)) // 文件不存在会返回true吗，就是可写？但是open不了？ReadOnly这种情况一定返回false？
    {
        QMessageBox::critical(this, "错误", "无法写入文件", QMessageBox::Ok);
        initialize_all();
        return;
    }

    if (ui->lineEdit_3->bt_show == 1)
    {
        msgBox.show();

        //    下面这些没必要，msgBox阻塞了其他窗口
        //    zip->process_file_makezip();
        //    ui->pushButton_4->setEnabled(false);
        //    ui->pushButton->setEnabled(false);
        //    ui->pushButton_3->setEnabled(false);
        //    ui->lineEdit_3->setDragEnabled(false);

        // 进度条
        ui->progressBar->setMaximum(0);
        ui->pushButton->hide();
        ui->progressBar->show();
        emit make_zip();
    }
    else if (ui->lineEdit_3->bt_show == 0)
    {
        msgBox.show();
        ui->progressBar->setMaximum(0);
        ui->pushButton->hide();
        ui->progressBar->show();
        emit process_zip();
    }
}

void MainWindow::on_radioButton_clicked()
{
    ui->lineEdit_3->bt_show = 1;
    ui->pushButton->setText("压缩");
    initialize_all();
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->lineEdit_3->bt_show = 0;
    ui->pushButton->setText("解压缩");
    initialize_all();
}
