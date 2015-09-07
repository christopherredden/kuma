//
// Created by Christopher Redden on 28/07/15.
//

#include "nodes.h"
#include "codegen.h"
#include "kuma_parser.h"
#include "kuma.h"

#include <iostream>

using namespace std;

/* Returns an LLVM type based on the identifier */
static llvm::Type *typeOf(NIdentifier *type)
{
    if (type->name.compare("int") == 0)
    {
        return llvm::Type::getInt32Ty(llvm::getGlobalContext());
    }
    else if (type->name.compare("double") == 0)
    {
        return llvm::Type::getDoubleTy(llvm::getGlobalContext());
    }
    else if (type->name.compare("string") == 0)
    {
        return llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
    }

    return llvm::Type::getVoidTy(llvm::getGlobalContext());
}

static std::string typeOf(llvm::Type* type)
{
    if (type == llvm::Type::getInt32Ty(llvm::getGlobalContext()))
    {
        return "int";
    }
    else if (type == llvm::Type::getDoubleTy(llvm::getGlobalContext()))
    {
        return "double";
    }
    else if (type == llvm::Type::getInt8PtrTy(llvm::getGlobalContext()))
    {
        return "double";
    }

    return "void";
}

static int kumaTypeOf(llvm::Type* type)
{
    if (type == llvm::Type::getInt32Ty(llvm::getGlobalContext()))
    {
        return KUMA_TYPE_INT;
    }
    else if (type == llvm::Type::getDoubleTy(llvm::getGlobalContext()))
    {
        return KUMA_TYPE_DOUBLE;
    }
    else if (type == llvm::Type::getInt8PtrTy(llvm::getGlobalContext()))
    {
        return KUMA_TYPE_STRING;
    }

    return 0;
}

static int kumaTypeOf(NIdentifier *type)
{
    if (type->name.compare("int") == 0)
    {
        return KUMA_TYPE_INT;
    }
    else if (type->name.compare("double") == 0)
    {
        return KUMA_TYPE_DOUBLE;
    }
    else if (type->name.compare("string") == 0)
    {
        return KUMA_TYPE_STRING;
    }

    return 0;
}

static llvm::Type *typeOf(int kumaType)
{
    if (kumaType == KUMA_TYPE_INT || kumaType == KUMA_TYPE_CONST_INT)
    {
        return llvm::Type::getInt32Ty(llvm::getGlobalContext());
    }
    else if (kumaType == KUMA_TYPE_DOUBLE || kumaType == KUMA_TYPE_CONST_DOUBLE)
    {
        return llvm::Type::getDoubleTy(llvm::getGlobalContext());
    }
    else if (kumaType == KUMA_TYPE_STRING || kumaType == KUMA_TYPE_CONST_STRING)
    {
        return llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
    }

    return 0;
}

static std::string kumaTypeName(int kumaType)
{
    if (kumaType == KUMA_TYPE_INT)
    {
        return "KUMA_TYPE_INT";
    }
    else if (kumaType == KUMA_TYPE_DOUBLE)
    {
        return "KUMA_TYPE_DOUBLE";
    }
    else if (kumaType == KUMA_TYPE_STRING)
    {
        return "KUMA_TYPE_STRING";
    }

    return 0;
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

llvm::Value *NString::codeGen(CodeGenContext &context)
{
    std::cout << "Creating string: " << value << std::endl;
    value = value.substr(1, value.length() - 2);
    llvm::Value *val = context.currentBuilder()->CreateGlobalStringPtr(value.c_str());
    SET_KUMA_TYPE(val, KUMA_TYPE_CONST_STRING);
    return val;
}

llvm::Value* NInteger::codeGen(CodeGenContext& context)
{
    std::cout << "Creating integer: " << value << std::endl;
    Value *val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), value, true);
    SET_KUMA_TYPE(val, KUMA_TYPE_CONST_INT);
    return val;
}

llvm::Value* NDouble::codeGen(CodeGenContext& context)
{
    std::cout << "Creating double: " << value << std::endl;
    Value *val = llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), value);
    SET_KUMA_TYPE(val, KUMA_TYPE_CONST_DOUBLE);
    return val;
}

