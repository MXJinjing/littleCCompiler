#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#define EMPTY -1
#define AUTOMATAEND -2

// 使用邻接表来存储图
typedef struct vertex_node{
    int data;   //节点名称（连接到下一个节点的名称）
    int weight; //权重（自动机转换的条件）
    struct vertex_node * next;  //下一个node的位置
}vnode;

typedef struct graph{
    int top;  // 图的节点数
    vnode vertex[100];  // 最大节点数目为100
}graph;



#endif