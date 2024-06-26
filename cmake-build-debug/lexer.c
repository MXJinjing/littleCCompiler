#include <stdio.h>
#include <stdlib.h>

#include "includes/lexer.h"

#include <string.h>

#include "includes/grammar_utils.h"
#include "includes/lexer_utils.h"

char *keywords[] = {"int","char","if","else","while","return"};
enum keywords_index{int_, char_, if_, else_, while_, return_};
// 运算符
char *operators[] = {"+", "-", "*", "/", "=", ">",">=", "<", "<=", "==", "!="};

enum operators_index{plus_, minus_, multiply_, divide_, equal_, greater_, greater_equal_, less_, less_equal_, equal_equal_, not_equal_};

char operator_starters[] = {'+', '-', '*', '/', '=', '>', '<','!'};
char operator_followers[] = {'='};


char delimiters[] = {'(', ')','{', '}', ';', ','};

enum delimiters_index{left_parenthesis_, right_parenthesis_, left_square_bracket_, right_square_bracket_, left_curly_bracket_, right_curly_bracket_, semicolon_, comma_};
enum type{id_type, kw_type, op_type, de_type, num_type,ch_type}; // 类别

node * lexer(FILE *input_file)
{
    node *head = lexer_table_init();
    node *end = head; //带一个头指针和一个尾指针的链表，插入时从尾指针插入

    //fprintf(output_file, "name\ttype\tfirst_place\n");
    char c;
    int c_count, row_count, col_count, sum_col_count;
    c_count = 1;
    row_count = 1;
    col_count = 1;
    sum_col_count = 0;
    int is_not_integer = 1;

    while ((c = fgetc(input_file)) != EOF)
    {
        col_count = c_count-sum_col_count;
        //printf("\t\tc: %c, row: %d, col: %d\tc_count: %d, sum_col_count: %d\n", c, row_count, col_count, c_count, sum_col_count);
        
        if (c == '\'') // 忽略单引号中的标识符
        {
            char temp[100];
            int i = 0;
            temp[i++] = c;
            c = fgetc(input_file);
            temp[i++] = c;
            while (1)
            {
                if(c == '\'' && temp[i-2] != '\\')
                {
                    break;
                }
                if(c == '\n')
                {
                    printf("error: cannot find another ' at row %d\n",row_count);
                    exit(1);
                }
                c_count++;
                c = fgetc(input_file);
                temp[i++] = c;
            }
            temp[i] = '\0'; // 字符串结束符
            //printf("char: %s\n", temp);
            if(i != 3)
            {
                printf("error: char length is not 1 at row %d, col %d\n", row_count, col_count);
                exit(1);
            }
            char char_temp = temp[1];
            char char_to_instr[5];
            sprintf(char_to_instr,"%d",temp[1]);
            end = lexer_table_insert_at_end(end, char_to_instr, VT_CHAR_VALUE);
        }
        else if (is_delimiter(c)) // 如果是分隔符，则直接输出
        {
            //printf("delimiter: %c\n", c);
            c_count++;
            char delimiter_string[2] = {c, '\0'};
            end = lexer_table_insert_at_end(end, delimiter_string, get_de_id(c));
        }
        else if (is_operator_starter(c) && is_not_integer) // 如果是运算符开始，则可能是运算符，也可能是正负号
        {
            //printf("operator_starter: %c\n", c);
            char temp[5];
            int i = 0, current_c_count = c_count;
            temp[i++] = c;
            c_count++, c = fgetc(input_file);
            // 判断是加减号还是正负号
            while (is_operator_follower(c))
            { // 如果是运算符的后续字符，继续读取
                temp[i++] = c;
                c_count++, c = fgetc(input_file);
                if (i == 3)
                { // 运算符长度超过2，报错
                    printf("error: the length of operator is too long\n");
                    exit(1);
                }
            }
            // 如果符号只有一位
            // 向后看一个字符，如果是数字，则可能是正负号
            if (i == 1)
            {
                c = fgetc(input_file);
                if (is_number(c) && temp[0] == '+' || temp[0] == '-') //+1 ,-123
                {    
                    // printf("end->token_id = %d\n", end->token_id);
                    //如果前面的是标识符或者数字，那么这个应该不是正负号
                    if(end->token_id == VT_ID || end->token_id == VT_NUMBER)
                    {
                        // printf("is not integer\n");
                        //do nothing here
                    } else
                    {
                        // printf("is integer\n");
                        is_not_integer = 0;
                        
                    }
                }
                fseek(input_file, -1, SEEK_CUR); // 回退一个字符
            }
            if (!is_not_integer)
            {
                // 进入数字识别
                // 回退到上一个字符
                fseek(input_file, -3, SEEK_CUR);
                c = fgetc(input_file);
                c_count--;
                //printf("\t\tretract to %c\n", c);
                continue;
            }
            // 回退一个字符
            fseek(input_file, -1, SEEK_CUR);
            temp[i] = '\0'; // 字符串结束符
            //printf("operator: %s\n", temp);
            // printf("temp = %s\n",temp);
            if (is_operator(temp))
            { // 判断是否是运算符
                end = lexer_table_insert_at_end(end, temp, get_op_id(temp));
            }
            else
            {
                printf("error: unknown operator '%s' at row %d, col %d\n",temp, row_count, current_c_count - sum_col_count);
                exit(1);
            }
        }
        else if (is_alpha_or_underline(c)) // 如果是字母或下划线，则可能是关键词或标识符
        {
            char temp[50];
            int i = 0;
            int current_c_count = c_count;
            temp[i++] = c;

            c_count++, c = fgetc(input_file);
            while (is_alpha_or_underline(c) || is_number(c))
            { // 如果是字母下划线或数字，继续读取
                temp[i++] = c;

                c_count++, c = fgetc(input_file);
                if (i == 50)
                { // 标识符长度超过50，报错
                    printf("error: the length of identifier is too long at row %d, col %d\n", row_count, current_c_count - sum_col_count);
                    exit(1);
                }
            }
            // 回退一个字符
            fseek(input_file, -1, SEEK_CUR);
            temp[i] = '\0'; // 字符串结束符
            if (is_keyword(temp))
            { // 判断是否是关键字
                //printf("keyword: %s\n", temp);
                end = lexer_table_insert_at_end(end, temp, get_kw_id(temp));
            }
            else
            {
                //printf("identifier: %s\n", temp);
                end = lexer_table_insert_at_end(end, temp, VT_ID);
            }
        }
        else if (is_number(c) || (!is_not_integer && (c == '+' || c == '-'))) // 判断是否是整数
        {
            //printf("number start: %c\n", c);
            char temp[50];
            int integer_temp = 0;
            int i = 0, is_negetive = 1;
            is_not_integer = 1;
            if (c == '+' || c == '-')
            {
                is_negetive = (c == '-') ? -1 : 1;
                c_count++, c = fgetc(input_file);
            }
            //printf("is_negetive: %d\n", is_negetive);
            temp[i++] = c;

            c_count++, c = fgetc(input_file);
            while (is_number(c))
            { // 如果是数字，继续读取
                temp[i++] = c;
                c_count++, c = fgetc(input_file);
                if (i == 15)
                { // 数字长度超过15，报错
                    printf("error: the length of number is too long at row %d, col %d\n", row_count, c_count - sum_col_count);
                    exit(1);
                }
            }
            for (int j = 0; j < i; j++) // 将字符串转换为整数
            {
                integer_temp = integer_temp * 10 + temp[j] - '0';
            }
            integer_temp *= is_negetive; // 判断正负
            // 回退一个字符
            fseek(input_file, -1, SEEK_CUR);
            temp[i] = '\0'; // 字符串结束符
            //printf("number: %d\n", integer_temp);
            char intstr[50];
            sprintf(intstr, "%d", integer_temp);
            end = lexer_table_insert_at_end(end, intstr, VT_NUMBER);
        }
        else if (c == '\n')
        { // 如果是换行符，则行数加一
            //printf("==========new line=========\n");
            row_count++;
            // 记录col_count以便计算标识符的位置
            sum_col_count += col_count;
            col_count = 1;
            c_count++;
        }
        else if (c == ' ' || c == '\t' || c=='\r' )
        {
            c_count++;
        }
        else // 如果遇到其他的则显示报错信息
        {
            c_count++;
            printf("error: unknown character '%c' at row %d, col %d\n",c, row_count, c_count - sum_col_count);
            exit(1);
        }
    }
    //printf("==========end=========\n");
    return head;
}