llvm::Value* NIdentifier::codeGen(CodeGenContext& context)
{
    std::cout << "Creating identifier reference: " << name << std::endl;
    if (context.locals().find(name) == context.locals().end())
    {
        std::cerr << "undeclared variable " << name << endl;
        return NULL;
    }

    if (L(name).type == KUMA_TYPE_UNKNOWN)
    {
        std::cerr << "Use of unassigned variable: " << name << endl;
    }

    Value *val = new llvm::LoadInst(context.locals()[name].value, "", false, context.currentBlock());
    SET_KUMA_TYPE(val, L(name).type);
    return val;
}

llvm::Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
    llvm::AllocaInst *alloc = NULL;
    llvm::Value *assignValue = NULL;
    llvm::Type *llvmType = NULL;

    if (assignmentExpr != NULL)
    {
        assignValue = assignmentExpr->codeGen(context);
        if(assignValue == NULL)
        {
            assignValue = assignmentExpr->values[0];
        }
    }

    // If we have no type, infer it from assignment
    int kumaType = 0;
    if(type == NULL && assignValue != NULL)
    {
        kumaType = GET_KUMA_TYPE(assignValue);

        if(kumaType == KUMA_TYPE_STRING || kumaType == KUMA_TYPE_CONST_STRING)
        {
            kumaType = KUMA_TYPE_STRING;
        }

        llvmType = typeOf(kumaType);
    }
    else if(type != NULL)
    {
        kumaType = kumaTypeOf(type);
        llvmType = typeOf(type);
    }

    // If we still have no type, we will defer
    if(llvmType == NULL)
    {
        // Add to Future context checking
        context.locals()[id->name] = CodeGenLocal();
        std::cout << "Creating deferred variable: " << id->name << endl;
    }
    else
    {
        std::cout << "Creating variable declaration " << typeOf(llvmType) << " " << id->name << std::endl;
        alloc = B().CreateAlloca(llvmType, NULL, id->name.c_str());
        SET_LOCAL(id->name, alloc, kumaType);
        SET_KUMA_TYPE(alloc, kumaType);

        if(kumaType == KUMA_TYPE_STRING || kumaType == KUMA_TYPE_CONST_STRING)
        {
            llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_create");
            llvm::Value *v = context.currentBuilder()->CreateCall(stringCreateFunction);
            context.currentBuilder()->CreateStore(v, alloc);
        }

        if(assignValue != NULL)
        {
            if(kumaType == KUMA_TYPE_STRING)
            {
                if(GET_KUMA_TYPE(assignValue) == KUMA_TYPE_CONST_STRING)
                {
                    llvm::Value *allocPtr = context.currentBuilder()->CreateLoad(alloc);
                    llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_set");
                    llvm::Value *v = context.currentBuilder()->CreateCall2(stringCreateFunction, allocPtr, assignValue);
                }
                else if(GET_KUMA_TYPE(assignValue) == KUMA_TYPE_STRING)
                {
                    llvm::Value *allocPtr = context.currentBuilder()->CreateLoad(alloc);
                    llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_copy");
                    llvm::Value *v = context.currentBuilder()->CreateCall2(stringCreateFunction, allocPtr, assignValue);
                }
                else
                {
                    cerr << "Unable to assign to string.";
                }
            }
            else
            {
                B().CreateStore(assignValue, alloc);
            }
        }
    }

    return alloc;
}

