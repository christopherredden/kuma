//
// Created by Christopher Redden on 28/07/15.
//

#include <string>
#include <vector>
#include "codegen.h"

#ifndef KUMA_NODES_H
#define KUMA_NODES_H

class NStatement;
class NExpression;
class NVariableDeclaration;
class NIdentifier;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;
typedef std::vector<NIdentifier*> IdentList;

class Node {
public:
    virtual ~Node() {}
    virtual llvm::Value* codeGen(CodeGenContext& context) { }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name){ this->name = name; }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
            id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
            lhs(lhs), rhs(rhs), op(op) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
public:
    IdentList lhs;
    NExpression& rhs;
    NAssignment(const IdentList& lhs, NExpression& rhs) :
            lhs(lhs), rhs(rhs) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignmentMethodCall : public NExpression {
public:
    IdentList lhs;
    NMethodCall& rhs;
    NAssignmentMethodCall(const IdentList& lhs, NMethodCall& rhs) :
            lhs(lhs), rhs(rhs) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
            expression(expression) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReturnStatement : public NStatement {
public:
    ExpressionList expressions;
    NReturnStatement(const ExpressionList& expressions) :
            expressions(expressions) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
    const NIdentifier& type;
    NIdentifier& id;
    NExpression *assignmentExpr;
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
            type(type), id(id), assignmentExpr(NULL) { }
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
            type(type), id(id), assignmentExpr(assignmentExpr) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExternDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList arguments;
    NExternDeclaration(const NIdentifier& type, const NIdentifier& id,
                       const VariableList& arguments) :
            type(type), id(id), arguments(arguments) {}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier& id;
    VariableList arguments;
    IdentList returns;
    NBlock& block;
    NFunctionDeclaration(const NIdentifier& id, const VariableList& arguments, const IdentList& returns, NBlock& block) :
            id(id), arguments(arguments), returns(returns), block(block) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

#endif //KUMA_NODES_H
