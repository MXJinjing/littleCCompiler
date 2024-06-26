cmake_minimum_required(VERSION 3.26)
project(littleCC C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")

include_directories(${PROJECT_SOURCE_DIR}/heads)

add_executable(littleCC
        tree.c
        graph_utils.c
        lexer.c
        main.c
        quad_utils.c
        parser.c
        prepare.c
        translator.c
        includes/grammar_utils.h
        includes/graph.h
        includes/graph_utils.h
        includes/lexer.h
        includes/lexer_utils.h
        includes/node.h
        includes/parser.h
        includes/prepare.h
        includes/quad_utils.h
        includes/tree.h
        includes/tree_utils.h
        includes/heads.h
        includes/translator.h
)

