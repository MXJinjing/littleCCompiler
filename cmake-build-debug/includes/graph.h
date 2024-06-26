#ifndef GRAPH_H
#define GRAPH_H
#include "graph_utils.h"
graph * graph_init();
void create_vertex(graph * g1, int data);   // 创建图节点
void edit_vertex(graph * g1, int i, int data, int weight);  //编辑图节点
void remove_vertex(graph * g1, int i);  // 删除图节点
void create_edge(graph * g1,int v1, int v2 ,int weight);    // 创建边
void edit_edge(graph * g1, int v1, int v2, int weight);
void delete_edge(graph * g1, int v1, int v2);   // 删除边
void print_graph(graph * g1);   // 打印图的邻接表
#endif //GRAPH_H
