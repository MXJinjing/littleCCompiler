#include "includes/translator.h"
#include "includes/grammar_utils.h"
#include "includes/parser.h"
#include "includes/quad_utils.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

int label_location[100];
int temp_count = 1;
int label_count = 0;
int line_count = 0;

node* current2 = NULL;

quad* q;
quad* qhead;

symtab* s;
symtab* shead;

//防止重名
void go_next2() // 读取下一个节点
{
    current2 = current2->next;
}


// 递归下降分析
ofm* generate_intercode(node* head, ofm* o)
{
    for (int i=0; i<50; i++)
    {
        label_location[i] = -1;
    }
    init_first_set(); // 初始化first集合
    //print_first_set();        // 打印first集合
    current2 = head; // 当前节点指向头节点

    qhead = q = new_quad(); //初始化四元组
    shead = s = new_symtab(); //记录已经声明的变量

    go_next2(); // 读取第一个节点
    p2_prog(); // 进入递归下降分析
    // print_quad_with_label(qhead);
    fill_label(qhead); // 将LABEL用数字填充

    //输出的格式。封装两个指针
    o = (ofm*)malloc(sizeof(ofm));
    o->qhead = qhead;
    o->shead = shead;

    return o;
}

// 起始节点 PROG → BLOCK
void p2_prog()
{
    if (in_first(VN_PROG, current2))
    {
        p2_block();
    }
    else
    {
        printf("error: PROG -> BLOCK\n");
    }
}

// BLOCK → { DECLS STMTS }
void p2_block()
{
    if (in_first(VN_BLOCK, current2))
    {
        if (current2->token_id == VT_CURLY_L)
        {
            go_next2();
            if (in_first(VN_DECLS, current2))
            {
                p2_decls();
                if (in_first(VN_STMTS, current2))
                {
                    p2_stmts();
                    if (current2->token_id == VT_CURLY_R)
                    {
                        go_next2();
                    }
                    else
                    {
                        printf("error: lack of }\n");
                    }
                }
            }
        }
        else
        {
            printf("error: can't find {\n");
        }
    }
}

// DECLS → DECL DECLS | empty
void p2_decls()
{
    // printf("into p2_decls\n");
    if (in_first(VN_DECLS, current2))
    {
        if (current2->token_id == VT_INT || current2->token_id == VT_CHAR)
        {
            p2_decl();
            if (in_first(VN_DECLS, current2))
            {
                p2_decls();
            }
        }
        else
        {
            // 空产生式
            return;
        }
    }
}

// DECL → TYPE NAMES ;
void p2_decl()
{
    // printf("into p2_decl\n");
    if (in_first(VN_DECL, current2))
    {
        if (in_first(VN_TYPE, current2))
        {
            int type = p2_type();

            if (in_first(VN_NAMES, current2))
            {
                p2_names(type);
                if (current2->token_id == VT_SEMICOLON)
                {
                    go_next2();
                }
                else
                {
                    printf("error: lack of ; 1\n");
                }
            }
        }
    }
}

// TYPE → int | char
int p2_type()
{
    // printf("into p2_type\n");
    if (in_first(VN_TYPE, current2))
    {
        if (current2->token_id == VT_INT)
        {
            go_next2();
            return VT_INT;
        }
        if (current2->token_id == VT_CHAR)
        {
            go_next2();
            return VT_CHAR;
        }
    }
    else
    {
        printf("error: TYPE -> int | char\n");
        return 0;
    }
    return 0;
}

// NAMES → NAME NAMES_
void p2_names(const int type)
{
    // printf("into p2_names\n");
    if (in_first(VN_NAMES, current2))
    {
        if (in_first(VN_NAME, current2))
        {
            p2_name(type);
            if (in_first(VN_NAMES_, current2))
            {
                p2_names_(type);
            }
        }
    }
}

// NAMES_ → , NAMES | empty
void p2_names_(const int type)
{
    // printf("into p2_names_\n");
    if (in_first(VN_NAMES_, current2))
    {
        if (current2->token_id == VT_COMMA)
        {
            go_next2();
            if (in_first(VN_NAMES, current2))
            {
                p2_names(type);
            }
        }
        else
        {
            // 空产生式
        }
    }
}

