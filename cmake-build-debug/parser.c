#include "includes/parser.h"
#include "includes/grammar_utils.h"
#include "includes/lexer_utils.h"
#include "includes/tree.h"
#include <stdio.h>
#include <stdbool.h>

// first集合表，初始化为false
bool firstset[NUM_NON_TERMINALS][NUM_TERMINALS] = {0};

node *current = NULL;

void go_next()  // 读取下一个节点
{
    current = current->next;
}


// 递归下降分析
tnode *ed_parsing(node *head)
{
    init_first_set();         // 初始化first集合
    //print_first_set();        // 打印first集合
    current = head;           // 当前节点指向头节点
    go_next();                // 读取第一个节点
    return p_prog();          // 进入递归下降分析 
}

// 起始节点 PROG → BLOCK
tnode *p_prog()
{
    tnode *root = NULL;
    if (in_first(VN_PROG, current))
    {
        root = new_tree(VN_PROG);          // 创建根节点
        tnode *block = add_child(root, VN_BLOCK); // 添加子节点
        p_block(block);
    } else
    {
        printf("error: PROG -> BLOCK\n");
        return NULL;
    }
    return root;
}

// BLOCK → { DECLS STMTS }
void p_block(tnode *block)
{
    if (in_first(VN_BLOCK, current))
    {
        // first集中只有一项，所以这个判断可以取消
        // if (current->token_id == get_de_id('{'))
        add_child(block, VT_START + VT_CURLY_L);
        go_next();
        if (in_first(VN_DECLS, current))
        {
            tnode *decls = add_child(block, VN_DECLS);
            p_decls(decls);
            if (in_first(VN_STMTS, current))
            {
                tnode *stmts = add_child(block, VN_STMTS);
                p_stmts(stmts);
                if (current->token_id == VT_CURLY_R)
                {
                    add_child(block, VT_START + VT_CURLY_R);
                    go_next();
                } else
                {
                    printf("error: lack of }\n");
                    return;
                }
            }
        }
    } else
    {
        printf("error: can't find {\n");
        return;
    }
}


// DECLS → DECL DECLS | empty
void p_decls(tnode *decls)
{
    
    if (in_first(VN_DECLS, current))
    {
        if (current->token_id == VT_INT || current->token_id == VT_CHAR)
        {
            tnode *decl = add_child(decls, VN_DECL);
            p_decl(decl);
            if (in_first(VN_DECLS, current))
            {
                tnode *decls_ = add_child(decls, VN_DECLS);
                p_decls(decls_);
            }
        } else
        {
            // 空产生式
        }
    }
}

// DECL → TYPE NAMES ;
void p_decl(tnode *decl)
{
    
    if (in_first(VN_DECL, current))
    {
        if (in_first(VN_TYPE, current))
        {
            tnode *type = add_child(decl, VN_TYPE);
            p_type(type);

            if (in_first(VN_NAMES, current))
            {
                tnode *names = add_child(decl, VN_NAMES);
                p_names(names);
                if (current->token_id == VT_SEMICOLON)
                {
                    add_child(decl, VT_START + VT_SEMICOLON);
                    go_next();
                } else
                {
                    printf("error: lack of ; 1\n");
                    return;
                }
            }
        }
    }
}

// TYPE → int | char
void p_type(tnode *type)
{
    
    if (in_first(VN_TYPE, current))
    {
        if (current->token_id == VT_INT)
        {
            add_child(type, VT_START + VT_INT);
            go_next();
        } else if (current->token_id == VT_CHAR)
        {
            add_child(type, VT_START + VT_CHAR);
            go_next();
        }
    } else
    {
        printf("error: TYPE -> int | char\n");
        return;
    }

}

// NAMES → NAME NAMES_
void p_names(tnode *names)
{
    
    if (in_first(VN_NAMES, current))
    {
        if (in_first(VN_NAME, current))
        {
            tnode *name = add_child(names, VN_NAME);

            p_name(name);
            if (in_first(VN_NAMES_, current))
            {
                tnode *names_ = add_child(names, VN_NAMES_);

                p_names_(names_);
            }
        }
    }
}

