#ifndef TREE_H
#define TREE_H
#include "tree_utils.h"
#include <stdio.h>


// 返回一个新的树节点
tnode * new_tree(int id);

// 在树中添加一个节点
tnode * add_tnode(tnode * root, int id, int is_left);

// 查找树中的节点
tnode * find_tnode(tnode * root, int id);

// 使用二叉树来代表语法树，左子树代表孩子节点，右子树代表兄弟节点

// 添加孩子节点
tnode * add_child(tnode * parent, int child_id);

// 添加兄弟节点
tnode * add_brother(tnode * parent, int brother_id);

// 打印树
int fprint_tree(tnode * root, FILE * fp);
int fprint_tree__(tnode * root, FILE * fp, int depth);
int print_tree(tnode * root);


// 打印语法树
int fprint_grammar_tree(tnode * root, FILE * fp);
int fprint_grammar_tree__(tnode * root, FILE * fp, int depth);
int print_grammar_tree(tnode * root);


int free_tree(tnode * root);

#endif //TREE_H
