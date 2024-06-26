#include <stdlib.h>
#ifndef TREE_UTILS_C
#define TREE_UTILS_C
#include "includes/tree.h"

tnode * new_tree(int id)
{
    tnode * new_node = (tnode *)malloc(sizeof(tnode));
    new_node->id = id;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

tnode * add_tnode(tnode * root, int id, int is_left)
{
    tnode * new_node = new_tree(id);
    if (is_left)
    {
        root->left = new_node;
    }
    else
    {
        root->right = new_node;
    }
    return new_node;
}
tnode * find_tnode(tnode * root, int id){
    if (root == NULL)
    {
        return NULL;
    }
    if (root->id == id)
    {
        return root;
    }
    tnode * left = find_tnode(root->left, id);
    if (left != NULL)
    {
        return left;
    }
    tnode * right = find_tnode(root->right, id);
    if (right != NULL)
    {
        return right;
    }
    return NULL;
}

tnode * add_child(tnode * parent, int child_id)
{
    tnode * new_child = new_tree(child_id);
    if (parent == NULL)
    {
        return NULL;
    }
    if (parent->left == NULL)
    {
        parent->left = new_child;
    }
    else
    {
        tnode * temp = parent->left;
        while (temp->right != NULL)
        {
            temp = temp->right;
        }
        temp->right = new_child;
    }
    return new_child;
}

tnode * add_brother(tnode *parent, int brother_id)
{
    tnode * new_brother = new_tree(brother_id);
    if (parent == NULL)
    {
        return NULL;
    }
    tnode * temp = parent;
    while (temp->right != NULL)
    {
        temp = temp->right;
    }
    temp->right = new_brother;
    return new_brother;
}


// 打印树节点
// 使用tree命令的格式打印树
int fprint_tree(tnode * root, FILE * fp)
{
    return fprint_tree__(root,fp,0);
}

int fprint_tree__(tnode * root,FILE * fp, int depth)
{
    tnode * temp = root;
    while(temp != NULL){
        if(1) {
            for(int i = 0; i < depth; i++)
                fprintf(fp,"|  "); // 打印空格，用来表示树的深度
            fprintf(fp,"|--"); // 打印树的连接线
        }
        fprintf(fp,"%d\n",temp->id,depth); // 打印节点的id
        fprint_tree__(temp->left,fp,depth+1); // 打印左子树,深度加1
        if (temp->right == NULL)
        {
            for(int i = 0; i < depth; i++)
                fprintf(fp,"|  "); // 打印空格，用来表示树的深度
            fprintf(fp,"\n"); // 空出一行
            return 0;
        }
        temp = temp->right; // 移动到右子树
    }
    return 0;
}

int print_tree(tnode * root)
{
    return fprint_tree(root,stdout);
}

int free_tree(tnode * root)
{
    if (root == NULL)
    {
        return 0;
    }
    free_tree(root->left);
    free_tree(root->right);
    free(root);
    return 0;
}

#endif