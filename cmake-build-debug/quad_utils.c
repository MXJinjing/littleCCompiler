#include "includes/quad_utils.h"

#include <stdlib.h>
#include <string.h>


void print_quad(quad * q){
    fprint_quad(q,stdout);
}

void fprint_quad(quad * q,FILE * output){
    fprintf(output,"\n===Quad Table====\n");
    quad * p = q->next; //skip head
    while(p!=NULL){
        fprintf(output,"( %s, %s, %s, %s )\n",p->op,p->rst,p->arg1,p->arg2);
        p = p->next;
    }
    fprintf(output,"==================\n");
}

void fprint_quad_clean(quad * q,FILE * output){
    quad * p = q->next; //skip head
    while(p != NULL)
    {
        if(strcmp(p->op,"" )!=0)
        {
            fprintf(output,"%s  ",p->op);
        }
        if(strcmp(p->rst,"" )!=0)

        {
            fprintf(output,"%s, ",p->rst);
        }
        if(strcmp(p->arg1,"" )!=0)

        {
            fprintf(output,"%s, ",p->arg1);
        }
        if(strcmp(p->arg2,"" )!=0)
        {
            fprintf(output,"%s, ",p->arg2);
        }
        fprintf(output,"\n");
        p = p->next;
    }
}

attr* new_attr(int is_temp, char * value){
    attr * a = (attr *) malloc(sizeof (attr));
    a->is_temp = is_temp;
    a->value = (char *) malloc(sizeof (value));
    strcpy(a->value,value);
    return a;
}

quad * new_quad(){
    quad * new_head = (quad *)malloc(sizeof(quad));
    new_head->op == NULL;
    new_head->arg1 == NULL;
    new_head->arg2 == NULL;
    new_head->rst == NULL;
    return new_head;
}


quad * insert_quad(quad * end, char* op, char* arg1, char* arg2, char* rst){
    // 创建新节点
    quad * q = (quad *)malloc(sizeof(quad));

    // 将分配好的字符串拷贝到新节点中
    q->op = (char *)malloc(strlen(op) + 1);
    q->arg1 = (char *)malloc(strlen(arg1) + 1);
    q->arg2 = (char *)malloc(strlen(arg2) + 1);
    q->rst = (char *)malloc(strlen(rst) + 1);

    strcpy(q->op,op);
    strcpy(q->arg1,arg1);
    strcpy(q->arg2,arg2);
    strcpy(q->rst,rst);
    q->next = NULL;

    // 将新节点插入到链表中
    end->next = q;

    // 返回链表末尾指针
    return q;
}

symtab* new_symtab(){
    char * empty = "";
    symtab * s1 = (symtab *) malloc(sizeof (symtab));
    s1->name = NULL;
    s1->temp_id = 0;
    s1->size = 0;
    s1->next = NULL;

    return s1;
}

symtab * add_sym(symtab* end,char * name, int temp_id, int size){
    // 创建新节点
    symtab * s = (symtab *)malloc(sizeof(symtab));

    // 将分配好的字符串拷贝到新节点中
    s->name = (char *)malloc(strlen(name) + 1);
    strcpy(s->name,name);

    s->temp_id = temp_id;
    s->size = size;
    s->next = NULL;

    // 将新节点插入到链表中
    end->next = s;


    // 返回链表末尾指针
    return s;
}

void fprint_symtab(symtab * q, FILE * output){
    symtab * p = q->next; //skip head
    fprintf(output,"\n===Symbol Table===\n");
    while(p!=NULL){
        fprintf(output,"( %s , T%d , %d byte)\n",p->name,p->temp_id,p->size);
        p = p->next;
    }
    fprintf(output,"==================\n");
}

void print_symtab(symtab * q){
    fprint_symtab(q,stdout);
}

void print_quad_with_label2(quad * q ,int label_count, int label_location[])
{
    fprint_quad_with_label2(q,label_count, label_location, stdout);
}

void fprint_quad_with_label2( quad * q,int label_count, int label_location[], FILE * out)
{
    fprintf(out,"\n===Quad Table with LABEL====\n");
    quad * p = q->next; //skip head
    int i = 0,line = 0;
    fprintf(out,"START:\n");
    while(p != NULL)
    {
        for (i = 0; i < label_count; i ++)
        {
            if(label_location[i] == line)
            {
                fprintf(out,"LABEL %d:\n",i);
            }
        }
        printf("    ");
        if(strcmp(p->op,"" )!=0)
        {
            fprintf(out,"%s  ",p->op);
        }
        if(strcmp(p->rst,"" )!=0)

        {
            fprintf(out,"%s, ",p->rst);
        }
        if(strcmp(p->arg1,"" )!=0)

        {
            fprintf(out,"%s, ",p->arg1);
        }
        if(strcmp(p->arg2,"" )!=0)
        {
            fprintf(out,"%s, ",p->arg2);
        }
        fprintf(out,"\b\b \n");
        p = p->next;
        line++;
    }
    for (i = 0; i < label_count; i ++)
    {
        if(label_location[i] == line)
        {
           fprintf(out,"LABEL %d:\n",i);
        }
    }

    printf("=============================\n");
}
