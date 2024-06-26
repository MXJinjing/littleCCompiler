#ifndef QUAD_UTIL_H
#define QUAD_UTIL_H
#include <stdio.h>

// 四元组链表
typedef struct quadruple{
    char * op;
    char * arg1;
    char * arg2;
    char * rst;
    struct quadruple * next;
} quad;


typedef struct attr{
    int is_temp;
    char * value;
}attr;

typedef struct symbol_table{
    char * name;
    int temp_id;
    int size;
    struct symbol_table * next;
}symtab;

typedef struct output_format{
    quad * qhead;
    symtab * shead;
}ofm;

attr* new_attr(int is_temp, char * value);

void print_quad(quad * q);

void fprint_quad(quad * q, FILE * output);
void fprint_quad_clean(quad * q, FILE * output);

quad * new_quad();

quad * insert_quad(quad * end, char* op, char* arg1, char* arg2, char* rst);

symtab * new_symtab();

symtab * add_sym(symtab* s,char * name, int temp_id,int size);

void print_symtab(symtab * q);

void fprint_symtab(symtab * q, FILE * output);

void print_quad_with_label2(quad * q ,int label_count, int label_location[]);

void fprint_quad_with_label2( quad * q,int label_count, int label_location[], FILE * out);





#endif //QUAD_UTIL_H