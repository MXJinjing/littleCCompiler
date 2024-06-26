#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "node.h"


node *lexer(FILE *input_file);
int is_alpha_or_underline(char c);
int is_number(char c);
int is_keyword(char *string);
int get_kw_id(char *string);
int is_operator(char *string);
int get_op_id(char *string);
int is_operator_starter(char c);
int is_operator_follower(char c);
int is_delimiter(char c);
int get_de_id(char c);
node *lexer_table_init();
node *lexer_get_end_pointer(node *head);
node *lexer_table_insert_at_end(node *end, char *name, int token_id);
int print_lexer_table(node *head);
int fprint_lexer_table(node *head, FILE *fp);
enum type get_type_by_id(int token_id);

enum type get_type_by_id(int token_id); // 通过id获取类别

#endif /* LEXER_H */
