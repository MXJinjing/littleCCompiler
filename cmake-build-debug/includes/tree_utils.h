#ifndef TREE_UTILS_H
#define TREE_UTILS_H

typedef struct tree_node{
    int id;
    struct tree_node * left;
    struct tree_node * right;
} tnode;

#endif