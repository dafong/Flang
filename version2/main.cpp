//
// Created by 范鑫磊 on 17/2/20.
//

#include <map>
#include "Lexer.h"
#include "Parser.h"

static ExprAST *ParseExpression(Lexer *);

//a+b+(c+d)*e*f+g

static std::map<char,int> BinOpPrecedence;
static int GetTokenPrecedence(Lexer *l){
    if(!isascii(l->curToken))
        return -1;
    int prec = BinOpPrecedence[l->curToken];
    if(prec<=0) return -1;
    return prec;
}



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

static ExprAST *ParseBinOpRhs(Lexer *l,int exprPrec,ExprAST *lhs){
    while(1){
        int tokprec = GetTokenPrecedence(l);
        if(tokprec < exprPrec){
            return lhs;
        }
        int binop = l->curToken;
        l->getToken();
        ExprAST *rhs = ParsePrimary(l);
        int nextprec = GetTokenPrecedence(l);
        if(tokprec < nextprec){
            rhs = ParseBinOpRhs(l,tokprec + 1,rhs);
            if(rhs == 0) return 0;
        }
        lhs = new BinaryExprAST(binop,lhs,rhs);
    }
}

static ExprAST *ParseExpression(Lexer *l){
    ExprAST *lhs = ParsePrimary(l);
    if(!lhs) return 0;

    return ParseBinOpRhs(l,0,lhs);
}

static PrototypeAST *ParsePrototype(Lexer *l){
    if(l->curToken != tok_identifier){
        return ErrorP("Expect function name in prototype");
    }
    std::string funcName = l->identifier;

    l->getToken();
    if(l->curToken != '('){
        ErrorP("Expect '(' in prototype");
    }
    l->getToken(); //eat (
    std::vector<std::string> argNames;
    while(l->curToken == tok_identifier) {
        argNames.push_back(l->identifier);
        l->getToken();//eat param
        if(l->curToken == ',')
            l->getToken();// eat ','
    }

    if(l->curToken != ')')
        return ErrorP("Expect ')' in prototype");

    l->getToken();//eat )
    return new PrototypeAST(funcName,argNames);
}

static FunctionAST *ParseDefinition(Lexer *l){
    l->getToken();
    PrototypeAST *proto = ParsePrototype(l);
    if(proto == 0) return 0;
    if(ExprAST *e = ParseExpression(l)){
        return new FunctionAST(proto,e);
    }
    return 0;
}

static PrototypeAST *ParseExtern(Lexer *l){
    l->getToken();
    return ParsePrototype(l);
}

static FunctionAST *ParseTopLevelExpr(Lexer *l){
    if(ExprAST *e = ParseExpression(l)){
        PrototypeAST *proto = new PrototypeAST("",std::vector<std::string>());
        return new FunctionAST(proto,e);
    }
    return 0;
}


//=========================================MainLoop========================================
static void HandleDefinition(Lexer *l){
    if (auto fnAst = ParseDefinition(l)) {
      if(auto *fnIR= fnAst->CodeGen()){
          fprintf(stderr,"Read function definition:");
          fnIR->print(errs());
          fprintf(stderr,"\n");
      }
    } else {
      // Skip token for error recovery.
      l->getToken();
    }
}

static void HandleExtern(Lexer *l){
    if (auto protoAst = ParseExtern(l)) {
        if(auto *protoIR = protoAst->CodeGen()) {
            fprintf(stderr, "Read extern:");
            protoIR->print(errs());
            fprintf(stderr,"\n");
        }
    } else {
      // Skip token for error recovery.
        l->getToken();
    }
}

static void HandleTopLevelExpression(Lexer *l){
    if (auto fnAst = ParseTopLevelExpr(l)) {
        if(auto *fnIR = fnAst->CodeGen()){
            fprintf(stderr,"Read top-level expression:");
            fnIR->print(errs());
            fprintf(stderr,"\n");
        }

    } else {
      // Skip token for error recovery.
      l->getToken();
    }
}

static void MainLoop(Lexer *l){
    while (1) {
        fprintf(stderr, "ready> ");
        switch (l->curToken) {
        case tok_eof:    return;
        case ';':        l->getToken(); break;  // ignore top-level semicolons.
        case tok_def:    HandleDefinition(l); break;
        case tok_extern: HandleExtern(l); break;
        default:         HandleTopLevelExpression(l); break;
        }
      }
}
extern std::unique_ptr<Module> TheModule;
extern  LLVMContext TheContext;

int main(){
    Lexer *l = new Lexer();
    BinOpPrecedence['<'] = 10;
    BinOpPrecedence['+'] = 20;
    BinOpPrecedence['-'] = 20;
    BinOpPrecedence['*'] = 40;

    fprintf(stderr, "ready>" );
    l->getToken();
    TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
    MainLoop(l);
    TheModule->print(errs(), nullptr);
    return 0;
}
