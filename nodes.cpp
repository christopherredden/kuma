//
// Created by Christopher Redden on 28/07/15.
//

#include "nodes.h"
#include "codegen.h"
#include "kuma_parser.h"

#include <iostream>

using namespace std;

/* Returns an LLVM type based on the identifier */
static llvm::Type *typeOf(const NIdentifier& type)
{
    if (type.name.compare("int") == 0)
    {
        return llvm::Type::getInt64Ty(llvm::getGlobalContext());
    }
    else if (type.name.compare("double") == 0)
    {
        return llvm::Type::getDoubleTy(llvm::getGlobalContext());
    }

    return llvm::Type::getVoidTy(llvm::getGlobalContext());
}

llvm::Value* NBlock::codeGen(CodeGenContext& context)
{
    StatementList::const_iterator it;
    llvm::Value *last = NULL;
    for (it = statements.begin(); it != statements.end(); it++)
    {
        std::cout << "Generating code for " << typeid(**it).name() << std::endl;
        last = (**it).codeGen(context);
    }
    std::cout << "Creating block" << std::endl;

    return last;
}

llvm::Value* NInteger::codeGen(CodeGenContext& context)
{
    std::cout << "Creating integer: " << value << std::endl;
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), value, true);
}

llvm::Value* NDouble::codeGen(CodeGenContext& context)
{
    std::cout << "Creating double: " << value << std::endl;
    return llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), value);
}

llvm::Value* NIdentifier::codeGen(CodeGenContext& context)
{
    std::cout << "Creating identifier reference: " << name << std::endl;
    if (context.locals().find(name) == context.locals().end())
    {
        std::cerr << "undeclared variable " << name << endl;
        return NULL;
    }

    return new llvm::LoadInst(context.locals()[name], "", false, context.currentBlock());

    //return new llvm::LoadInst(context.locals()[name], "", false, context.currentBlock());
}

llvm::Value* NMethodCall::codeGen(CodeGenContext& context)
{
    llvm::Function *function = context.module->getFunction(id.name.c_str());
    if (function == NULL)
    {
        cerr << "no such function " << id.name << endl;
    }
    vector<llvm::Value*> args;
    ExpressionList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        args.push_back((**it).codeGen(context));
    }
    llvm::CallInst *call = llvm::CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
    std::cout << "Creating method call: " << id.name << endl;
    return call;
}

llvm::Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
    std::cout << "Creating binary operation " << op << std::endl;

    llvm::Instruction::BinaryOps instr;
    switch (op)
    {
        case TPLUS: 	instr = llvm::Instruction::Add; break;
        case TMINUS: 	instr = llvm::Instruction::Sub; break;
        case TMUL: 		instr = llvm::Instruction::Mul; break;
        case TDIV: 		instr = llvm::Instruction::SDiv; break;
        default:        return NULL;
    }

    return llvm::BinaryOperator::Create(instr, lhs.codeGen(context), rhs.codeGen(context), "", context.currentBlock());
}

llvm::Value* NAssignment::codeGen(CodeGenContext& context)
{
    //std::cout << "Creating assignment for " << lhs.name << std::endl;
    llvm::Value *value = rhs.codeGen(context);
    IdentList::const_iterator it;
    for (it = lhs.begin(); it != lhs.end(); it++)
    {
        if (context.locals().find((**it).name) == context.locals().end())
        {
            std::cerr << "undeclared variable " << (**it).name << endl;
            return NULL;
        }

        new llvm::StoreInst(value, context.locals()[(**it).name], false, context.currentBlock());
    }

    return NULL;
}

