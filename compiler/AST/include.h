#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <stack>
#include <fstream>
#include <iostream>
// #include "../Utils/llvm.h"
#include "../Token/include.h"
#include "../Lexer/include.h"
#include "../Type/include.h"

class AST;
class ASTNode;
class ASTExpression;
class ASTBinaryExpression;
class ASTUnaryExpression;
class ASTBlock;
class ASTProgramBlock;
class ASTCallExpression;
class ASTExpression;
class ASTStringExpression;
class ASTCharExpression;
class ASTIntExpression;
class ASTFloatExpression;
class ASTIdentifierExpression;
class ASTFucntion;
class ASTPrototype;
class ASTStatement;
class ASTReturnStatement;
class ASTType;
class BlockStack;
class ASTVariableExpression;
class ASTAssignVariableExpression;
class ASTMutateVariableExpression;
class ASTForExpression;
class ASTIfExpression;
class ASTWhileExpression;
class ASTDoWhileExpression;
class ASTElseExpression;
class ASTElseIfExpression;

class ASTNode
{
public:
  enum ASTNodeID
  {
    ASTNodeID,
    ASTExpressionID,
    ASTBinaryExpressionID,
    ASTUnaryExpressionID,
    ASTBlockID,
    ASTProgramBlockID,
    ASTCallExpressionID,
    ASTExpressionID,
    ASTStringExpressionID,
    ASTCharExpressionID,
    ASTIntExpressionID,
    ASTFloatExpressionID,
    ASTIdentifierExpressionID,
    ASTFucntionID,
    ASTPrototypeID,
    ASTStatementID,
    ASTReturnStatementID,
    ASTTypeID,
    BlockStackID,
    ASTVariableExpressionID,
    ASTAssignVariableExpressionID,
    ASTMutateVariableExpressionID,
    ASTForExpressionID,
    ASTIfExpressionID,
    ASTWhileExpressionID,
    ASTDoWhileExpressionID,
    ASTElseExpressionID,
    ASTElseIfExpressionID,
  };

  std::string show_kind;
  std::string show_value;

protected:
  enum ASTNodeID ID;

public:
  ASTNode(enum ASTNodeID ID, std::string show_kind = "", std::string show_value = "") : ID(ID), show_kind(show_kind), show_value(show_value){};
  virtual ~ASTNode() = default;
  virtual std::vector<ASTNode *> getChildrenShow() = 0;
  virtual llvm::Value *codegen() = 0;
  enum ASTNodeID getASTNodeID() { return ID; }
};

class ASTBlock : public ASTNode
{
protected:
  std::vector<std::unique_ptr<ASTNode>> body;
  // std::map<std::string, ASTVariableExpression *> named_variables;

public:
  ASTBlock(std::vector<std::unique_ptr<ASTNode>> body, std::string show_kind) : body(std::move(body)), ASTNode(ASTNodeID::ASTBlockID, show_kind){};
  ASTBlock(std::vector<std::unique_ptr<ASTNode>> body) : body(std::move(body)), ASTNode(ASTNodeID::ASTBlockID, "Block"){};

  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    for (int i = 0; i < body.size(); i++)
    {
      children.push_back(&(*body[i]));
    }

    return children;
  }

  void push_back(std::unique_ptr<ASTNode> node)
  {
    if (node)
    {
      body.push_back(std::move(node));
    }
  }

  virtual llvm::Value *codegen() override
  {
    // global_block_stack->push_block(this);

    llvm::Value *FnIR;
    for (int i = 0; i < body.size(); i++)
    {
      FnIR = body[i]->codegen();
    }

    // global_block_stack->pop_block();

    return FnIR;
  }
};

/*
class BlockStack
{
private:
  std::vector<ASTBlock *> stack;

public:
  ASTBlock *get_current_block()
  {
    return stack.back();
  }

  void push_block(ASTBlock *block)
  {
    stack.push_back(block);
  }

  void pop_block()
  {
    stack.pop_back();
  }

  ASTVariableExpression *named_variable(std::string name)
  {
    for (int i = stack.size() - 1; i >= 0; i--)
    {
      ASTVariableExpression *named_variable = stack[i]->named_variable(name);
      if (named_variable)
      {
        return named_variable;
      }
    }

    return nullptr;
  }
};

class Statement : public ASTNode
{
public:
  virtual ~Statement() = 0;
};
*/

