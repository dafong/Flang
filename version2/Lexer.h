//
// Created by 范鑫磊 on 17/2/20.
//

#ifndef FLANG_LEXER_H
#define FLANG_LEXER_H

#include <cstdio>
#include <string>

enum Token{
    tok_eof  = -1,
    tok_def  = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5
};

class Lexer {
    int lastchar;

    void setToken(int);
public:
    int curToken;
    std::string identifier;
    double numVal;

    Lexer()
            :lastchar(' '){}
    int getToken();
};


#endif //FLANG_LEXER_H