llvm::Value* NAssignment::codeGen(CodeGenContext& context)
{
    llvm::Value *value = rhs->codeGen(context);

    if(value != NULL)
    {
        IdentList::const_iterator it;
        for (it = lhs.begin(); it != lhs.end(); it++)
        {
            if (context.locals().find((**it).name) == context.locals().end())
            {
                std::cerr << "undeclared variable " << (**it).name << endl;
                return NULL;
            }

            std::cout << "Creating assignment for " << (**it).name << std::endl;

            auto local = context.locals()[(**it).name];

            // If Variable not been defined, define it now
            if(local.type == KUMA_TYPE_UNKNOWN)
            {
                llvm::Type *llvmType = value->getType();
                if(llvmType->isPointerTy() && llvmType->getPointerElementType()->isArrayTy())
                {
                    llvmType = llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
                }

                std::string name = (**it).name;
                std::cout << "Creating variable declaration " << typeOf(llvmType) << " " << name << std::endl;

                llvm::AllocaInst *alloc = new llvm::AllocaInst(llvmType, name.c_str(), context.currentBlock());
                local.type = kumaTypeOf(llvmType);
                local.value = alloc;
                context.locals()[name] = local;

                if(kumaTypeOf(llvmType) == KUMA_TYPE_STRING)
                {
                    llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_create");
                    llvm::Value *v = context.currentBuilder()->CreateCall(stringCreateFunction);
                    context.currentBuilder()->CreateStore(v, alloc);
                }

                // Debug Info
                std::string typeNameString = kumaTypeName(kumaTypeOf(llvmType));
                llvm::MDString *typeName = llvm::MDString::get(llvm::getGlobalContext(), typeNameString);
                llvm::MDNode *mdNode = llvm::MDNode::get(llvm::getGlobalContext(), typeName);
                alloc->setMetadata("TYPE", mdNode);
            }

            if(local.type == KUMA_TYPE_STRING)
            {
                if(value->getType()->getPointerElementType()->isArrayTy())
                {
                    llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
                    llvm::Value *Args[] = { zero, zero };
                    llvm::Value *assignValuePtr = context.currentBuilder()->CreateGEP(value, Args);

                    llvm::Value *allocPtr = context.currentBuilder()->CreateLoad(local.value);
                    llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_set");
                    llvm::Value *v = context.currentBuilder()->CreateCall2(stringCreateFunction, allocPtr, assignValuePtr);
                }
                else
                {
                    llvm::Value *allocPtr = context.currentBuilder()->CreateLoad(local.value);
                    llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_copy");
                    llvm::Value *v = context.currentBuilder()->CreateCall2(stringCreateFunction, allocPtr, value);
                }
            }
            else
            {
                new llvm::StoreInst(value, local.value, false, context.currentBlock());
            }
        }
    }
    else
    {
        for(unsigned i = 0; i < lhs.size(); i++)
        {
            NIdentifier *ident = lhs[i];

            if (context.locals().find(ident->name) == context.locals().end())
            {
                std::cerr << "undeclared variable " << ident->name << endl;
                return NULL;
            }

            if(i < rhs->values.size())
            {
                std::cout << "Creating assignment for " << ident->name << std::endl;

                auto local = context.locals()[ident->name];

                // If Variable not been defined, define it now
                if(local.type == KUMA_TYPE_UNKNOWN)
                {
                    llvm::Type *llvmType = rhs->values[i]->getType();
                    if(llvmType->isPointerTy() && llvmType->getPointerElementType()->isArrayTy())
                    {
                        llvmType = llvm::Type::getInt8PtrTy(llvm::getGlobalContext());
                    }

                    std::string name = ident->name;
                    std::cout << "Creating variable declaration " << typeOf(llvmType) << " " << name << std::endl;

                    llvm::AllocaInst *alloc = new llvm::AllocaInst(llvmType, name.c_str(), context.currentBlock());
                    local.type = kumaTypeOf(llvmType);
                    local.value = alloc;
                    context.locals()[name] = local;

                    if(kumaTypeOf(llvmType) == KUMA_TYPE_STRING)
                    {
                        llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_create");
                        llvm::Value *v = context.currentBuilder()->CreateCall(stringCreateFunction);
                        context.currentBuilder()->CreateStore(v, alloc);
                    }

                    // Debug Info
                    std::string typeNameString = kumaTypeName(kumaTypeOf(llvmType));
                    llvm::MDString *typeName = llvm::MDString::get(llvm::getGlobalContext(), typeNameString);
                    llvm::MDNode *mdNode = llvm::MDNode::get(llvm::getGlobalContext(), typeName);
                    alloc->setMetadata("TYPE", mdNode);
                }

                if(local.type == KUMA_TYPE_STRING)
                {
                    if(rhs->values[i]->getType()->getPointerElementType()->isArrayTy())
                    {
                        llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
                        llvm::Value *Args[] = { zero, zero };
                        llvm::Value *assignValuePtr = context.currentBuilder()->CreateGEP(rhs->values[i], Args);

                        llvm::Value *allocPtr = context.currentBuilder()->CreateLoad(local.value);
                        llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_set");
                        llvm::Value *v = context.currentBuilder()->CreateCall2(stringCreateFunction, allocPtr, assignValuePtr);
                    }
                    else
                    {
                        llvm::Value *allocPtr = context.currentBuilder()->CreateLoad(local.value);
                        llvm::Function *stringCreateFunction = context.module->getFunction("kuma_string_copy");
                        llvm::Value *v = context.currentBuilder()->CreateCall2(stringCreateFunction, allocPtr, rhs->values[i]);
                    }
                }
                else
                {
                    new llvm::StoreInst(rhs->values[i], local.value, false,
                                        context.currentBlock());
                }
            }
        }
    }

    return NULL;
}

