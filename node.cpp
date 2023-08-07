
#include "node.h"

node::node(int _count, unsigned char _a) // 在这里不用再赋值一次初始值了
{
    a = _a;
    count = _count;
    lchild = NULL;
    rchild = NULL;
}
