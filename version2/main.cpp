//
// Created by 范鑫磊 on 17/2/20.
//

#include <map>
#include "Lexer.h"
#include "Parser.h"
static ExprAST *ParseExpression(Lexer *);

static ExprAST *Error(const char *str){
    fprintf(stderr,"Error %s \n",str); return 0;
}

static PrototypeAST *ErrorP(const char *str){
    Error(str);
    return 0;
}

static FunctionAST * ErrorF(const char *str){
    Error(str);
    return 0;
}

static ExprAST *ParseNumExpr(Lexer *l){
    ExprAST *ast = new NumberExprAst(l->numVal);
    l->getToken();
    return ast;
}

static ExprAST *ParseParenExpr(Lexer *l){
    l->getToken();
    ExprAST *v = ParseExpression(l);
    if(!v) return 0;
    if(l->curToken != ')')
        return Error("expect ')'");
    l->getToken();
    return v;
}


static ExprAST *ParseIdentifierExpr(Lexer *l) {
    std::string idname = l->identifier;
    l->getToken();
    if (l->curToken != '(') {
        return new VariableExprAST(idname);
    }
    l->getToken();
    std::vector<ExprAST *> args;
    if (l->curToken != ')') {
        while (1) {
            ExprAST *arg = ParseExpression(l);
            if(!arg) return 0;
            args.push_back(arg);
            if(l->curToken == ')') break;
            if(l->curToken != ',')
                return Error("Expect ')' or ',' in argument list");
            l->getToken();
        }
    }
    l->getToken();
    return new CallExprAST(idname,args);
}

static ExprAST *ParsePrimary(Lexer *l){
    switch (l->curToken){
        case tok_identifier: return ParseIdentifierExpr(l);
        case tok_number: return ParseNumExpr(l);
        case '(': return ParseParenExpr(l);
        default:
            return Error("unknow token when expecting a expression");
    }
}

static ExprAST *ParseBinOpRhs(int exprPrec,Lexer *l){
    return 0;
}

static ExprAST *ParseExpression(Lexer *l){
    ExprAST *lhs = ParsePrimary(l);
    if(!lhs) return 0;

    return ParseBinOpRhs(0,l);
}


//a+b+(c+d)*e*f+g

static std::map<char,int> BinOpPrecedence;
static int GetTokenPrecedence(Lexer *l){
    if(!isascii(l->curToken))
        return -1;
    int prec = BinOpPrecedence[l->curToken];
    if(prec<=0) return -1;
    return prec;
}






int main(){
    Lexer *l = new Lexer();
    BinOpPrecedence['<'] = 10;
    BinOpPrecedence['+'] = 20;
    BinOpPrecedence['-'] = 20;
    BinOpPrecedence['*'] = 40;

    return 0;
}