class ASTExpression : public ASTNode
{
protected:
  Type *type;

public:
  ASTExpression(enum ASTNodeID ID, std::string show_kind, std::string show_value = "") : ASTNode(ID, show_kind, show_value){};
  ASTExpression() : ASTNode(ASTNodeID::ASTExpressionID){};

  Type *getType() { return type; };
  virtual void setType(Type *type) { this->type = type; }
};

/*
class ASTCallExpression : public ASTExpression
{
private:
  Token name;
  std::vector<std::unique_ptr<ASTExpression>> args;

public:
  ASTCallExpression(Token name, std::vector<std::unique_ptr<ASTExpression>> args) : ASTNode(ASTNodeID::ASTCallExpressionID, "CallExpression", name.value){};

  Type *getType()
  {
    return global_functions[name.value]->getType();
  }

  llvm::Value *codegen()
  {
    llvm::Function *function = module->getFunction(name.value);
    if (!function)
    {
      return nullptr;
    }

    if (this->args.size() != function->arg_size())
    {
      return nullptr;
    }

    std::vector<llvm::Value *> args;
    for (int i = 0; i < this->args.size(); i++)
    {
      llvm::Value *codegen = this->args[i]->codegen();
      if (codegen)
      {
        args.push_back(codegen);
      }
    }

    if (args.size() != function->arg_size())
    {
      return nullptr;
    }

    return builder->CreateCall(function, args, "call_" + function->getName() + "_temp");
  }

  std::vector<ASTNode *> getChildrenShow()
  {
    std::vector<ASTNode *> children;
    for (int i = 0; i < args.size(); i++)
    {
      children.push_back(&(*args[i]));
    }

    return children;
  }
};
*/

class ASTBinaryExpression : public ASTExpression
{
private:
  Token operator_token;
  std::unique_ptr<ASTExpression> left_operand;
  std::unique_ptr<ASTExpression> right_operand;

public:
  ASTBinaryExpression(Token operator_token,
                      std::unique_ptr<ASTExpression> left_operand,
                      std::unique_ptr<ASTExpression> right_operand) : ASTExpression(ASTNodeID::ASTBinaryExpressionID, "BinaryExpression", operator_token.value),
                                                                      operator_token(operator_token),
                                                                      left_operand(std::move(left_operand)),
                                                                      right_operand(std::move(right_operand)){};

