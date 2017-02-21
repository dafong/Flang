//
// Created by 范鑫磊 on 17/2/20.
//

#ifndef FLANG_PARSER_H
#define FLANG_PARSER_H


#include <string>
#include <vector>
#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

class ExprAST{
public :
    virtual  ~ExprAST(){}
    virtual  Value *CodeGen() = 0;
};

class NumberExprAst : public ExprAST{
    double val;
public :
    NumberExprAst(double v)
            :val(v){}
    virtual Value *CodeGen() override;
};

class VariableExprAST : public ExprAST{
    std::string name;
public:
    VariableExprAST(const std::string &n):name(n){}

    virtual Value *CodeGen() override;

};

class BinaryExprAST : public ExprAST{
    char op;
    ExprAST *lhs,*rhs;
public:
    BinaryExprAST(char o,ExprAST *l,ExprAST *r)
            :op(o),lhs(l),rhs(r){}

    virtual Value *CodeGen() override;
};

class CallExprAST : public ExprAST{
    std::string callee;
    std::vector<ExprAST*> args;
public :
    CallExprAST(const std::string &c,std::vector<ExprAST*> &paras)
            : callee(c),args(paras){}

    virtual Value *CodeGen() override;
};

class PrototypeAST {
    std::string name;
    std::vector<std::string> args;
public:
    PrototypeAST(const std::string &n , const std::vector<std::string> &a)
            :name(n),args(a){}
    const std::string &getName() const { return this->name; }
    Function *CodeGen();
};

class FunctionAST{
    PrototypeAST * proto;
    ExprAST *body;
public:
    FunctionAST(PrototypeAST *p,ExprAST *b)
            :proto(p),body(b){}
    Function *CodeGen();
};


#endif //FLANG_PARSER_H