// NAME → id
void p2_name(int type)
{
    // printf("into p2_name\n");
    if (in_first(VN_NAME, current2))
    {
        if (current2->token_id == VT_ID)
        {
            // 将id记录到表中，并分配temp
            if (type == VT_INT)
            {
                // printf("add sym : %s, int\n", current2->name);
                s = add_sym(s, current2->name, get_new_temp(), 4);
            }
            else if (type == VT_CHAR)
            {
                // printf("add sym : %s, char\n", current2->name);
                s = add_sym(s, current2->name, get_new_temp(), 1);
            }

            go_next2();
        }
        else
        {
            printf("error: NAME -> id\n");
            return;
        }
    }
}

// STMTS → STMT STMTS | empty
void p2_stmts()
{
    // printf("into p2_stmts\n");
    if (in_first(VN_STMTS, current2))
    {
        if (in_first(VN_STMT, current2))
        {
            p2_stmt();
            if (in_first(VN_STMTS, current2))
            {
                p2_stmts();
            }
        }
        else
        {
            // 空产生式
        }
    }
}

// STMT  → id = EXPR ;
//       | if ( REL ) STMT ELSESTMT
//       | while ( REL ) STMT
//       | BLOCK
//       | return EXPR ;

void p2_stmt()
{
    // printf("into p2_stmt\n");
    if (in_first(VN_STMT, current2))
    {
        // left = id ,find temp
        // right = EXPR return value;
        if (current2->token_id == VT_ID)
        {
            // 找到坐标标识符分配的temp id
            int left = find_temp_id(current2->name);
            go_next2();
            if (current2->token_id == VT_ASSIGN)
            {
                // printf("into =\n");
                go_next2();

                if (in_first(VN_EXPR, current2))
                {
                    attr* right = p2_expr();
                    if (current2->token_id == VT_SEMICOLON)
                    {
                        char left_str[10] = "";
                        char right_str[10] = "";

                        char left_str_tmp[10] = "";
                        char right_str_temp[10] = "";

                        strcat(right_str_temp, right->value);
                        sprintf(left_str_tmp, "%d", left);

                        //left
                        strcpy(left_str, "T");
                        strcat(left_str, left_str_tmp);


                        //right
                        if (right->is_temp == 1)
                        {
                            strcpy(right_str, "T");
                            strcat(right_str, right_str_temp);
                        }
                        else
                        {
                            strcat(right_str, right->value);
                        }

                        // 生成四元式(=,right,,left)
                        q = insert_quad(q, "mov", right_str, "", left_str);
                        line_count++;
                        go_next2();
                    }
                    else
                    {
                        printf("error: lack of ; 2\n");
                    }
                }
            }
            else
            {
                printf("error: lack of =\n");
            }
        }
        else if (current2->token_id == VT_IF)
        {
            go_next2();
            if (current2->token_id == VT_BRACKET_L)
            {
                go_next2();
                if (in_first(VN_REL, current2))
                {
                    quad* temp = NULL;
                    temp = p2_rel(temp);

                    // 跳转函数
                    // 因为是符合要求后跳转，所以要颠倒
                    char j_op[10] = "";

                    if(strcmp("a",temp->op) == 0)
                    {
                        strcat(j_op,"jnb");
                    }
                    else if(strcmp("na",temp->op) == 0)
                    {
                        strcat(j_op,"jb");
                    }
                    else if(strcmp("b",temp->op) == 0)
                    {
                        strcat(j_op,"jna");
                    }
                    else if(strcmp("nb",temp->op) == 0)
                    {
                        strcat(j_op,"ja");
                    }
                    else  if(strcmp("e",temp->op) == 0)
                    {
                        strcat(j_op,"jne");
                    }
                    else if(strcmp("ne",temp->op) == 0)
                    {
                        strcat(j_op,"je");
                    }

                    // 跳转目标
                    int label_id = get_new_label();

                    char lid_str[10] = "";
                    sprintf(lid_str, "%d", label_id);

                    char j_lid_str[10] = "";
                    sprintf(j_lid_str, "L%d", label_id);


                    if (current2->token_id == VT_BRACKET_R)
                    {
                        q = insert_quad(q, j_op, temp->arg1, temp->arg2, j_lid_str);
                        line_count++;

                        go_next2();
                        if (in_first(VN_STMT, current2))
                        {
                            p2_stmt();
                        }
                        if (in_first(VN_ELSEST, current2))
                        {
                            int lable_id_2 = get_new_label();
                            char lid2_str[10];
                            sprintf(lid2_str, "%d", lable_id_2);

                            char j_lid2_str[10];
                            sprintf(j_lid2_str, "L%d", lable_id_2);
                            //先让if的跳出else循环外, 无条件
                            q = insert_quad(q, "jmp", "", "", j_lid2_str);

                            line_count++;
                            //再将其他的引导入else循环内
                            set_label(label_id);

                            p2_elsestmt(label_id);
                            //if跳到else后
                            set_label(lable_id_2);
                        }
                    }
                    else
                    {
                        printf("error: lack of ) 1\n");
                    }
                }
            }
            else
            {
                printf("error: lack of (\n");
            }
        }
        else if (current2->token_id == VT_WHILE)
        {
            go_next2();
            if (current2->token_id == VT_BRACKET_L)
            {
                go_next2();
                if (in_first(VN_REL, current2))
                {
                    quad* temp = NULL;
                    temp = p2_rel(temp);

                    // 跳转函数
                    // 因为是符合要求后跳转，所以要颠倒
                    char j_op[10] = "";

                    if(strcmp("a",temp->op) == 0)
                    {
                        strcat(j_op,"jnb");
                    }
                    else if(strcmp("na",temp->op) == 0)
                    {
                        strcat(j_op,"jb");
                    }
                    else if(strcmp("b",temp->op) == 0)
                    {
                        strcat(j_op,"jna");
                    }
                    else if(strcmp("nb",temp->op) == 0)
                    {
                        strcat(j_op,"ja");
                    }
                    else  if(strcmp("e",temp->op) == 0)
                    {
                        strcat(j_op,"jne");
                    }
                    else if(strcmp("ne",temp->op) == 0)
                    {
                        strcat(j_op,"je");
                    }

                    if (current2->token_id == VT_BRACKET_R)
                    {
                        // 先给开头，结尾都做上一个标记
                        int begin_label = get_new_label();
                        int end_label = get_new_label();
                        char blstr[10] = "";
                        char elstr[10] = "";
                        sprintf(blstr, "%d", begin_label);
                        sprintf(elstr, "%d", end_label);

                        char j_blstr[10] = "";
                        char j_elstr[10] = "";
                        sprintf(j_blstr, "L%d", begin_label);
                        sprintf(j_elstr, "L%d", end_label);

                        set_label(begin_label);

                        // 判断条件结束
                        q = insert_quad(q, j_op, temp->arg1, temp->arg2, j_elstr);
                        line_count++;

                        go_next2();
                        if (in_first(VN_STMT, current2))
                        {
                            p2_stmt();
                        }

                        //无条件跳转
                        q = insert_quad(q, "jmp", "", "", j_blstr);
                        line_count++;
                        set_label(end_label);
                    }
                }
                else
                {
                    printf("error: lack of ) 2\n");
                }
            }
            else
            {
                printf("error: lack of (\n");
            }
        }
        else if (in_first(VN_BLOCK, current2))
        {
            p2_block();
        }
        else if (current2->token_id == VT_RETURN)
        {
            go_next2();

            if (in_first(VN_EXPR, current2))
            {
                attr* left = p2_expr();
                if (current2->token_id == VT_SEMICOLON)
                {
                    // 生成四元组('ret', left, '', '')
                    char vstr[10];
                    if (left->is_temp)
                        sprintf(vstr, "T%s", left->value);
                    else
                    {
                        sprintf(vstr, "%s", left->value);
                    }
                    q = insert_quad(q, "ret", "", "", vstr);
                    line_count++;
                    go_next2();
                }
                else
                {
                    printf("error: lack of ; 3\n");
                }
            }
        }
        else
        {
            printf(
                "error: STMT -> id = EXPR ; | if ( REL ) STMT ELSESTMT | while ( REL ) STMT | BLOCK | return EXPR ;\n");

        }
    }
}

