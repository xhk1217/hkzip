
#include "hkzip.h"

void hkzip::delete_node(node *n)
{
    if (n == NULL)
        return;
    delete_node(n->lchild);
    delete_node(n->rchild);
    delete n;
}

hkzip::hkzip() : QObject(NULL), count_char{0}, binary_coding{}
{
    jud_binary_out_finished = 0;
}

bool hkzip::compare(node *n1, node *n2)
{
    return n1->count > n2->count; // 这里先后之后再试一试
}

void hkzip::process_file_makezip()
{
    // 预处理，统计数目
    QFile infile(path_in);            // 创建一个QFile对象，指定要读取的文件名
    infile.open(QIODevice::ReadOnly); // 以只读模式打开文件
    unsigned char temp;               // 定义一个无符号字符变量，用于存储读取到的字节
    while (!infile.atEnd())
    {                                                    // 循环读取文件，直到结束
        infile.read(reinterpret_cast<char *>(&temp), 1); // 读取一个字节，转换成char类型的指针
        count_char[temp]++;                              // 统计该字节出现的次数
    }
    infile.close(); // 关闭文件

    // 初始化vec
    for (int i = 0; i < 256; i++)
    {
        if (count_char[i] != 0)
        {
            node *temp_n = new node(count_char[i], i);
            vec_char_final.push_back(temp_n);
        }
    }

    // 创建优先队列
    char_final = std::priority_queue<node *, std::vector<node *>, decltype(&hkzip::compare)>(vec_char_final.begin(), vec_char_final.end(), &hkzip::compare);
    // cout << char_final.top()->count;//测试//发现是最低的在队列头

    // 创建哈夫曼树
    while (char_final.size() != 1) // 就剩一个的时候是根节点
    {
        node *temp_n1 = char_final.top();
        char_final.pop();
        node *temp_n2 = char_final.top();
        char_final.pop();
        node *temp_n3 = new node(temp_n1->count + temp_n2->count, 0);
        temp_n3->lchild = temp_n1;
        temp_n3->rchild = temp_n2;
        char_final.push(temp_n3);
    }
    root = char_final.top();

    // 赋予编码
    binary_coding_tree(root);

    // 将编码写入数组
    for (int i = 0; i < vec_char_final.size(); i++)
    {
        binary_coding[vec_char_final[i]->a] = vec_char_final[i]->binary_coding; // a对应unsigned
    }

    // 这样只要执行一个程序
    create_zip_file();

    delete_node(root);
    emit zip_finished();
}

void hkzip::create_zip_file()
{
    QFile outfile(path_out);
    outfile.open(QIODevice::WriteOnly);

    // 写入后缀名,以及一个\0
    std::string str_suffix = QFileInfo(path_in).suffix().toStdString();
    char char_0 = '\0';
    for (int i = 0; i < str_suffix.size(); i++)
    {
        outfile.write((char *)&str_suffix[i], 1);
    }
    outfile.write(&char_0, 1);

    // 写入数组
    outfile.write((char *)count_char, 256 * sizeof(int)); // 不对，写入顺序即可以//写的可以更少吗，怎么简化

    // 写入压缩后的编码
    std::string temp_string;
    QFile infile(path_in);
    infile.open(QIODevice::ReadOnly);
    unsigned char temp_char_in = 0;
    while (infile.read((char *)&temp_char_in, 1)) // 要是我填>1这时最后只剩下三个
    {
        temp_string += binary_coding[temp_char_in];
        while (temp_string.size() > 8)
        {
            unsigned char temp_char_out = 0;
            for (int i = 0; i < 8; i++)
            {
                temp_char_out = temp_char_out * 2 + temp_string[i] - '0'; // 无符号扩展
                // 或者强制转换为int计算就不会有问题
            }
            temp_string.erase(0, 8);
            outfile.write((char *)&temp_char_out, 1);
        }
    }
    infile.close(); // 关闭

    // 最后还剩下一点，特殊处理，也就是记录下倒二个字符为后temp_char_final_num个
    unsigned char temp_char_out_final = 0;
    for (int i = 0; i < temp_string.size(); i++)
    {
        temp_char_out_final = temp_char_out_final * 2 + temp_string[i] - '0';
    }
    char temp_char_final_num;
    temp_char_final_num = temp_string.size();
    outfile.write((char *)&temp_char_out_final, 1);
    outfile.write(&temp_char_final_num, 1);
    outfile.close();
}

