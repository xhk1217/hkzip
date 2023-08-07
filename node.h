
#ifndef NODE_H
#define NODE_H
#include <string>

class node
{
public:
    node(int _count = 0, unsigned char _a = 0);
    unsigned char a;
    int count; // 权重
    node *lchild;
    node *rchild;
    std::string binary_coding;
};

#endif // NODE_H
