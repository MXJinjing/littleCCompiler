#include <stdlib.h>
#ifndef GRAPH_UTILS_C
#define GRAPH_UTILS_C
#include "includes/graph.h"

// 初始化一个新的表格
graph * graph_init(){
    graph* g1 = (graph*)malloc(sizeof(graph));
    g1->top =0;
    return g1;
}

// 创建图节点
void create_vertex(graph * g1, int data){
    g1->vertex[g1->top].data = data;
    g1->vertex[g1->top].weight = EMPTY;
    g1->vertex[g1->top].next = NULL;
    g1->top++;
}


// 编辑图节点
void edit_vertex(graph * g1, int i, int data, int weight){
    g1->vertex[i].data = data;
    g1->vertex[i].weight = weight;
}

// 删除图节点
void remove_vertex(graph * g1, int v){
    // 寻找出口为i节点的边并删除
    vnode* p,*prev;
    for(int i=0; i<g1->top; i++){
        p = &(g1->vertex[i]);
        prev = p, p = p->next;
        while(p != NULL){
            if(p->data == v){
                prev->next = p->next;
                free(p);
                break;
            }
            prev = p, p = p->next;
        }
        printf("\n");
    }
}

// 创建边
void create_edge(graph * g1,int v1, int v2,int weight){
    if(v1 > g1->top || v2 > g1->top){
        printf("error: vertex %d out of range!\n",v1>v2?v1:v2);
        return;
    } 
    vnode* p = &(g1->vertex[v1]);
    vnode* e1 = (vnode*)malloc(sizeof(vnode));
    e1->data = v2;
    e1->weight = weight;
    e1->next = NULL;
    while(1){
        if(p->data == v2 && p->weight == weight){
            printf("error: edge %d->%d (weight:%d) already exists!\n",v1,v2,weight);
            return;
        }
        if(p->next == NULL)
        {
            p->next = e1;
            return;
        }
        p = p->next;
    }
}

// 更改边的权重（转换条件）
void edit_edge(graph * g1, int v1, int v2, int weight){
    if(v1 > g1->top || v2 > g1->top){
        printf("error: vertex %d out of range!\n",v1>v2?v1:v2);
        return;
    } 
    vnode* p = &(g1->vertex[v1]),* prev;
    if(p->next == NULL)return;
    prev = p, p = p->next;
    while(p != NULL)
    {
        if(p->data == v2)
        {
            p->weight = weight; // 更新权重
            return;
        }
        prev = p, p = p->next;
    }
}

// 删除边
void delete_edge(graph * g1, int v1, int v2){
    if(v1 > g1->top || v2 > g1->top){
        printf("error: vertex %d out of range!\n",v1>v2?v1:v2);
        return;
    } 
    vnode* p = &(g1->vertex[v1]),* prev;
    if(p->next == NULL)return;
    prev = p, p = p->next;
    while(p != NULL)
    {
        if(p->data == v2)
        {
            prev->next = p->next;
            free(p);    //释放内存空间
            return;
        }
        prev = p, p = p->next;
    }
}

// 打印邻接表
void print_graph(graph * g1){
    vnode* p;
    printf("g->top = %d\n",g1->top);
    for(int i=0; i<g1->top; i++){
        printf("V%d\t",i);
        p = &(g1->vertex[i]);
        while(p != NULL){
            printf("%d|%d->",p->data,p->weight);
            p = p->next;
        }
        printf("\n");
    }
}

#endif