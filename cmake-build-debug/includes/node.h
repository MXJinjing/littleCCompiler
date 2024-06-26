#ifndef NODE_H
#define NODE_H
//存放单词符号的表
typedef struct table_node{
    int token_id;  // 标识符的id, 用于区分不同的标识符
    char *name; // 标识符的名字

    struct table_node *next;
} node;

#endif
