#include "includes/prepare.h"
#include <stdio.h>
#include <stdlib.h>


void comment_remove(FILE *input_file, FILE *output_file)
{
    char next_c;
    char c;      // 记录字符
    char last_c; // 记录上一个字符

    last_c = '\0';
    c = fgetc(input_file); // 读取第一个字符

    // 定义状态
    enum CLOCATION
    {
        OUTSIDE,
        INSIDE_SINGLE_QUOTE, // 在单引号中
        INSIDE_MULTI_QUOTE   // 在双引号中
    } c_location;

    c_location = OUTSIDE;

    // 读取文件
    for (next_c = fgetc(input_file); next_c != EOF; last_c = c, c = next_c, next_c = fgetc(input_file))
    {
        //printf("%c,%d\n",next_c,c_location);
        if (c_location == INSIDE_MULTI_QUOTE)
        {
            if (c == '\"' && last_c != '\\')
            {
                c_location = OUTSIDE;
            }
        }
        else if (c_location == INSIDE_SINGLE_QUOTE)
        {
            if (c == '\'' && last_c != '\\')
            {
                c_location = OUTSIDE;
            }
        }
        else if (c == '\"'  && last_c != '\\')
        {
            //printf("now c = %c, last c = %c, next c = %c\n",c,last_c,next_c);
            c_location = INSIDE_MULTI_QUOTE;
        }
        else if (c == '\''  && last_c != '\\')
        {
            c_location = INSIDE_SINGLE_QUOTE;
        }
        else if (c == '/')
        {
            if (next_c == '*' && last_c != '*')
            { // 找到开始标记/*
                //printf("find /* starter\n");
                for (next_c = fgetc(input_file); next_c != EOF; next_c = fgetc(input_file))
                {
                    if (next_c == '/' && c == '*')
                    { // 找到结束标记*/
                        last_c = c, c = next_c;
                        next_c = fgetc(input_file); // 读取下一个字符

                        last_c = c, c = next_c;
                        next_c = fgetc(input_file); // 读取下一个字符
                        break;
                    }
                    c = next_c;
                }
                if (next_c == EOF)
                {
                    printf("Error: Cannot find end mark for comment\n");
                    exit(1);
                }
            }
            else if (next_c == '/')
            {
                // 直接读取下一行
                for (next_c = fgetc(input_file); next_c != EOF; next_c = fgetc(input_file))
                {
                    if (next_c == '\n')
                    { // 找到换行符
                        last_c = c, c = next_c;
                        next_c = fgetc(input_file); // 读取下一个字符
                        break;
                    }
                }
            }
        }

        fputc(c, output_file); // 将字符写入输出文件
    }
    fputc(c, output_file); // 将最后一个字符写入输出文件
}
