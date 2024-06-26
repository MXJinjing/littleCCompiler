#ifndef GRAMMA_UTILS_H
#define GRAMMA_UTILS_H

#define NUM_NON_TERMINALS 20
#define NUM_TERMINALS 27


// 非终结符的定义
#define VN_PROG   0
#define VN_BLOCK  1
#define VN_DECLS  2
#define VN_DECL   3
#define VN_TYPE   4
#define VN_NAMES  5
#define VN_NAMES_ 6
#define VN_NAME   7
#define VN_STMTS  8
#define VN_STMT   9
#define VN_ELSEST 10
#define VN_EXPR   11
#define VN_EXPR_  12
#define VN_ADD    13
#define VN_TERM   14
#define VN_TERM_  15
#define VN_MUL    16
#define VN_FACTOR 17
#define VN_REL    18
#define VN_ROP    19


//终结符从20开始计数
#define VT_START  20
#define VT_EMPTY  0

// 终结符的定义
#define VT_ADD    1
#define VT_MINUS  2
#define VT_MULTIPLY 3
#define VT_DIVIDE 4
#define VT_ASSIGN 5
#define VT_LARGER 6
#define VT_LARGER_EQUAL 7
#define VT_LESS 8
#define VT_LESS_EQUAL 9
#define VT_EQUAL 10
#define VT_NOT_EQUAL 11

#define VT_BRACKET_L 12
#define VT_BRACKET_R 13
#define VT_CURLY_L   14
#define VT_CURLY_R   15
#define VT_SEMICOLON 16
#define VT_COMMA     17

#define VT_INT          18
#define VT_CHAR         19
#define VT_IF           20
#define VT_ELSE         21
#define VT_WHILE        22
#define VT_RETURN       23

#define VT_NUMBER       24
#define VT_CHAR_VALUE   25
#define VT_ID           26

#endif //GRAMMA_UTILS_H