  llvm::Value *codegen() override
  {
    llvm::Value *left_value = left_operand->codegen();
    llvm::Value *right_value = right_operand->codegen();
    Type *left_operand_type = left_operand->getType();
    Type *right_operand_type = right_operand->getType();

    if (!left_value || !right_value)
    {
      return nullptr;
    }

    if (left_operand_type->isFloatTy() || right_operand_type->isFloatTy())
    {
      switch (operator_token.type)
      {
      case Token::Type::PLUS:
        return builder->CreateFAdd(left_value, right_value, "add_tmp");
      case Token::Type::HYPHEN:
        return builder->CreateFSub(left_value, right_value, "sub_tmp");
      case Token::Type::ASTERISK:
        return builder->CreateFMul(left_value, right_value, "mul_tmp");
      case Token::Type::BACKSLASH:
        return builder->CreateFDiv(left_value, right_value, "div_tmp");
      case Token::Type::PERCENT:
        return builder->CreateFRem(left_value, right_value, "rem_tmp");
      case Token::Type::DOUBLE_AMPERSAND:
        return builder->CreateLogicalAnd(left_value, right_value, "and_tmp");
      case Token::Type::DOUBLE_VBAR:
        return builder->CreateLogicalOr(left_value, right_value, "or_tmp");
      case Token::Type::DOUBLE_EQUALS:
        return builder->CreateFCmpOEQ(left_value, right_value, "equal_to_tmp");
      case Token::Type::EXCLAMATION_EQUALS:
        return builder->CreateFCmpONE(left_value, right_value, "not_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET:
        return builder->CreateFCmpOGT(left_value, right_value, "greater_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET:
        return builder->CreateFCmpOLT(left_value, right_value, "lower_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateFCmpOLE(left_value, right_value, "lower_than_or_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateFCmpOGE(left_value, right_value, "greater_than_or_equal_to_tmp");
      default:
        return nullptr;
      }
    }
    else
    {
      switch (operator_token.type)
      {
      case Token::Type::PLUS:
        return builder->CreateAdd(left_value, right_value, "add_tmp");
      case Token::Type::HYPHEN:
        return builder->CreateSub(left_value, right_value, "sub_tmp");
      case Token::Type::ASTERISK:
        return builder->CreateMul(left_value, right_value, "mul_tmp");
      case Token::Type::BACKSLASH:
        left_value = builder->CreateSIToFP(left_value, llvm::Type::getFloatTy(*context), "sint_to_float_tmp");
        right_value = builder->CreateSIToFP(right_value, llvm::Type::getFloatTy(*context), "sint_to_float_tmp");
        return builder->CreateFDiv(left_value, right_value, "div_tmp");
      case Token::Type::PERCENT:
        return builder->CreateSRem(left_value, right_value, "rem_tmp");
      case Token::Type::DOUBLE_AMPERSAND:
        return builder->CreateLogicalAnd(left_value, right_value, "and_tmp");
      case Token::Type::DOUBLE_VBAR:
        return builder->CreateLogicalOr(left_value, right_value, "or_tmp");
      case Token::Type::DOUBLE_EQUALS:
        return builder->CreateICmpEQ(left_value, right_value, "equal_to_tmp");
      case Token::Type::EXCLAMATION_EQUALS:
        return builder->CreateICmpNE(left_value, right_value, "not_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET:
        return builder->CreateICmpSGT(left_value, right_value, "greater_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET:
        return builder->CreateICmpSLT(left_value, right_value, "lower_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateICmpSLE(left_value, right_value, "lower_than_or_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateICmpSGE(left_value, right_value, "greater_than_or_equal_to_tmp");
      default:
        return nullptr;
      }
    }
  }

  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    children.push_back(&(*left_operand));
    children.push_back(&(*right_operand));
    return children;
  }
};

/*
class ASTUnaryExpression : public ASTExpression
{
private:
  Token operator_token;
  std::unique_ptr<ASTExpression> operand;

public:
  ASTUnaryExpression(Token operator_token, std::unique_ptr<ASTExpression> operand);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTStringExpression : public ASTExpression
{
private:
  Token token;

public:
  ASTStringExpression(Token token);
  llvm::Value *codegen() override;
};

class ASTCharExpression : public ASTExpression
{
private:
  Token token;

public:
  ASTCharExpression(Token token);
  llvm::Value *codegen() override;
};

class ASTIdentifierExpression : public ASTExpression
{
private:
  Token token;

public:
  ASTIdentifierExpression(Token token);
  llvm::Value *codegen() override;
};
*/

class ASTIntExpression : public ASTExpression
{
private:
  Token token;

public:
  ASTIntExpression(Token token) : ASTExpression(ASTNodeID::ASTIntExpressionID, "IntExpression", token.value), token(token)
  {
    setType(Type::getInteger32Ty());
  }
};

class ASTFloatExpression : public ASTExpression
{
private:
  Token token;

public:
  ASTFloatExpression(Token token) : ASTExpression(ASTNodeID::ASTFloatExpressionID, "FloatExpression", token.value), token(token)
  {
    setType(Type::getFloat32Ty());
  }
};

/*
class ASTFunction : public ASTNode
{
private:
  std::unique_ptr<ASTPrototype> prototype;
  std::unique_ptr<ASTBlock> block;

public:
  ASTFunction(std::unique_ptr<ASTPrototype> prototype, std::unique_ptr<ASTBlock> block);
  std::vector<ASTNode *> getChildrenShow() override;
  virtual llvm::Function *codegen() override;
  std::string get_name();
};

class ASTPrototype : public ASTNode
{
protected:
  Token name;
  std::unique_ptr<ASTType> return_type;
  std::vector<std::unique_ptr<ASTVariableExpression>> args;

public:
  ASTPrototype() = default;
  ASTPrototype(Token name, std::vector<std::unique_ptr<ASTVariableExpression>> args, std::unique_ptr<ASTType> return_type);
  std::vector<ASTNode *> getChildrenShow() override;
  virtual llvm::Function *codegen() override;
  std::string get_name();
  llvm::Type *get_variable_llvm_type_at(int i);
  ASTVariableExpression *get_variable_at(int i);
};

class ASTReturnStatement : public ASTStatement
{
private:
  std::unique_ptr<ASTExpression> expression;

public:
  ASTReturnStatement(std::unique_ptr<ASTExpression> expression);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTVariableExpression : public ASTExpression
{
protected:
  Token name;
  std::unique_ptr<ASTType> type;
  llvm::AllocaInst *value;

public:
  ASTVariableExpression() = default;
  ASTVariableExpression(Token name, std::unique_ptr<ASTType> type);
  virtual std::vector<ASTNode *> getChildrenShow() override;
  virtual llvm::Value *codegen() override;
  std::string get_name();
  virtual llvm::AllocaInst *get_value();
  virtual Type getType() override;
  void set_value(llvm::AllocaInst *value);
  llvm::Type *get_llvm_type() override;
};

class ASTAssignVariableExpression : public ASTVariableExpression
{
private:
  std::unique_ptr<ASTExpression> expression;

public:
  ASTAssignVariableExpression(Token name, std::unique_ptr<ASTType> type, std::unique_ptr<ASTExpression> expression);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTMutateVariableExpression : public ASTVariableExpression
{
private:
  std::unique_ptr<ASTExpression> expression;

public:
  ASTMutateVariableExpression(Token name, std::unique_ptr<ASTExpression> expression);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTForExpression : public ASTExpression
{
private:
public:
};

class ASTIfExpression : public ASTExpression
{
private:
  std::unique_ptr<ASTExpression> condition;
  std::unique_ptr<ASTBlock> block;
  std::unique_ptr<ASTElseExpression> else_expression;
  std::vector<std::unique_ptr<ASTElseIfExpression>> else_if_expressions;

public:
  ASTIfExpression(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTBlock> block, std::vector<std::unique_ptr<ASTElseIfExpression>> else_if_expressions, std::unique_ptr<ASTElseExpression> else_expression);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTWhileExpression : public ASTExpression
{
private:
  std::unique_ptr<ASTExpression> condition;
  std::unique_ptr<ASTBlock> block;
  std::unique_ptr<ASTElseExpression> else_expression;

public:
  ASTWhileExpression(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTBlock> block, std::unique_ptr<ASTElseExpression> else_expression);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTDoWhileExpression : public ASTExpression
{
private:
  std::unique_ptr<ASTExpression> condition;
  std::unique_ptr<ASTBlock> block;

public:
  ASTDoWhileExpression(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTBlock> block);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTElseExpression : public ASTExpression
{
private:
  std::unique_ptr<ASTBlock> block;

public:
  ASTElseExpression(std::unique_ptr<ASTBlock> block);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::Value *codegen() override;
};

class ASTElseIfExpression : public ASTExpression
{
private:
  std::unique_ptr<ASTExpression> condition;
  std::unique_ptr<ASTBlock> block;

public:
  ASTElseIfExpression(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTBlock> block);
  std::vector<ASTNode *> getChildrenShow() override;
  llvm::BasicBlock *codegen(llvm::BasicBlock *MergeBB, llvm::BasicBlock *ElseBB = nullptr);
};

ASTVariableExpression *current_variable;
std::unique_ptr<AST> global_tree(new AST());
std::unique_ptr<BlockStack> global_block_stack(new BlockStack());
std::map<std::string, ASTFunction *> global_functions;

class AST
{
private:
  std::unique_ptr<ASTBlock> root;

public:
  AST::AST()
  {
    root.reset(new ASTBlock());
  }

  void AST::push_back(std::unique_ptr<ASTNode> node)
  {
    root->push_back(std::move(node));
  }

  void AST::show()
  {
    if (root != nullptr)
    {
      pretty_print(&(*root));
    }
    else
    {
      printf("No Tree to Show\n");
    }
  }
};
*/