llvm::Value* NAssignmentMethodCall::codeGen(CodeGenContext& context)
{

    llvm::Function *function = context.module->getFunction(rhs.id.name.c_str());
    llvm::StructType *type = (llvm::StructType*)function->getReturnType();
    llvm::Value *returnVal = new llvm::AllocaInst(type, "", context.currentBlock());

    llvm::CallInst *call = (llvm::CallInst*)rhs.codeGen(context);
    llvm::Value *storeVal = new llvm::StoreInst(call, returnVal, false, context.currentBlock());

    vector<llvm::Value*> indices;

    indices.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, 0, false)));

    unsigned i = 0;
    IdentList::const_iterator it;
    for (it = lhs.begin(); it != lhs.end(); it++)
    {
        if (context.locals().find((**it).name) == context.locals().end())
        {
            std::cerr << "undeclared variable " << (**it).name << endl;
            return NULL;
        }

        if(i < type->getNumElements())
        {
            indices.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, i, false)));
            llvm::GetElementPtrInst *elementPtr = llvm::GetElementPtrInst::CreateInBounds(returnVal, indices, "", context.currentBlock());

            llvm::Value* loadPtr = new llvm::LoadInst(elementPtr, "", context.currentBlock());
            llvm::Value* storeInst = new llvm::StoreInst(loadPtr, context.locals()[(**it).name], false, context.currentBlock());
            indices.pop_back();
        }
        i++;
    }

    return NULL;
}

llvm::Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
    std::cout << "Generating code for " << typeid(expression).name() << std::endl;
    return expression.codeGen(context);
}

llvm::Value* NReturnStatement::codeGen(CodeGenContext& context)
{
    llvm::Function *function = context.getCurrentFunction();
    llvm::StructType *type = (llvm::StructType*)function->getReturnType();
    llvm::Value *returnVal = new llvm::AllocaInst(type, "return_struct", context.currentBlock());

    vector<llvm::Value*> indices;
    indices.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, 0, false)));

    ExpressionList::const_iterator it;
    int i = 0;
    for (it = expressions.begin(); it != expressions.end(); it++)
    {
        llvm::Value *expressionValue = (**it).codeGen(context);

        indices.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, i, false)));
        llvm::GetElementPtrInst *elementPtr = llvm::GetElementPtrInst::CreateInBounds(returnVal, indices, "", context.currentBlock());

        llvm::Value* storeInst = new llvm::StoreInst(expressionValue, elementPtr, false, context.currentBlock());

        indices.pop_back();
        i++;
    }

    llvm::LoadInst *loadInst = new llvm::LoadInst(returnVal, "", context.currentBlock());
    context.setCurrentReturnValue(loadInst);

    return loadInst;
}

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
    std::cout << "Creating variable declaration " << type.name << " " << id.name << std::endl;
    llvm::AllocaInst *alloc = new llvm::AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
    context.locals()[id.name] = alloc;
    if (assignmentExpr != NULL)
    {
        IdentList identList;
        identList.push_back(&id);
        NAssignment assn(identList, *assignmentExpr);
        assn.codeGen(context);
    }
    return alloc;
}

llvm::Value* NExternDeclaration::codeGen(CodeGenContext& context)
{
    /*vector<Type*> argTypes;
    VariableList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
    return function;*/
}

llvm::Value* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
    vector<llvm::Type*> argTypes;
    VariableList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        argTypes.push_back(typeOf((**it).type));
    }

    // Create return struct
    vector<llvm::Type*> returnTypes;
    IdentList::const_iterator ident;
    for (ident = returns.begin(); ident != returns.end(); ident++)
    {
        returnTypes.push_back(typeOf((**ident)));
    }

    string structTypeName = id.name + "_return_struct";
    llvm::StructType* rstruct = llvm::StructType::create(llvm::getGlobalContext(), llvm::ArrayRef<llvm::Type*>(returnTypes), structTypeName.c_str(), false); // isPacked

    llvm::FunctionType *ftype = llvm::FunctionType::get(rstruct, llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, id.name.c_str(), context.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0);

    context.pushBlock(bblock);
    context.setCurrentFunction(function);

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;

    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        (**it).codeGen(context);

        argumentValue = argsValues++;
        argumentValue->setName((*it)->id.name.c_str());
        llvm::StoreInst *inst = new llvm::StoreInst(argumentValue, context.locals()[(*it)->id.name], false, bblock);
    }

    block.codeGen(context);
    llvm::ReturnInst::Create(llvm::getGlobalContext(), context.getCurrentReturnValue(), bblock);

    context.popBlock();

    std::cout << "Creating function: " << id.name << std::endl;
    return function;
}