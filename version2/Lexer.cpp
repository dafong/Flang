//
// Created by 范鑫磊 on 17/2/20.
//

#include "Lexer.h"

#include <cstdlib>

void Lexer::setToken(int t){
    curToken = t;
}

int Lexer::getToken() {

    while(isspace(lastchar)){
        lastchar = getchar();
    }

    if(isalpha(lastchar)){
        identifier = lastchar;
        while(isalnum(lastchar = getchar())){
            identifier += lastchar;
        }
        if(identifier == "def") {
            setToken(tok_def);
            return tok_def;
        }
        if(identifier == "extern"){
            setToken(tok_extern);
            return tok_extern;
        }
        setToken(tok_identifier);
        return tok_identifier;
    }

    if(isdigit(lastchar) || lastchar == '.'){
        std::string numStr;
        do{
            numStr += lastchar;
            lastchar = getchar();
        }while(isdigit(lastchar) || lastchar == '.' );
        numVal = strtod(numStr.c_str(),0);
        setToken(tok_number);
        return tok_number;
    }

    if(lastchar == '#'){
        do lastchar =  getchar();
        while(lastchar != EOF && lastchar != '\n' && lastchar != '\r');
        if(lastchar != EOF){
            return this->getToken();
        }
    }

    if(lastchar == EOF){
        setToken(tok_eof);
        return tok_eof;
    }
    int thisChar =  lastchar;
    setToken(thisChar);
    lastchar = getchar();
    return thisChar;
}
