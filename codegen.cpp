//
// Created by Christopher Redden on 01/08/15.
//

#include <stdarg.h>
#include <stdio.h>
#include "codegen.h"
#include "nodes.h"
#include "kuma_string.h"

using namespace std;

extern "C"
{

    void println(u_int8_t *kstring_src, ...)
    {
        //va_list args;
        //va_start(args, kstring_src);
        //__ks__string__type *kstring_src_ptr = (__ks__string__type*)kstring_src;
        //printf((char*)kstring_src_ptr->buf, __VA_ARGS__);
        //printf("%i %i\n", args);
        //va_end (args);

        /*va_list ap;
        int j;
        double sum = 0;

        va_start(ap, count);
        for (j = 0; j < count; j++) {
            sum += va_arg(ap, double);
        }
        va_end(ap);*/
        //printf("Kuma Print: %s\n", kstring_src_ptr->buf);
        printf((char*)kstring_src);
    }
}

void generateStringFunctions(CodeGenContext& context)
{
    // kuma_string_create
    {
        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), false);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage,
                                                          "kuma_string_create", context.module);
        context.addFunction("kuma_string_create", function, 1, true);
    }

    // kuma_string_destroy
    {
        vector<llvm::Type *> args;
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()),
                                                                   llvm::makeArrayRef(args), false);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage,
                                                          "kuma_string_destroy", context.module);
        context.addFunction("kuma_string_destroy", function, 0, true);
    }

    // kuma_string_resize
    {
        vector<llvm::Type *> args;
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
        args.push_back(llvm::Type::getInt32Ty(llvm::getGlobalContext()));

        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()),
                                                                   llvm::makeArrayRef(args), false);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage,
                                                          "kuma_string_resize", context.module);
        context.addFunction("kuma_string_resize", function, 0, true);
    }

    // kuma_string_add_char
    {
        vector<llvm::Type *> args;
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
        args.push_back(llvm::Type::getInt8Ty(llvm::getGlobalContext()));

        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()),
                                                                   llvm::makeArrayRef(args), false);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage,
                                                          "kuma_string_add_char", context.module);
        context.addFunction("kuma_string_add_char", function, 0, true);
    }

    // kuma_string_append
    {
        vector<llvm::Type *> args;
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()),
                                                                   llvm::makeArrayRef(args), false);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage,
                                                          "kuma_string_append", context.module);
        context.addFunction("kuma_string_append", function, 0, true);
    }

    // kuma_string_copy
    {
        vector<llvm::Type *> args;
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()),
                                                                   llvm::makeArrayRef(args), false);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage,
                                                          "kuma_string_copy", context.module);
        context.addFunction("kuma_string_copy", function, 0, true);
    }

    // kuma_string_set
    {
        vector<llvm::Type *> args;
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
        args.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

        llvm::FunctionType *functionType = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()),
                                                                   llvm::makeArrayRef(args), false);
        llvm::Function *function = llvm::Function::Create(functionType, llvm::GlobalValue::ExternalLinkage,
                                                          "kuma_string_set", context.module);
        context.addFunction("kuma_string_set", function, 0, true);
    }
}

void generateStdlib(CodeGenContext& context)
{
    vector<llvm::Type*> argTypes;
    argTypes.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

    llvm::FunctionType *functype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), llvm::makeArrayRef(argTypes), true);
    llvm::Function *function = llvm::Function::Create(functype, llvm::GlobalValue::ExternalLinkage, "println", context.module);
    context.addFunction("println", function, 0, true);

    // String Functions
    generateStringFunctions(context);
}

void CodeGenContext::generateCode(NBlock &root)
{
    std::cout << "Generating code...\n";

    generateStdlib(*this);

    vector<llvm::Type*> argTypes;
    llvm::FunctionType *functype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), llvm::makeArrayRef(argTypes), false);
    mainFunction = llvm::Function::Create(functype, llvm::GlobalValue::ExternalLinkage, "ks__main", module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root.codeGen(*this);
    llvm::ReturnInst::Create(llvm::getGlobalContext(), bblock);
    popBlock();

    std::cout << "Code is generated.\n";
    module->dump();
}

llvm::GenericValue CodeGenContext::runCode()
{
    std::cout << "Running code...\n";
    string errStr;

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    //module->setTargetTriple("x86_64-apple-darwin11.0.0");

    if(llvm::Triple(llvm::sys::getProcessTriple()).isOSDarwin())
    {
        module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 2);
    }

    llvm::ExecutionEngine *ee = llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module))
                                                    .setErrorStr(&errStr)
                                                    .setVerifyModules(true)
                                                    .setMCJITMemoryManager(llvm::make_unique<llvm::SectionMemoryManager>())
                                                    .create();
    ee->finalizeObject();
    cout << "Target Triple: " << ee->getTargetMachine()->getTargetTriple().str() << endl;

    //module->setDataLayout(ee->getDataLayout());

    cout << errStr << endl;

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