// NAMES_ → , NAMES | empty
void p_names_(tnode *names_)
{
    
    if (in_first(VN_NAMES_, current))
    {
        if (current->token_id == VT_COMMA)
        {
            add_child(names_, VT_START + VT_COMMA);
            go_next();
            if (in_first(VN_NAMES, current))
            {
                tnode *names = add_child(names_, VN_NAMES);

                p_names(names);
            }
        } else
        {
            // 空产生式
        }
    }
}

// NAME → id
void p_name(tnode *name)
{
    
    if (in_first(VN_NAME, current))
    {
        if (current->token_id == VT_ID)
        {
            add_child(name, VT_START + VT_ID);
            go_next();
        } else
        {
            printf("error: NAME -> id\n");
            return;
        }
    }
}

// STMTS → STMT STMTS | empty
void p_stmts(tnode *stmts)
{
    
    if (in_first(VN_STMTS, current))
    {
        if (in_first(VN_STMT, current))
        {
            tnode *stmt = add_child(stmts, VN_STMT);

            p_stmt(stmt);
            if (in_first(VN_STMTS, current))
            {
                tnode *stmts_ = add_child(stmts, VN_STMTS);

                p_stmts(stmts_);
            }
        } else
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

void p_stmt(tnode *stmt)
{
    
    if (in_first(VN_STMT, current))
    {
        if (current->token_id == VT_ID)
        {
            add_child(stmt, VT_START + VT_ID);
            go_next();
            if (current->token_id == VT_ASSIGN)
            {
                // printf("into =\n");
                add_child(stmt, VT_START + VT_ASSIGN);
                go_next();
                if (in_first(VN_EXPR, current))
                {
                    tnode *expr = add_child(stmt, VN_EXPR);
                    p_expr(expr);
                }

//                printf("now current->token_id = %d\n", current->token_id);
                if (current->token_id == VT_SEMICOLON)
                {
                    add_child(stmt, VT_START + VT_SEMICOLON);
                    go_next();
                } else
                {
                    printf("error: lack of ; 2\n");
                    return;
                }
            } else
            {
                printf("error: lack of =\n");
                return;
            }
        } else if (current->token_id == VT_IF)
        {
            add_child(stmt, VT_START + VT_IF);
            go_next();
            if (current->token_id == VT_BRACKET_L)
            {
                add_child(stmt, VT_START + VT_BRACKET_L);
                go_next();
                if (in_first(VN_REL, current))
                {
                    tnode *rel = add_child(stmt, VN_REL);
                    p_rel(rel);
                }
                if (current->token_id == VT_BRACKET_R)
                {
                    add_child(stmt, VT_START + VT_BRACKET_R);
                    go_next();
                    if (in_first(VN_STMT, current))
                    {
                        tnode *stmt_ = add_child(stmt, VN_STMT);

                        p_stmt(stmt_);
                    }
                    if (in_first(VN_ELSEST, current))
                    {
                        tnode *elsestmt = add_child(stmt, VN_ELSEST);

                        p_elsestmt(elsestmt);
                    }
                } else
                {
                    printf("error: lack of ) 1\n");
                    return;
                }
            } else
            {
                printf("error: lack of (\n");
                return;
            }
        } else if (current->token_id == VT_WHILE)
        {
            add_child(stmt, VT_START + VT_WHILE);
            go_next();
            if (current->token_id == VT_BRACKET_L)
            {
                add_child(stmt, VT_START + VT_BRACKET_L);
                go_next();
                if (in_first(VN_REL, current))
                {
                    tnode *rel = add_child(stmt, VN_REL);
                    p_rel(rel);
                }
                if (current->token_id == VT_BRACKET_R)
                {
                    add_child(stmt, VT_START + VT_BRACKET_R);
                    go_next();
                    if (in_first(VN_STMT, current))
                    {
                        tnode *stmt_ = add_child(stmt, VN_STMT);

                        p_stmt(stmt_);
                    }
                } else
                {
                    printf("error: lack of ) 2\n");
                    return;
                }
            } else
            {
                printf("error: lack of (\n");
                return;
            }
        } else if (in_first(VN_BLOCK, current))
        {
            tnode *block = add_child(stmt, VN_BLOCK);

            p_block(block);
        } else if (current->token_id == VT_RETURN)
        {
            add_child(stmt, VT_START + VT_RETURN);
            go_next();
            if (in_first(VN_EXPR, current))
            {
                tnode *expr = add_child(stmt, VN_EXPR);

                p_expr(expr);
            }
            if (current->token_id == VT_SEMICOLON)
            {
                add_child(stmt, VT_START + VT_SEMICOLON);
                go_next();
            } else
            {
                printf("error: lack of ; 3\n");
                return;
            }
        } else
        {
            printf("error: STMT -> id = EXPR ; | if ( REL ) STMT ELSESTMT | while ( REL ) STMT | BLOCK | return EXPR ;\n");
            return;
        }
    }
}

// ELSESTMT → else STMT | empty
void p_elsestmt(tnode *elsestmt)
{
    
    if (in_first(VN_ELSEST, current))
    {
        if (current->token_id == VT_ELSE)
        {
            add_child(elsestmt, VT_START + VT_ELSE);
            go_next();
            if (in_first(VN_STMT, current))
            {
                tnode *stmt = add_child(elsestmt, VN_STMT);

                p_stmt(stmt);
            }
        } else
        {
            // 空产生式
        }
    }
}

// EXPR → TERM EXPR_
void p_expr(tnode *expr)
{
    
    if (in_first(VN_EXPR, current))
    {
        if (in_first(VN_TERM, current))
        {
            tnode *term = add_child(expr, VN_TERM);

            p_term(term);
            if (in_first(VN_EXPR_, current))
            {
                tnode *expr_ = add_child(expr, VN_EXPR_);

                p_expr_(expr_);
            }
        }
    }
}

// EXPR_ → ADD EXPR | empty
void p_expr_(tnode *expr_)
{
    
    if (in_first(VN_EXPR_, current))
    {
        if (in_first(VN_ADD, current))
        {
            // printf("into ADD\n");
            // printf("now current->token_id = %d\n",current->token_id);
            tnode *add = add_child(expr_, VN_ADD);
            p_add(add);

            if (in_first(VN_EXPR, current))
            {
                tnode *expr = add_child(expr_, VN_EXPR);

                p_expr(expr);
            }
        } else
        {
            // 空产生式
        }
    }
}

// ADD → + | -
void p_add(tnode *add)
{
    
    if (in_first(VN_ADD, current))
    {
                // printf("now current->token_id = %d\n",current->token_id);
        // printf("%d %d",get_op_id("+"),get_op_id("-"));
        if (current->token_id == VT_ADD)
        {
            add_child(add, VT_START + VT_ADD);
            go_next();
        } else if (current->token_id == VT_MINUS)
        {
            add_child(add, VT_START + VT_MINUS);
            go_next();
        } else
        {
            printf("error: ADD -> + | -\n");
            return;
        }
    }
}

// TERM → FACTOR TERM_
void p_term(tnode *term)
{
    
    if (in_first(VN_TERM, current))
    {
        if (in_first(VN_FACTOR, current))
        {
            tnode *factor = add_child(term, VN_FACTOR);

            p_factor(factor);
            if (in_first(VN_TERM_, current))
            {
//               printf("current->token_id = %d is in first\n", current->token_id);
                tnode *term_ = add_child(term, VN_TERM_);
                p_term_(term_);
            }
        }
    }
}

// TERM_ → MUL TERM | empty
void p_term_(tnode *term_)
{
    
    if (in_first(VN_TERM_, current))
    {
//        printf("current->token_id = %d is in first\n", current->token_id);
        if (in_first(VN_MUL, current))
        {
            tnode *mul = add_child(term_, VN_MUL);

            p_mul(mul);

            if (in_first(VN_TERM, current))
            {
                tnode *term = add_child(term_, VN_TERM);

                p_term(term);
            }
        } else
        {
            // 空产生式
        }
    }
}

// MUL → * | /
void p_mul(tnode *mul)
{
    
    if (in_first(VN_MUL, current))
    {
        if (current->token_id == VT_MULTIPLY)
        {
            add_child(mul, VT_START + VT_MULTIPLY);
            go_next();
        } else if (current->token_id == VT_DIVIDE)
        {
            add_child(mul, VT_START + VT_DIVIDE);
            go_next();
        } else
        {
            printf("error: MUL -> * | /\n");
            return;
        }
    }
}

// FACTOR → ( REL ) | id | number | chr
void p_factor(tnode *factor)
{
    
    if (in_first(VN_FACTOR, current))
    {
        if (current->token_id == VT_BRACKET_L)
        {
            add_child(factor, VT_START + VT_BRACKET_L);
            go_next();
            if (in_first(VN_REL, current))
            {
                tnode *rel = add_child(factor, VN_REL);

                p_rel(rel);
            }
            if (current->token_id == VT_BRACKET_R)
            {
                add_child(factor, VT_START + VT_BRACKET_R);
                go_next();
            } else
            {
                printf("error: lack of ) 3\n");
                return;
            }
        } else if (current->token_id == VT_ID)
        {
            add_child(factor, VT_START + VT_ID);
            go_next();
        } else if (current->token_id == VT_NUMBER)
        {
            add_child(factor, VT_START + VT_NUMBER);
            go_next();
        } else if (current->token_id == VT_CHAR_VALUE)
        {
            add_child(factor, VT_START + VT_CHAR_VALUE);
            go_next();
        } else
        {
            printf("error: FACTOR -> ( REL ) | id | number | chr\n");
            return;
        }
    }
}

// REL → EXPR ROP EXPR
void p_rel(tnode *rel)
{
    
    if (in_first(VN_REL, current))
    {
        if (in_first(VN_EXPR, current))
        {
            tnode *expr1 = add_child(rel, VN_EXPR);

            p_expr(expr1);
            if (in_first(VN_ROP, current))
            {
                tnode *rop = add_child(rel, VN_ROP);

                p_rop(rop);
            }
            if (in_first(VN_EXPR, current))
            {
                tnode *expr2 = add_child(rel, VN_EXPR);

                p_expr(expr2);
            }
        }
    }
}

// ROP → > | >= | < | <= | == | !=
void p_rop(tnode *rop)
{
    
    if (in_first(VN_ROP, current))
    {
        if (current->token_id == VT_LARGER)
        {
            add_child(rop, VT_START + VT_LARGER);
            go_next();
        } else if (current->token_id == VT_LARGER_EQUAL)
        {
            add_child(rop, VT_START + VT_LARGER_EQUAL);
            go_next();
        } else if (current->token_id == VT_LESS)
        {
            add_child(rop, VT_START + VT_LESS);
            go_next();
        } else if (current->token_id == VT_LESS_EQUAL)
        {
            add_child(rop, VT_START + VT_LESS_EQUAL);
            go_next();
        } else if (current->token_id == VT_EQUAL)
        {
            add_child(rop, VT_START + VT_EQUAL);
            go_next();
        } else if (current->token_id == VT_NOT_EQUAL)
        {
            add_child(rop, VT_START + VT_NOT_EQUAL);
            go_next();
        } else
        {
            printf("error: ROP -> > | >= | < | <= | == | !=\n");
            return;
        }
    }

}


// 初始化文法
// 初始化文法
void init_first_set()
{
    // 如果终结符包含在first中，则bool值为true，否则为false

    // 0. PROG        {
    firstset[VN_PROG][VT_CURLY_L] = true;

    // 1. BLOCK       {
    firstset[VN_BLOCK][VT_CURLY_L] = true;

    // 2. DECLS       int char empty
    firstset[VN_DECLS][VT_INT] = true;
    firstset[VN_DECLS][VT_CHAR] = true;
    firstset[VN_DECLS][VT_EMPTY] = true;

    //EMPTY FOLLOWERS + first(STMTS) + empty followers
    firstset[VN_DECLS][VT_ID] = true;
    firstset[VN_DECLS][VT_IF] = true;
    firstset[VN_DECLS][VT_WHILE] = true;
    firstset[VN_DECLS][VT_CURLY_L] = true;
    firstset[VN_DECLS][VT_RETURN] = true;
    firstset[VN_DECLS][VT_EMPTY] = true;
    firstset[VN_DECLS][VT_CURLY_R] = true;

    // 3. DECL        int char
    firstset[VN_DECL][VT_INT] = true;
    firstset[VN_DECL][VT_CHAR] = true;

    // 4. TYPE        int char
    firstset[VN_TYPE][VT_INT] = true;
    firstset[VN_TYPE][VT_CHAR] = true;

    // 5. NAMES       id
    firstset[VN_NAMES][VT_ID] = true;

    // 6. NAMES_      , empty
    firstset[VN_NAMES_][VT_COMMA] = true;
    firstset[VN_NAMES_][VT_EMPTY] = true;

    //EMPTY
    firstset[VN_NAMES_][VT_SEMICOLON] = true;

    // 7. NAME        id
    firstset[VN_NAME][VT_ID] = true;

    // 8. STMTS       id if while { return empty
    firstset[VN_STMTS][VT_ID] = true;
    firstset[VN_STMTS][VT_IF] = true;
    firstset[VN_STMTS][VT_WHILE] = true;
    firstset[VN_STMTS][VT_CURLY_L] = true;
    firstset[VN_STMTS][VT_RETURN] = true;
    firstset[VN_STMTS][VT_EMPTY] = true;

    // EMPTY FOLLOWERS
    firstset[VN_STMTS][VT_CURLY_R] = true;

    // 9. STMT        id if while { return
    firstset[VN_STMT][VT_ID] = true;
    firstset[VN_STMT][VT_IF] = true;
    firstset[VN_STMT][VT_WHILE] = true;
    firstset[VN_STMT][VT_CURLY_L] = true;
    firstset[VN_STMT][VT_RETURN] = true;

    // 10.ELSESTMT    else empty
    firstset[VN_ELSEST][VT_ELSE] = true;
    firstset[VN_ELSEST][VT_EMPTY] = true;

    // EMPTY FOLLOWERS + first(STMTS) + follow(STMTS)
    firstset[VN_ELSEST][VT_ID] = true;
    firstset[VN_ELSEST][VT_IF] = true;
    firstset[VN_ELSEST][VT_WHILE] = true;
    firstset[VN_ELSEST][VT_CURLY_L] = true;
    firstset[VN_ELSEST][VT_CURLY_R] = true;
    firstset[VN_ELSEST][VT_RETURN] = true;

    // 11.EXPR        ( id number chr
    firstset[VN_EXPR][VT_BRACKET_L] = true;
    firstset[VN_EXPR][VT_ID] = true;
    firstset[VN_EXPR][VT_NUMBER] = true;
    firstset[VN_EXPR][VT_CHAR_VALUE] = true;


    // 12.EXPR_       + - empty
    firstset[VN_EXPR_][VT_ADD] = true;
    firstset[VN_EXPR_][VT_MINUS] = true;
    firstset[VN_EXPR_][VT_EMPTY] = true;

    //EMPTY FOLLOWERS + first(ROP) + ) + ;
    firstset[VN_EXPR_][VT_SEMICOLON] = true;
    firstset[VN_EXPR_][VT_BRACKET_R] = true;

    firstset[VN_EXPR_][VT_LARGER] = true;
    firstset[VN_EXPR_][VT_LARGER_EQUAL] = true;
    firstset[VN_EXPR_][VT_LESS] = true;
    firstset[VN_EXPR_][VT_LESS_EQUAL] = true;
    firstset[VN_EXPR_][VT_EQUAL] = true;
    firstset[VN_EXPR_][VT_NOT_EQUAL] = true;

    // 13.ADD         + -
    firstset[VN_ADD][VT_ADD] = true;
    firstset[VN_ADD][VT_MINUS] = true;

    // 14.TERM        ( id number chr
    firstset[VN_TERM][VT_BRACKET_L] = true;
    firstset[VN_TERM][VT_ID] = true;
    firstset[VN_TERM][VT_NUMBER] = true;
    firstset[VN_TERM][VT_CHAR_VALUE] = true;

    // 15.TERM_       * / empty
    firstset[VN_TERM_][VT_MULTIPLY] = true;
    firstset[VN_TERM_][VT_DIVIDE] = true;
    firstset[VN_TERM_][VT_EMPTY] = true;
    firstset[VN_TERM_][VT_ADD] = true;
    firstset[VN_TERM_][VT_MINUS] = true;

    //EMPTY FOLLOWERS + first(EXPR_) + follow(EXPR_)
    firstset[VN_TERM_][VT_ADD] = true;
    firstset[VN_TERM_][VT_MINUS] = true;
    firstset[VN_TERM_][VT_EMPTY] = true;

    //+ first(ROP) + ) + ;
    firstset[VN_TERM_][VT_SEMICOLON] = true;
    firstset[VN_TERM_][VT_BRACKET_R] = true;

    firstset[VN_TERM_][VT_LARGER] = true;
    firstset[VN_TERM_][VT_LARGER_EQUAL] = true;
    firstset[VN_TERM_][VT_LESS] = true;
    firstset[VN_TERM_][VT_LESS_EQUAL] = true;
    firstset[VN_TERM_][VT_EQUAL] = true;
    firstset[VN_TERM_][VT_NOT_EQUAL] = true;

    // 16.MUL         * /
    firstset[VN_MUL][VT_MULTIPLY] = true;
    firstset[VN_MUL][VT_DIVIDE] = true;

    // 17.FACTOR      ( id number chr
    firstset[VN_FACTOR][VT_BRACKET_L] = true;
    firstset[VN_FACTOR][VT_ID] = true;
    firstset[VN_FACTOR][VT_NUMBER] = true;
    firstset[VN_FACTOR][VT_CHAR_VALUE] = true;

    // 18.REL         ( id number chr
    firstset[VN_REL][VT_BRACKET_L] = true;
    firstset[VN_REL][VT_ID] = true;
    firstset[VN_REL][VT_NUMBER] = true;
    firstset[VN_REL][VT_CHAR_VALUE] = true;

    // 19.ROP         > >= < <= == !=
    firstset[VN_ROP][VT_LARGER] = true;
    firstset[VN_ROP][VT_LARGER_EQUAL] = true;
    firstset[VN_ROP][VT_LESS] = true;
    firstset[VN_ROP][VT_LESS_EQUAL] = true;
    firstset[VN_ROP][VT_EQUAL] = true;
    firstset[VN_ROP][VT_NOT_EQUAL] = true;
}

char *get_v_name(int v)
{
    char *v_names[] = {
            "PROG",
            "BLOCK",
            "DECLS",
            "DECL",
            "TYPE",
            "NAMES",
            "NAMES_",
            "NAME",
            "STMTS",
            "STMT",
            "ELSEST",
            "EXPR",
            "EXPR_",
            "ADD",
            "TERM",
            "TERM_",
            "MUL",
            "FACTOR",
            "REL",
            "ROP",
            "empty",
            "+",
            "-",
            "*",
            "/",
            "=",
            ">",
            ">=",
            "<",
            "<=",
            "==",
            "!=",
            "(",
            ")",
            "{",
            "}",
            ";",
            ",",
            "int",
            "char",
            "if",
            "else",
            "while",
            "return",
            "number",
            "chr",
            "id"};
    return v_names[v];
}

void print_first_set()
{
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        printf("%s:\t", get_v_name(i));
        for (int j = 0; j < NUM_TERMINALS; j++)
        {
            if (firstset[i][j])
            {
                printf("%s, ", get_v_name(j + VT_START));
            }
        }
        printf("\n");
    }
}

int in_first(int vn, node *current)
{
    int token_id = current->token_id;
    int isin = firstset[vn][token_id];
    return isin;
}

int fprint_grammar_tree(tnode *root, FILE *fp)
{
    return fprint_grammar_tree__(root, fp, 0);
}

int fprint_grammar_tree__(tnode *root, FILE *fp, int depth)
{
    tnode *temp = root;
    while (temp != NULL)
    {
        if (1)
        {
            for (int i = 0; i < depth; i++)
                fprintf(fp, "|  "); // 打印空格，用来表示树的深度
            fprintf(fp, "|--"); // 打印树的连接线
        }
        char *vname = get_v_name(temp->id);
        if (temp->id > VT_START)
        {
            fprintf(fp, "\"%s\"\n", vname); // 打印节点的id
        } else
        {
            fprintf(fp, "%s\n", vname); // 打印节点的id
        }

        fprint_grammar_tree__(temp->left, fp, depth + 1); // 打印左子树,深度加1
        // if (temp->right == NULL)
        // {
        //     for(int i = 0; i < depth; i++)
        //         fprintf(fp,"|  "); // 打印空格，用来表示树的深度
        //     fprintf(fp,"\n"); // 空出一行
        //     return 0;
        // }
        temp = temp->right; // 移动到右子树
    }
    return 0;
}

int print_grammar_tree(tnode *root)
{
    return fprint_grammar_tree(root, stdout);
}