// ELSESTMT → else STMT | empty
void p2_elsestmt()
{
    // printf("into p2_elsestmt\n");
    if (in_first(VN_ELSEST, current2))
    {
        if (current2->token_id == VT_ELSE)
        {

            go_next2();
            if (in_first(VN_STMT, current2))
            {
                p2_stmt();
            }
        }
        else
        {
            // 空产生式
        }
    }
}

// EXPR → TERM EXPR_
// send attr from TERM to EXPR_
// return value : return from EXPR_
attr* p2_expr()
{
    // printf("into p2_expr\n");
    if (in_first(VN_EXPR, current2))
    {
        if (in_first(VN_TERM, current2))
        {
            attr* left = p2_term();
            if (in_first(VN_EXPR_, current2))
            {
                return p2_expr_(left);
            }
        }
    }
    return NULL;
}

// EXPR_ → ADD EXPR | empty
// get left from brother TERM
// return value:{
//      1. attr from TERM if EXPR_ == empty
//      2. temp
//  }
attr* p2_expr_(attr* left)
{
    // printf("into p2_expr_\n");
    if (in_first(VN_EXPR_, current2))
    {
        if (in_first(VN_ADD, current2))
        {
            int op = p2_add();

            if (in_first(VN_EXPR, current2))
            {
                attr* right = p2_expr(left);

                char op_str[10] = "";
                char left_str[10] = "";
                char right_str[10] = "";
                char temp_str[10] = "";
                char tstr[10] = "";

                //OP
                if (op == VT_ADD)
                {
                    strcpy(op_str, "add");
                }
                else if (op == VT_MINUS)
                {
                    strcpy(op_str, "sub");
                }

                //left
                if (left->is_temp == 1)
                {
                    strcpy(left_str, "T");
                    strcat(left_str, left->value);
                }
                else
                {
                    strcat(left_str, left->value);
                }

                //right
                if (right->is_temp == 1)
                {
                    strcpy(right_str, "T");
                    strcat(right_str, right->value);
                }
                else
                {
                    strcat(right_str, right->value);
                }

                int temp_id = get_new_temp();
                sprintf(tstr, "%d", temp_id);
                strcpy(temp_str, "T");
                strcat(temp_str, tstr);


                // 生成四元组 (op, left, right, temp)
                q = insert_quad(q, op_str, left_str, right_str, temp_str);
                line_count++;

                return new_attr(1, tstr);
            }
        }
        else
        {
            // 空产生式
            return left;
        }
    }
    return NULL;
}

