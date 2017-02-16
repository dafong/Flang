cmake_minimum_required(VERSION 3.5)
project(test)
set(SOURCE_FILES y.tab.c lex.yy.c)
add_executable(test1 ${SOURCE_FILES})
