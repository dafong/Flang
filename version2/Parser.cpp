//
// Created by 范鑫磊 on 17/2/20.
//

#include "Parser.h"

std::unique_ptr<Module> TheModule;
 LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::map<std::string, Value *> NamedValues;

Value *LogErrorV(const std::string &msg){
    fprintf(stderr,"%s",msg.c_str());
    return nullptr;
}

Value *NumberExprAst::CodeGen() {
    return ConstantFP::get(TheContext,APFloat(val));
}

Value *VariableExprAST::CodeGen() {
    Value *V = NamedValues[name];
    if(!V)
        LogErrorV("Unknown variable name");
    return V;
}

Value *BinaryExprAST::CodeGen() {
    Value *L = lhs->CodeGen();
    Value *R = rhs->CodeGen();
    if(!L || !R) return nullptr;
    switch (op){
        case '+':
            return Builder.CreateFAdd(L,R,"addtmp");
        case '-':
            return Builder.CreateFSub(L,R,"subtmp");
        case '*':
            return Builder.CreateFMul(L,R,"multmp");
        case '<':
            L = Builder.CreateFCmpULT(L,R,"cmptmp");
            return Builder.CreateUIToFP(L,Type::getDoubleTy(TheContext),"booltmp");
        default:
            return LogErrorV("invalid binary operator");
    }
}

Value *CallExprAST::CodeGen() {
    Function *CalleeF = TheModule->getFunction(callee);
    if(!CalleeF)
        return LogErrorV("Unknow function refrenced");
    if(CalleeF->arg_size() != this->args.size()){
        return LogErrorV("Incorrect # arguments passed");
    }
    std::vector<Value *> ArgsV;
    for(unsigned i = 0,e=args.size(); i!=e;++i){
        ArgsV.push_back(args[i]->CodeGen());
        if(!ArgsV.back())
            return nullptr;
    }
    return Builder.CreateCall(CalleeF,ArgsV,"calltmp");
}

Function *PrototypeAST::CodeGen() {
    std::vector<Type *> Doubles(args.size(),Type::getDoubleTy(TheContext));
    FunctionType *FT = FunctionType::get(Type::getDoubleTy(TheContext),Doubles,false);
    Function *F = Function::Create(FT,Function::ExternalLinkage,name,TheModule.get());
    unsigned  idx = 0;
    for(auto &arg : F->args()){
        arg.setName(args[idx ++]);
    }
    return F;
}

Function *FunctionAST::CodeGen() {
    Function *TheFunction = TheModule->getFunction(proto->getName());
    if(!TheFunction)
        TheFunction = proto->CodeGen();

    if(!TheFunction)
        return nullptr;

    if(!TheFunction->empty())
        return (Function *)LogErrorV("Functoin cannot be redefined");

    BasicBlock *BB = BasicBlock::Create(TheContext,"entry",TheFunction);
    Builder.SetInsertPoint(BB);
    NamedValues.clear();
    for(auto &arg : TheFunction->args())
        NamedValues[arg.getName()] = &arg;

    if(Value *retval = body->CodeGen()){
        Builder.CreateRet(retval);
        verifyFunction(*TheFunction);
        return TheFunction;
    }
    TheFunction->eraseFromParent();
    return nullptr;
}


