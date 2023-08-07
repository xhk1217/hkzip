
#ifndef HKZIP_H
#define HKZIP_H
#include <string>
#include <vector>
#include <queue>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "node.h"

class hkzip : public QObject
{
    Q_OBJECT
private:
    void create_zip_file();
    void binary_coding_tree(node *n);
    node *root;
    node *now;

public:
    hkzip();
    QString path_in;
    QString path_out;

    // 每次完成需要初始化，所以public便于操作，或者用信号和槽函数
    static bool compare(node *n1, node *n2); // 记住static //外部初始化char_final有用
    void jud_binary_out(bool b);
    std::priority_queue<node *, std::vector<node *>, decltype(&hkzip::compare)> char_final; // 记住要std，当然qt中也有对应容器
    int count_char[256];
    std::vector<node *> vec_char_final;
    std::string binary_coding[256];
    bool jud_binary_out_finished;
    QString get_suffix();

    void process_file();         // 解压缩
    void process_file_makezip(); // 压缩
    void delete_node(node *n);   // 释放空间

signals:
    void zip_finished(); // 已完成
};

#endif