void hkzip::binary_coding_tree(node *n)
{
    static std::string temp;
    if (n == NULL)
    {
        return;
    }
    n->binary_coding = temp;

    temp += '0';
    binary_coding_tree(n->lchild);
    temp.pop_back();

    temp += '1';
    binary_coding_tree(n->rchild);
    temp.pop_back();
}

void hkzip::jud_binary_out(bool b) // 写入解压缩文件
{
    static QFile *outfile = NULL;
    if (outfile == NULL)
    {
        outfile = new QFile(path_out);
    }
    if (!outfile->isOpen())
    {
        outfile->open(QIODevice::WriteOnly); // 重复打开会重置文件指针
    }

    if (jud_binary_out_finished == 1)
    {
        outfile->close();
        delete outfile;
        outfile = NULL;
        jud_binary_out_finished = 0;
        return;
    }
    if (b == 0)
    {
        now = now->lchild;
    }
    else
    {
        now = now->rchild;
    }
    if (now->lchild == NULL && now->rchild == NULL) // 前后只要一个应该就可以了
    {
        outfile->write((char *)&(now->a), 1);
        now = root;
    }
}

void hkzip::process_file()
{
    // 跳过写入的文件后缀名
    QFile infile(path_in);
    infile.open(QIODevice::ReadOnly);
    char temp_char_find0;
    while (infile.read((char *)&temp_char_find0, 1))
    {
        if (temp_char_find0 == '\0')
        {
            break;
        }
    }

    // 输入数组
    infile.read((char *)count_char, 256 * sizeof(int));

    // 统计数目
    int count = 0;
    unsigned char count_temp_char;
    while (infile.read((char *)&count_temp_char, 1))
    {
        count++;
    }
    infile.close();

    // 初始化vec
    for (int i = 0; i < 256; i++)
    {
        if (count_char[i] != 0)
        {
            node *temp_n = new node(count_char[i], i);
            vec_char_final.push_back(temp_n);
        }
    }

    // 创建优先队列
    char_final = std::priority_queue<node *, std::vector<node *>, decltype(&hkzip::compare)>(vec_char_final.begin(), vec_char_final.end(), &hkzip::compare);
    // 成员函数的地址必须加上::还有&

    // 创建哈夫曼树
    while (char_final.size() != 1)
    {
        node *temp_n1 = char_final.top();
        char_final.pop();
        node *temp_n2 = char_final.top();
        char_final.pop();
        node *temp_n3 = new node(temp_n1->count + temp_n2->count, 0);
        temp_n3->lchild = temp_n1;
        temp_n3->rchild = temp_n2;
        char_final.push(temp_n3);
    }
    root = char_final.top();

    // 赋予编码
    binary_coding_tree(root);

    // 准备工作完成

    // 跳过写入的文件后缀名以及数组
    QFile infile2(path_in);
    infile2.open(QIODevice::ReadOnly);
    while (infile2.read((char *)&temp_char_find0, 1)) // infile2,不是infile1，找了3个小时
    {
        if (temp_char_find0 == '\0')
        {
            break;
        }
    }
    infile2.read((char *)count_char, 256 * sizeof(int)); // 为了让读取的标号到指定的位置

    // 解压缩，写入文件
    unsigned char temp_char = 0;
    now = root; // 初始化now
    for (int i = 0; i < count - 2; i++)
    {
        infile2.read((char *)&temp_char, 1);
        int k = temp_char;
        for (int j = 7; j >= 0; j--)
        {
            jud_binary_out(((k >> j) & 1));
        }
    }

    // 最后两个字节特殊处理
    unsigned char temp_char2 = 0;
    infile2.read((char *)&temp_char, 1);
    infile2.read((char *)&temp_char2, 1);
    for (int i = temp_char2 - 1; i >= 0; i--) // 记得要减1
    {
        jud_binary_out(((temp_char >> i) & 1));
    }
    jud_binary_out_finished = 1;
    jud_binary_out(1);

    delete_node(root);

    emit zip_finished();
}

QString hkzip::get_suffix() // 读取文件，得到文件的后缀名
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
    infile.close();
}
