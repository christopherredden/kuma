//
// Created by Christopher Redden on 01/08/15.
//

#ifndef KUMA_CODEGEN_H
#define KUMA_CODEGEN_H

#include <map>
#include <vector>
#include <string>
#include <stack>
#include <iostream>
#include <sstream>

// LLVM
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/Pass.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Host.h"
#include "llvm/ADT/Triple.h"

using namespace std;

class NBlock;

class CodeGenLocal
{
public:
    llvm::Value *value;
    int type;

    CodeGenLocal(llvm::Value* value, int type) : value(value), type(type) {}
    CodeGenLocal() : value(0), type(0) {}
};

class CodeGenBlock
{
public:
    llvm::BasicBlock* block;
    llvm::Value* currentReturnValue;
    llvm::Function* currentFunction;
    map<string, CodeGenLocal> locals;
    llvm::IRBuilder<> *builder;
};

class CodeGenFunction
{
public:
    bool isExtern;
    int numReturns;
    llvm::Function *functionValue;
};

class CodeGenContext
{
private:
    stack<CodeGenBlock *> blocks;
    map<string, CodeGenFunction> functionMap;
    llvm::Function *mainFunction;

public:
    llvm::Module *module;
    llvm::IRBuilder<> *globalBuilder;
    CodeGenContext() { module = new llvm::Module("ks__main__module", llvm::getGlobalContext()); globalBuilder = new llvm::IRBuilder<>(llvm::getGlobalContext()); }

    void generateCode(NBlock& root);
    llvm::GenericValue runCode();

    map<string, CodeGenLocal>& locals() { return blocks.top()->locals; }
    map<string, CodeGenFunction> &functions() { return functionMap; }
    llvm::BasicBlock *currentBlock() { return blocks.top()->block; }
    llvm::IRBuilder<> *currentBuilder() { return blocks.top()->builder; }

    void addFunction(string name, llvm::Function *function, int numReturns, bool isExtern)
    {
        CodeGenFunction codeGenFunction;
        codeGenFunction.functionValue = function;
        codeGenFunction.numReturns = numReturns;
        codeGenFunction.isExtern = isExtern;

        functionMap[name] = codeGenFunction;
    }

    void pushBlock(llvm::BasicBlock *block) { blocks.push(new CodeGenBlock()); blocks.top()->block = block; blocks.top()->builder = new llvm::IRBuilder<>(block);}
    void popBlock() { CodeGenBlock *top = blocks.top(); blocks.pop(); delete top; }

    void setCurrentReturnValue(llvm::Value *value) { blocks.top()->currentReturnValue = value; }
    llvm::Value* getCurrentReturnValue() { return blocks.top()->currentReturnValue; }

    void setCurrentFunction(llvm::Function *function) { blocks.top()->currentFunction = function; }
    llvm::Function* getCurrentFunction() { return blocks.top()->currentFunction; }
};

#endif //KUMA_CODEGEN_H