// ADD → + | -
// return value: VT_ADD | VT_MINUS
int p2_add()
{
    // printf("into p2_add\n");
    if (in_first(VN_ADD, current2))
    {
        // printf("into p2_add\n");
        // printf("now current2->token_id = %d\n",current2->token_id);
        // printf("%d %d",get_op2_id("+"),get_op2_id("-"));
        if (current2->token_id == VT_ADD)
        {
            go_next2();
            return VT_ADD;
        }
        if (current2->token_id == VT_MINUS)
        {
            go_next2();
            return VT_MINUS;
        }
    }
    printf("error: ADD -> + | -\n");
    return 0;
}

// TERM → FACTOR TERM_
// send attr from FACTOR to TERM_
// return value: return from TERM_
attr* p2_term()
{
    // printf("into p2_term\n");
    if (in_first(VN_TERM, current2))
    {
        if (in_first(VN_FACTOR, current2))
        {
            attr* left = p2_factor();
            if (in_first(VN_TERM_, current2))
            {
                return p2_term_(left);
            }
        }
    }
    return NULL;
}

// TERM_ → MUL TERM | empty
// get attr * left form brother FACTOR
// return value:{
//      1. attr from FACTOR if TERM == empty
//      2. temp
// }
attr* p2_term_(attr* left)
{
    // printf("into p2_term_\n");
    if (in_first(VN_TERM_, current2))
    {
        //printf("current2->token_id = %d is in first\n",current2->token_id);
        if (in_first(VN_MUL, current2))
        {
            int op = p2_mul();

            if (in_first(VN_TERM, current2))
            {
                attr* right = p2_term();

                char op_str[10] = "";
                char left_str[10] = "";
                char right_str[10] = "";
                char temp_str[10] = "";

                //OP
                if (op == VT_MULTIPLY)
                {
                    strcpy(op_str, "mul");
                }
                else if (op == VT_DIVIDE)
                {
                    strcpy(op_str, "div");
                }
                else
                {
                    strcpy(op_str, "");
                }

                //left
                if (left->is_temp == 1)
                {
                    strcpy(left_str, "T");
                    strcat(left_str, left->value);
                }
                else
                {
                    strcat(left_str, left->value);
                }

                //right
                if (right->is_temp == 1)
                {
                    strcpy(right_str, "T");
                    strcat(right_str, right->value);
                }
                else
                {
                    strcat(right_str, right->value);
                }

                int temp_id = get_new_temp(); //分配 temp
                char tstr[10];
                sprintf(tstr, "%d", temp_id);
                strcpy(temp_str, "T");
                strcat(temp_str, tstr);

                // 生成四元组(op, left, right, temp)
                q = insert_quad(q, op_str, left_str, right_str, temp_str);
                line_count++;

                return new_attr(1, tstr);
            }
        }
    }
    return left;
}

