
#include "fileedit.h"

FileEdit::FileEdit(QWidget *parent) : QLineEdit(parent)
{
    setDragEnabled(true); // 启用拖放功能
    bt_show = 1;          // 初始化
}

void FileEdit::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *data = event->mimeData(); // 获取拖入的数据
    QList<QUrl> urls = data->urls();           // 获取拖入的文件的路径
    if (urls.size() == 1 && urls[0].isLocalFile())
    {                                  // 如果只有一个本地文件
        event->acceptProposedAction(); // 接受事件   什么意思
    }
}

void FileEdit::dragMoveEvent(QDragMoveEvent *event)
{
    const QMimeData *data = event->mimeData(); // 获取拖入的数据
    QList<QUrl> urls = data->urls();           // 获取拖入的文件的路径
    if (urls.size() == 1 && urls[0].isLocalFile())
    {                                  // 如果只有一个本地文件
        event->acceptProposedAction(); // 接受事件  什么意思
    }
}

void FileEdit::dropEvent(QDropEvent *event)
{
    QString path_in, path_out;
    const QMimeData *data = event->mimeData();     // 获取拖入的数据
    QList<QUrl> urls = data->urls();               // 获取拖入的文件的路径
    if (urls.size() == 1 && urls[0].isLocalFile()) // 如果只有一个本地文件
    {
        // 如果是文件夹就返回
        if (!QFileInfo(urls[0].toLocalFile()).isFile())
        {
            QMessageBox::information(this, "错误", "文件不能为文件夹", QMessageBox::Ok);
            return;
        }

        if (bt_show == 1)
        {

            // path_in的初始化
            path_in = urls[0].toLocalFile();
            if (!path_in.isEmpty())
            {
                // 发送信号
                emit set_in_path(path_in);
                QFileInfo temp(path_in);
                // 获取文件的绝对路径
                QString path = temp.absoluteFilePath();
                // 获取文件的后缀
                QString suffix = temp.suffix();
                // 更改文件的后缀
                path_out = path.replace(suffix, "hkzip");
                emit set_out_path(path_out);
            }
        }
        else // bt_show==0
        {

            path_in = urls[0].toLocalFile();
            // 判断文件格式
            if (QFileInfo(path_in).suffix() != "hkzip")
            {
                QMessageBox::information(this, "提示", "文件格式应为.hkzip", QMessageBox::Ok);
                path_in.clear();
                return;
            }
            emit set_in_path(path_in);

            if (!path_in.isEmpty())
            {
                QFileInfo temp(path_in);
                // 获取文件的绝对路径
                QString path = temp.absoluteFilePath();
                // 获取文件的后缀
                QString suffix = temp.suffix();
                // 更改文件的后缀
                path_out = path.replace(suffix, get_suffix(path_in));
                emit set_out_path(path_out);
            }
        }
    }
}

QString FileEdit::get_suffix(QString path_in) // 得到压缩前文件后缀
{
    QFile infile(path_in);
    infile.open(QIODevice::ReadOnly);

    std::string temp_str_suffix;
    char temp_char_suffix;
    while (infile.read((char *)&temp_char_suffix, 1))
    {
        if (temp_char_suffix == '\0')
        {
            infile.close();
            return QString::fromStdString(temp_str_suffix);
        }
        else
        {
            temp_str_suffix += temp_char_suffix;
        }
    }
}
