//
// Created by Christopher Redden on 01/08/15.
//

#include "codegen.h"
#include "nodes.h"

using namespace std;

extern "C"
{
    void println(int v)
    {
        printf("Kuma Print: %d\n", v);
    }
}

void generateStdlib(CodeGenContext& context)
{
    vector<llvm::Type*> argTypes;
    argTypes.push_back(llvm::Type::getInt32Ty(llvm::getGlobalContext()));

    llvm::FunctionType *functype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(functype, llvm::GlobalValue::ExternalLinkage, "println", context.module);
}

void CodeGenContext::generateCode(NBlock &root)
{
    std::cout << "Generating code...\n";

    generateStdlib(*this);

    vector<llvm::Type*> argTypes;
    llvm::FunctionType *functype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), llvm::makeArrayRef(argTypes), false);
    mainFunction = llvm::Function::Create(functype, llvm::GlobalValue::InternalLinkage, "main", module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root.codeGen(*this);
    llvm::ReturnInst::Create(llvm::getGlobalContext(), bblock);
    popBlock();

    std::cout << "Code is generated.\n";
    llvm::legacy::PassManager pm;
    pm.add(llvm::createPrintModulePass(llvm::outs()));
    pm.run(*module);
}

llvm::GenericValue CodeGenContext::runCode()
{
    std::cout << "Running code...\n";
    string errStr;
    unique_ptr<llvm::Module> pmodule(module);

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::ExecutionEngine *ee = llvm::EngineBuilder(std::move(pmodule))
                                                    .setErrorStr(&errStr)
                                                    .setEngineKind(llvm::EngineKind::JIT)
                                                    .create();

    if(ee == NULL)
    {
        cout << "Engine Builder Error! " << errStr << endl;
        llvm::GenericValue r;
        r.IntVal = 0;
        return r;
    }

    vector<llvm::GenericValue> noargs;
    llvm::GenericValue v = ee->runFunction(mainFunction, noargs);
    std::cout << "Code was run.\n";
    return v;
}