llvm::Value* NMethodCall::codeGen(CodeGenContext& context)
{
    if(context.functions()[id->name].numReturns > 1)
    {
        codeGenMultiReturn(context);
    }
    else
    {
        return codeGenSingleReturn(context);
    }

    return NULL;
}

llvm::Value *NMethodCall::codeGenSingleReturn(CodeGenContext &context)
{
    llvm::Function *function = context.module->getFunction(id->name.c_str());
    if (function == NULL)
    {
        cerr << "no such function " << id->name << endl;
    }

    vector<llvm::Value*> args;
    ExpressionList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        NExpression* expr = *it;
        llvm::Value* val = expr->codeGen(context);

        if(val != NULL)
        {
            args.push_back(val);
        }
        else
        {
            for(int i = 0; i < expr->values.size(); i++)
            {
                args.push_back(expr->values[i]);
            }
        }
    }

    args.resize(function->getFunctionType()->getNumParams());
    std::cout << "Creating method call: " << id->name << endl;
    llvm::CallInst *call = llvm::CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
    return call;
}

void NMethodCall::codeGenMultiReturn(CodeGenContext &context)
{
   llvm::Function *function = context.module->getFunction(id->name.c_str());
    if (function == NULL)
    {
        cerr << "no such function " << id->name << endl;
    }

    llvm::Type *typeptr = function->arg_begin()->getType();
    llvm::Value *returnVal = new llvm::AllocaInst(typeptr, "", context.currentBlock());
    llvm::Value *ptr = context.currentBuilder()->CreateBitOrPointerCast(returnVal, typeptr);
    llvm::StructType *type = (llvm::StructType*)typeptr->getPointerElementType();

    vector<llvm::Value*> args;
    args.push_back(ptr); // Return ptr
    ExpressionList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        NExpression* expr = *it;
        llvm::Value* val = expr->codeGen(context);

        if(val != NULL)
        {
            args.push_back(val);
        }
        else
        {
            for(int i = 0; i < expr->values.size(); i++)
            {
                args.push_back(expr->values[i]);
            }
        }
    }

    args.resize(function->getFunctionType()->getNumParams());
    llvm::CallInst *call = llvm::CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
    std::cout << "Creating method call: " << id->name << endl;

    // Create return values
    vector<llvm::Value*> indices;
    indices.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, 0, false)));

    for (unsigned i = 0; i < type->getNumElements(); i++)
    {
        if(i < type->getNumElements())
        {
            indices.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(32, i, false)));
            llvm::GetElementPtrInst *elementPtr = llvm::GetElementPtrInst::CreateInBounds(ptr, indices, "", context.currentBlock());

            llvm::Value *loadPtr = new llvm::LoadInst(elementPtr, "", context.currentBlock());
            values.push_back(loadPtr);
            indices.pop_back();
        }
    }
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

    Value *val = llvm::BinaryOperator::Create(instr, lhs->codeGen(context), rhs->codeGen(context), "", context.currentBlock());
    SET_KUMA_TYPE(val, KUMA_TYPE_INT);
    return val;
}

llvm::Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
    std::cout << "Generating code for " << typeid(expression).name() << std::endl;
    return expression->codeGen(context);
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
    if(returns.size() > 1)
    {
        return codeGenMultiReturn(context);
    }
    else
    {
        return codeGenSingleReturn(context);
    }
}

llvm::Value *NClassDeclaration::codeGen(CodeGenContext &context)
{
    return NULL;
}

llvm::Value *NFunctionDeclaration::codeGenSingleReturn(CodeGenContext &context)
{
    vector<llvm::Type*> argTypes;

    // Add actual args
    VariableList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        argTypes.push_back(typeOf((**it).type));
    }

    // Return Type
    llvm::Type *returnType = llvm::Type::getVoidTy(llvm::getGlobalContext());
    if(returns.size() > 0)
    {
        returnType = typeOf(returns[0]);
    }

    llvm::FunctionType *ftype = llvm::FunctionType::get(returnType, llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, id->name.c_str(), context.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0);

    context.pushBlock(bblock);
    context.setCurrentFunction(function);

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;

    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        (**it).codeGen(context);

        argumentValue = argsValues++;
        argumentValue->setName((*it)->id->name.c_str());
        llvm::StoreInst *inst = new llvm::StoreInst(argumentValue, context.locals()[(*it)->id->name].value, false, bblock);
    }

    block->codeGen(context);

    // Cleanup strings
    map<string, CodeGenLocal>::iterator localIt;
    llvm::Function *stringDestroyFunc = context.module->getFunction("kuma_string_destroy");

    for(localIt = context.locals().begin(); localIt != context.locals().end(); localIt++)
    {
        if(localIt->second.type == KUMA_TYPE_STRING)
        {
            context.currentBuilder()->CreateCall(stringDestroyFunc, localIt->second.value);
        }
    }

    // Return
    llvm::ReturnInst::Create(llvm::getGlobalContext(), context.getCurrentReturnValue(), bblock);

    context.popBlock();
    context.addFunction(id->name, function, returns.size(), false);

    std::cout << "Creating function: " << id->name << std::endl;
    return function;
}

