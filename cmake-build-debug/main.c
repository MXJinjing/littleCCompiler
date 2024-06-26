#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/heads.h"

int main(int argc, char *argv[])
{
    // 显示用法
    if (argc != 3)
    {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        exit(1);
    }
    if (argv[1] == argv[2])
    {
        printf("Error: The input file and output file should not be the same\n");
        exit(1);
    }
    char *input_string = argv[1];
    char *output_string = argv[2];

    FILE *input_file = fopen(input_string, "r");
    if (input_file == NULL)
    {
        printf("Can't open file %s\n", input_string);
        exit(1);
    }
    FILE *output_file = fopen(output_string, "w");
    char *temp_file_name = strcat(output_string, ".temp");  // 临时文件
    FILE *temp_file = fopen(temp_file_name, "w+b");

    // FILE *temp_file;
    // temp_file = tmpfile();
    if(temp_file == NULL) {
        printf("error: tempfile creation failed");
        exit(1);
    }

    // 去掉注释
    printf("now progressing comment remove\n");
    comment_remove(input_file, temp_file);
    fclose(input_file);
    rewind(temp_file);

    // 获得单词符号表
    printf("now progressing lexer analyze\n");
    node * head = lexer(temp_file);
    fprint_lexer_table(head,temp_file);// 打印单词符号表

    // 进行语法分析
    printf("now progressing syntax analyze\n");
    tnode * root = ed_parsing(head);

    // 打印语法树
    printf("now progressing print tree\n");
    // print_grammar_tree(root);
    fprint_grammar_tree(root, temp_file);

    //生成中间代码
    printf("now generating inter code\n");
    ofm * o = NULL;
    o = generate_intercode(head,o);
    fprint_symtab(o->shead,temp_file);
    fprint_quad(o->qhead,temp_file);
    fprint_quad_clean(o->qhead,output_file);


    // 使用完毕后删除临时文件
    fclose(output_file);
    fclose(temp_file);

    return 0;
}

