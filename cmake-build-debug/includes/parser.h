#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include <stdio.h>

#include "node.h"
#include "grammar_utils.h"
#include "tree_utils.h"
#include "lexer.h"

// 函数声明
tnode* p_prog();
void p_block(tnode *block);
void p_decls(tnode *decls);
void p_decl(tnode *decl);
void p_type(tnode *type);
void p_names(tnode *names);
void p_names_(tnode *names_);
void p_name(tnode *name);
void p_stmts(tnode *stmts);
void p_stmt(tnode *stmt);
void p_elsestmt(tnode *elsestmt);
void p_expr(tnode *expr);
void p_expr_(tnode *expr_);
void p_add(tnode *add);
void p_term(tnode *term);
void p_term_(tnode *term_);
void p_mul(tnode *mul);
void p_factor(tnode *factor);
void p_rel(tnode *rel);
void p_rop(tnode *rop);
tnode *ed_parsing(node *head);
void init_first_set();
char *get_v_name(int v);
void print_first_set();
int in_first(int vn, node *current);
int fprint_grammar_tree(tnode * root, FILE * fp);
int print_grammar_tree(tnode * root);

#endif // PARSER_H
