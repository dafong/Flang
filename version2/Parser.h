//
// Created by 范鑫磊 on 17/2/20.
//

#ifndef FLANG_PARSER_H
#define FLANG_PARSER_H


#include <string>
#include <vector>

class ExprAST{
public :
    virtual  ~ExprAST(){}
};

class NumberExprAst : public ExprAST{
    double val;
public :
    NumberExprAst(double v)
            :val(v){}

};

class VariableExprAST : public ExprAST{
    std::string name;
public:
    VariableExprAST(const std::string &n):name(n){

    }
};

class BinaryExprAST : public ExprAST{
    char op;
    ExprAST *lhs,*rhs;
public:
    BinaryExprAST(char o,ExprAST *l,ExprAST *r)
            :op(o),lhs(l),rhs(r){

    }
};

class CallExprAST : public ExprAST{
    std::string callee;
    std::vector<ExprAST*> args;
public :
    CallExprAST(const std::string &c,std::vector<ExprAST*> &paras)
            : callee(c),args(paras){
    }
};

class PrototypeAST {
    std::string name;
    std::vector<std::string> args;
public:
    PrototypeAST(const std::string &n , const std::vector<std::string> &a)
            :name(n),args(a){

    }
};

class FunctionAST{
    PrototypeAST * proto;
    ExprAST *body;
public:
    FunctionAST(PrototypeAST *p,ExprAST *b)
            :proto(p),body(b){

    }
};


#endif //FLANG_PARSER_H