// 判断是否是字母或下划线
int is_alpha_or_underline(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// 判断是否是数字
int is_number(char c)
{
    return (c >= '0' && c <= '9');
}


// 判断是否是关键字
int is_keyword(char *string)
{
    int i;
    for (i = 0; i < KEYWORD_LENGTH; i++)
    {
        if (
            strcmp(string, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int get_kw_id(char *string)
{
    int i;
    for(i = 0; i < KEYWORD_LENGTH; i++)
    {
        if (strcmp(string, keywords[i]) == 0)
        {
            return i+KEYWORD_START;
        }
    }
    return 0;
}

// 判断是否是运算符
int is_operator(char *string)
{
    int i;
    for (i = 0; i < OPERATOR_LENGTH; i++)
    {
        if (strcmp(string, operators[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// 找到运算符的token_id
int get_op_id(char *string)
{
    int i;
    for(i = 0; i < OPERATOR_LENGTH; i++)
    {
        if (strcmp(string, operators[i]) == 0)
        {
            return i+OPERATOR_START;
        }
    }
    return 0;
}


int is_operator_starter(char c)
{
    int i, length = 8;
    for (i = 0; i < length; i++)
    {
        if (operator_starters[i] == c)
        {
            return 1;
        }
    }
    return 0;
}

int is_operator_follower(char c)
{
    int i, length = 1;
    for (i = 0; i < length; i++)
    {
        if (operator_followers[i] == c)
        {
            return 1;
        }
    }
    return 0;
}

// 判断是否是分隔符
int is_delimiter(char c)
{
    int i;
    for (i = 0; i < DELIMITER_LENGTH; i++)
    {
        if (delimiters[i] == c)
        {
            return 1;
        }
    }
    return 0;
}

// 找到分隔符的token_id
int get_de_id(char c)
{
    int i;
    for(i = 0; i < DELIMITER_LENGTH; i++)
    {
        if (delimiters[i] == c)
        {
            return i+DELIMITER_START;
        }
    }
    return 0;
}

// // 找到identifier第一次出现的位置
// int first_place(node *head, char *name, int place)
// {
//     node * p = head->next;
//     while(p != NULL){ // 遍历链表，找到标识符第一个出现的位置
//         if(p->token_id == IDENT_ID && strcmp(p->name, name) == 0)
//         {
//             return p->value;
//         }
//         p = p->next;
//     }
//     return place; //没有则就是当前的位置
// }


// 初始化单词符号表
node * lexer_table_init()
{
    node *head = (node *)malloc(sizeof(node));
    head->name = NULL;
    head->token_id = 0;
    head->next = NULL;
    return head;
}

// 获得单词符号表的尾指针
node * lexer_get_end_pointer(node *head)
{
    node *p = head;
    while (p->next != NULL)
    {
        p = p->next;
    }
    return p;
}

// 在单词符号表的尾部插入一个节点
node * lexer_table_insert_at_end(node *end, char *name, int token_id)
{
    node *new_node = (node *)malloc(sizeof(node));
    char *name_pointer = (char *)malloc(strlen(name) + 1); // 为name分配内存
    strcpy(name_pointer, name);
    new_node->name = name_pointer;
    new_node->token_id = token_id;
    new_node->next = NULL;

    end->next = new_node;
    end = end->next;
    return end;
}

// 打印单词符号表
int print_lexer_table(node *head)
{
    return fprint_lexer_table(head, stdout);
}

int fprint_lexer_table(node * head, FILE * fp)
{
    int i = 1;
    node *p = head->next;
    if(p == NULL)
    {
        fprintf(fp,"No token found\n");
        return 1;
    }
    fprintf(fp,"\nNo\tname\ttoken->id\n");
    while (p != NULL)
    {
        char *type;
        fprintf(fp,"%d.\t%s\t%4d\n", i++, p->name, p->token_id);
        p = p->next;
    }
    return 0;
}

enum type get_type_by_id(int token_id)
{
    if(token_id >= KEYWORD_START && token_id < OPERATOR_START)
    {
        return kw_type;
    } else if(token_id >= OPERATOR_START && token_id < DELIMITER_START)
    {
        return op_type;
    } else if(token_id >= DELIMITER_START)
    {
        return de_type;
    } else if(token_id == VT_NUMBER)
    {
        return num_type;
    } else if(token_id == VT_CHAR_VALUE)
    {
        return ch_type;
    } else if(token_id == VT_ID)
    {
        return id_type;
    }
}