// MUL → * | /
// return value: VT_MULTIPLY | VT_DIVIDE
int p2_mul()
{
    // printf("into p2_mul\n");
    if (in_first(VN_MUL, current2))
    {
        if (current2->token_id == VT_MULTIPLY)
        {
            go_next2();
            return VT_MULTIPLY;
        }
        if (current2->token_id == VT_DIVIDE)
        {
            go_next2();
            return VT_DIVIDE;
        }
    }
    printf("error: MUL -> * | /\n");
    return 0;
}

// FACTOR → ( REL ) | id | number | chr
// return value: attr {
//     1. raw data
//     2. allocated temp from (REL)
//   }
attr* p2_factor()
{
    // printf("into p2_factor\n");
    if (in_first(VN_FACTOR, current2))
    {
        if (current2->token_id == VT_BRACKET_L)
        {
            go_next2();
            if (in_first(VN_REL, current2))
            {
                quad* temp = NULL;
                temp = p2_rel(temp);
                int temp_id = get_new_temp();

                char temp_str[10] = "T";
                char tstr[10] = "";
                sprintf(tstr, "%d", temp_id);
                strcat(temp_str, tstr);

                if (current2->token_id == VT_BRACKET_R)
                {
                    go_next2();

                    // 计算REL保存到temp
                    q = insert_quad(q, temp->op, temp->arg1, temp->arg2, temp_str);
                    line_count++;
                    return new_attr(1, tstr);
                }
                else
                {
                    printf("error: lack of ) 3\n");
                    return NULL;
                }
            }
        }
        else if (current2->token_id == VT_ID)
        {
            int temp_id;
            if ((temp_id = find_temp_id(current2->name)) != 0)
            {
                char tstr[10];
                sprintf(tstr, "%d", temp_id);
                go_next2();
                return new_attr(1, tstr);
            }
        }
        else if (current2->token_id == VT_NUMBER)
        {
            attr* new = new_attr(0, current2->name);
            go_next2();
            return new;
        }
        else if (current2->token_id == VT_CHAR_VALUE)
        {
            attr* new = new_attr(0, current2->name);
            go_next2();
            return new;
        }
        else
        {
            printf("error: FACTOR -> ( REL ) | id | number | chr\n");
        }
    }return 0;
}

