#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include <stdbool.h>
#include <stdio.h>

#include "node.h"
#include "grammar_utils.h"
#include "tree_utils.h"
#include "lexer.h"
#include "quad_utils.h"

// 函数声明
void p2_prog();
void p2_block();
void p2_decls();
void p2_decl();
int p2_type();
void p2_names(int type);
void p2_names_(int type);
void p2_name(int type);
void p2_stmts();
void p2_stmt();
void p2_elsestmt();
attr* p2_expr();
attr* p2_expr_(attr* left);
int p2_add();
attr* p2_term();
attr* p2_term_(attr * left);
int p2_mul();
attr*  p2_factor();
quad* p2_rel(quad* temp);
int  p2_rop();
int find_temp_id(char * name);
ofm * generate_intercode(node *head,ofm*o);
int get_new_temp();
int get_new_label();
void set_label(int label_id);

void fill_label(quad * q);
void print_quad_with_label(quad * q);

#endif // TRANSLATOR_H