llvm::Value *NFunctionDeclaration::codeGenMultiReturn(CodeGenContext &context)
{
    vector<llvm::Type*> argTypes;

    // Create return struct
    vector<llvm::Type*> returnTypes;
    IdentList::const_iterator ident;
    for (ident = returns.begin(); ident != returns.end(); ident++)
    {
        returnTypes.push_back(typeOf((*ident)));
    }

    string structTypeName = id->name + "_return_struct";
    llvm::StructType *rstruct = llvm::StructType::create(llvm::getGlobalContext(), llvm::ArrayRef<llvm::Type*>(returnTypes), structTypeName.c_str(), false); // isPacked
    llvm::PointerType *structPtr = llvm::PointerType::get(rstruct, 0);
    argTypes.push_back(structPtr);

    // Add actual args
    VariableList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        argTypes.push_back(typeOf((**it).type));
    }

    llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), llvm::makeArrayRef(argTypes), false);
    llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, id->name.c_str(), context.module);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0);

    context.addFunction(id->name, function, returns.size(), false);
    context.pushBlock(bblock);
    context.setCurrentFunction(function);

    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value* argumentValue;

    // First the return pointer
    argsValues++;

    for (it = arguments.begin(); it != arguments.end(); it++)
    {
        (**it).codeGen(context);

        argumentValue = argsValues++;
        argumentValue->setName((*it)->id->name.c_str());
        llvm::StoreInst *inst = new llvm::StoreInst(argumentValue, context.locals()[(*it)->id->name].value, false, bblock);
    }

    block->codeGen(context);

    // Cleanup strings
    map<string, CodeGenLocal>::iterator localIt;
    llvm::Function *stringDestroyFunc = context.module->getFunction("kuma_string_destroy");

    for(localIt = context.locals().begin(); localIt != context.locals().end(); localIt++)
    {
        if(localIt->second.type == KUMA_TYPE_STRING)
        {
            context.currentBuilder()->CreateCall(stringDestroyFunc, localIt->second.value);
        }
    }

    // Return
    llvm::ReturnInst::Create(llvm::getGlobalContext(), NULL, bblock);

    context.popBlock();

    std::cout << "Creating function: " << id->name << std::endl;
    return function;
}

llvm::Value* NReturnStatement::codeGen(CodeGenContext& context)
{
    llvm::Function *function = context.getCurrentFunction();
    std::string name = function->getName().str();

    if(context.functions()[name].numReturns > 1)
    {
        return codeGenMultiReturn(context);
    }
    else
    {
        return codeGenSingleReturn(context);
    }
}

llvm::Value *NReturnStatement::codeGenMultiReturn(CodeGenContext &context)
{
    llvm::Function *function = context.getCurrentFunction();
    std::cout << "Generating code for " << function->getName().str() << " return statement." << std::endl;
    llvm::Function::arg_iterator argsValues = function->arg_begin();
    llvm::Value *returnVal = argsValues;

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

    //llvm::LoadInst *loadInst = new llvm::LoadInst(returnVal, "", context.currentBlock());
    //context.setCurrentReturnValue(loadInst);

    return NULL;
}

llvm::Value *NReturnStatement::codeGenSingleReturn(CodeGenContext &context)
{
    llvm::Function *function = context.getCurrentFunction();
    std::cout << "Generating code for " << function->getName().str() << " return statement." << std::endl;
    llvm::Value *returnVal = NULL;
    if(expressions.size() > 0)
    {
        returnVal = expressions[0]->codeGen(context);
    }

    context.setCurrentReturnValue(returnVal);

    return returnVal;
}

