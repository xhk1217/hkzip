
#ifndef FILEEDIT_H
#define FILEEDIT_H
#include <string.h>
#include <qstring.h>
#include <qfile.h>
#include <queue>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QFileInfo>
#include <QLineEdit>
#include <QDragEnterEvent>
#include <QMimeData>

class FileEdit : public QLineEdit
{
    Q_OBJECT public : FileEdit(QWidget *parent = nullptr);
    // 构造函数
protected:
    // 处理拖入事件
    void dragEnterEvent(QDragEnterEvent *event) override;

    // 处理拖动事件
    void dragMoveEvent(QDragMoveEvent *event) override;

    // 处理放下事件
    void dropEvent(QDropEvent *event) override;

    QString get_suffix(QString path_in);

signals:
    void set_in_path(QString path_in);
    void set_out_path(QString path_out);

public:
    bool bt_show; // 这里决定是解压缩还是压缩模式
};

#endif