// REL → EXPR ROP EXPR
quad* p2_rel(quad* temp)
{
    // printf("into p2_rel\n");
    if (in_first(VN_REL, current2))
    {
        if (in_first(VN_EXPR, current2))
        {
            attr* left = p2_expr();
            if (in_first(VN_ROP, current2))
            {
                int op = p2_rop();
                if (in_first(VN_EXPR, current2))
                {
                    attr* right = p2_expr();

                    char op_str[10] = "";
                    char left_str[10] = "";
                    char right_str[10] = "";
                    char temp_str[10] = "";

                    //OP
                    if (op == VT_LARGER)
                    {
                        strcpy(op_str, "a"); //above
                    }
                    else if (op == VT_LARGER_EQUAL)
                    {
                        strcpy(op_str, "nb");
                    } //not below
                    else if (op == VT_LESS)
                    {
                        strcpy(op_str, "b");
                    } //below
                    else if (op == VT_LESS_EQUAL)
                    {
                        strcpy(op_str, "na"); //not above
                    }
                    else if (op == VT_EQUAL)
                    {
                        strcpy(op_str, "e"); //equal
                    }
                    else if (op == VT_NOT_EQUAL)
                    {
                        strcpy(op_str, "ne"); //not equal
                    }
                    else
                    {
                        strcpy(op_str, "");
                    }

                    //left
                    if (left->is_temp == 1)
                    {
                        strcpy(left_str, "T");
                        strcat(left_str, left->value);
                    }
                    else
                    {
                        strcat(left_str, left->value);
                    }

                    //right
                    if (right->is_temp == 1)
                    {
                        strcpy(right_str, "T");
                        strcat(right_str, right->value);
                    }
                    else
                    {
                        strcat(right_str, right->value);
                    }

                    // 创建新节点
                    temp = (quad*)malloc(sizeof(quad));
                    quad* q = temp;

                    // 将分配好的字符串拷贝到新节点中
                    q->op = (char*)malloc(strlen(op_str) + 1);
                    q->arg1 = (char*)malloc(strlen(left_str) + 1);
                    q->arg2 = (char*)malloc(strlen(right_str) + 1);
                    q->rst = (char*)malloc(strlen(temp_str) + 1);

                    strcpy(q->op, op_str);
                    strcpy(q->arg1, left_str);
                    strcpy(q->arg2, right_str);
                    strcpy(q->rst, temp_str);
                    q->next = NULL;
                    //半成品四元组 (op, left, right, label ?)
                    //交给 IF 进行判断

                    return temp;
                }
            }
        }
    }
    return NULL;
}

// ROP → > | >= | < | <= | == | !=
int p2_rop()
{
    // printf("into p2_rop\n");
    if (in_first(VN_ROP, current2))
    {
        if (current2->token_id == VT_LARGER)
        {
            go_next2();
            return VT_LARGER;
        }
        if (current2->token_id == VT_LARGER_EQUAL)
        {
            go_next2();
            return VT_LARGER_EQUAL;
        }
        if (current2->token_id == VT_LESS)
        {
            go_next2();
            return VT_LESS;
        }
        if (current2->token_id == VT_LESS_EQUAL)
        {
            go_next2();
            return VT_LESS_EQUAL;
        }
        if (current2->token_id == VT_EQUAL)
        {
            go_next2();
            return VT_EQUAL;
        }
        if (current2->token_id == VT_NOT_EQUAL)
        {
            go_next2();
            return VT_NOT_EQUAL;
        }
    }
    printf("error: ROP -> > | >= | < | <= | == | !=\n");
    return 0;
}


int get_new_temp()
{
    return temp_count++;
}

int get_new_label()
{
    return label_count++;
}

void set_label(int label_id)
{
    label_location[label_id] = line_count;
}

//typedef struct symbol_table{
//    char * name;
//    int temp_id;
//    struct symbol_table * next;
//}symtab;
int find_temp_id(char* name)
{
    symtab* s = shead->next;
    //search for symbol_table
    while (s != NULL)
    {
        if (strcmp(name, s->name) == 0)
            return s->temp_id;
        s = s->next;
    }
    printf("error : undefined id '%s' exists.\ncompile failed", name);
    exit(1);
}



void fill_label(quad * q)
{
    quad * p = q->next; //skip head
    int line = 0;

    while(p != NULL)
    {
        if(p->rst[0] == 'L')    //如果是L开头，进行替换
        {
            //get Substring
            char num_str[10];
            int i=1, j=0;
            while(1)
            {
                num_str[j] = p->rst[i];
                i++,j++;
                if(p->rst[i] != '\0')
                {
                    break;
                }
            }
            int label_id = atoi(num_str);
            if(label_id < 0 || label_id >= label_count)
            {
                printf("error: label not found\n");
            }

            int pos = label_location[label_id];

            //进行位置计算
            sprintf(p->rst,"%d",(pos - line - 1));
        }
        p = p->next;
        line++;
    }
}