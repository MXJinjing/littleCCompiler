# Little C Compiler 编译器

文件结构：
```
├── cmake-build-debug
│         ├── build.ninja、
│         ├── CMakeCache.txt
│         ├── CMakeFiles
│         ├── cmake_install.cmake
│         ├── examples
│         │         ├── example1.txt
│         │         ├── example2.txt
│         │         ├── example3.txt
│         │         ├── example4.txt
│         │         └── example5.txt
│         ├── graph_utils.c
│         ├── includes  /* 头文件目录 */
│         │         ├── grammar_utils.h
│         │         ├── graph.h
│         │         ├── graph_utils.h
│         │         ├── heads.h
│         │         ├── lexer.h
│         │         ├── lexer_utils.h
│         │         ├── node.h
│         │         ├── parser.h
│         │         ├── prepare.h
│         │         ├── quad_utils.h
│         │         ├── translator.h
│         │         ├── tree.h
│         │         └── tree_utils.h
│         ├── lexer.c       /* 词法分析器 */
│         ├── main.c        /* 主函数 */
│         ├── parser.c      /* 构建语法树 */
│         ├── prepare.c     /* 去除注释 */
│         ├── quad_utils.c  /* 四元组工具 */
│         ├── README.md
│         ├── translator.c  /* 翻译中间代码 */
│         ├── tree.c        /* 树、二叉树 */
│         └── Testing
│                  └── Temporary
│                      └── LastTest.log
└── CMakeLists.txt  /* CMake 配置文件 */
```


### 文法规则
PROG → BLOCK

BLOCK → { DECLS STMTS }

DECLS → DECL DECLS | empty

DECL → TYPE NAMES ;

TYPE → int | char

NAMES → NAME NAMES_

NAMES_ → , NAME | empty

NAME → id

STMTS → STMT STMTS | empty

STMT → id = EXPR ;

STMT → if ( REL ) STMT ELSESTMT

STMT → while ( REL ) STMT

STMT → BLOCK

STMT → return EXPR ;

ELSESTMT → else STMT | empty

EXPR → TERM EXPR_

EXPR_ → ADD EXPR | empty

ADD → + | -

TERM → FACTOR TERM_

TERM_ →  MUL TERM | empty

MUL → * | /

FACTOR → ( REL ) | id | number | chr

REL → EXPR ROP EXPR

ROP → > | >= | < | <= | == | !=



### 非终结符V_N  FIRST集合
0. PROG        {
1. BLOCK       {
2. DECLS       int char empty
3. DECL        int char
4. TYPE        int char
5. NAMES       id
6. NAMES_      , empty
7. NAME        id
8. STMTS       id if while { return empty
9. STMT        id if while { return
10. ELSESTMT    else empty
11. EXPR        ( id number chr
12. EXPR_       + - empty
13. ADD         + -
14. TERM        ( id number chr
15. TERM_       * / empty
16. MUL         * /
17. FACTOR      ( id number chr
18. REL         ( id number chr
19. ROP         > >= < <= == !=

### 终结符V_T

operators:
- "+"   1
- "-"   2
- "*"   3
- "/"   4
- "="   5
- ">"   6
- ">="  7
- "<"   8
- "<="  9
- "=="  10
- "!="  11

delimiters:
- '('  12
- ')'  13
- '{'  14
- '}'  15
- ';'  16
- ','  17

keywords：
- int    18
- char   19
- if     20
- else   21
- while  22
- return 23

identifier:
- number 24
- chr 25
- id 26

