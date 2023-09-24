#include <iostream>
#include "./Settings/include.h"
#include "./Type/include.h"
#include "./Token/include.h"
// #include "./AST/include.h"

void TestTypes(std::vector<Type *> types)
{
  std::string typesManglingNames[types.size()];

  printf("\n------------------ IS IT SAME TYPES? ------------------\n");
  for (int i = 0; i < types.size(); i++)
  {
    Type *type1 = types[i];
    if (typesManglingNames[i].empty())
    {
      typesManglingNames[i] = type1->getManglingName();
    }

    for (int j = 0; j < types.size(); j++)
    {
      if (i == j)
      {
        continue;
      }

      Type *type2 = types[j];
      if (typesManglingNames[j].empty())
      {
        typesManglingNames[j] = type2->getManglingName();
      }

      std::cout << typesManglingNames[i] + " == " + typesManglingNames[j] + " is " << (type1->isEquals(type2) ? "true" : "false") << std::endl;
    }
  }

  printf("\n------------------ TYPES MANGLING NAMES ------------------\n");
  for (int i = 0; i < types.size(); i++)
  {
    std::cout << typesManglingNames[i] << std::endl;
  }
}

Type *getTypeFromTwoTypes(Type *type1, Type *type2)
{
  if (type1->isEquals(type2))
  {
    return type1->copy();
  }
  else if (type1->isNumberTy() && type2->isNumberTy())
  {
    if (type1->getSubclassData() > type2->getSubclassData())
    {
      if (type2->isFloatTy() && !type1->isFloatTy())
      {
        return type2->copy();
      }

      return type1->copy();
    }
    else if (type1->isFloatTy() && !type2->isFloatTy())
    {
      return type1->copy();
    }
    else
    {
      return type2->copy();
    }
  }
  else
  {
    return Type::getVoidTy();
  }
}

class ASTNode
{
public:
  enum ASTNodeID
  {
    ASTExpressionID,
    ASTBinaryExpressionID,
    ASTUnaryExpressionID,
    ASTBlockID,
    ASTCallExpressionID,
    ASTStringExpressionID,
    ASTCharExpressionID,
    ASTIntNumberExpressionID,
    ASTFloatNumberExpressionID,
    ASTIdentifierExpressionID,
    ASTFucntionID,
    ASTPrototypeID,
    ASTReturnStatementID,
    ASTVariableStatementID,
    ASTAssignVariableStatementID,
    ASTMutateVariableStatementID,
    ASTForStatementID,
    ASTIfStatementID,
    ASTWhileStatementID,
    ASTDoWhileStatementID,
    ASTElseStatementID,
    ASTElseIfStatementID,
  };

  std::string showKind;
  std::string showValue;

protected:
  ASTNodeID ID;

public:
  ASTNode(ASTNodeID ID, std::string showKind = "", std::string showValue = "") : ID(ID), showKind(showKind), showValue(showValue){};
  ASTNode() = default;
  virtual ~ASTNode() = default;

  virtual std::vector<ASTNode *> getChildrenShow() { return std::vector<ASTNode *>(); };
  virtual llvm::Value *codegen() = 0;
  ASTNodeID getASTNodeID() { return ID; }
};

class ASTStatement : public ASTNode
{
public:
  ASTStatement(ASTNodeID ID, std::string showKind = "", std::string showValue = "") : ASTNode(ID, showKind, showValue){};
};

class ASTExpression : public ASTStatement
{
protected:
  Type *type;

  explicit ASTExpression(ASTNodeID ID, std::string showKind = "", std::string showValue = "") : ASTStatement(ID, showKind, showValue){};
  explicit ASTExpression(Type *type, ASTNodeID ID, std::string showKind = "", std::string showValue = "") : type(type), ASTStatement(ID, showKind, showValue){};

public:
  Type *getType() { return type; }
  virtual void setType(Type *type) { this->type = type; }
  virtual void evaluateType(){};
};

class ASTNumberExpression : public ASTExpression
{
protected:
  Token token;
  explicit ASTNumberExpression(Token token, Type *type, ASTNodeID ID, std::string showKind) : token(token), ASTExpression(type, ID, showKind, token.value){};

public:
  void setType(Type *type) override
  {
    if (!(type->isNumberTy()))
    {
      return;
    }

    this->type = type;
  }
};

class ASIntTNumberExpression : public ASTNumberExpression
{
public:
  ASIntTNumberExpression(Token token) : ASTNumberExpression(token, Type::getInteger32Ty(), ASTNode::ASTIntNumberExpressionID, "IntNumberExpression"){};
  llvm::Value *codegen() override
  {
    return llvm::ConstantInt::get(getType()->getLLVMTy(), std::stol(token.value), true);
  }
};

class ASFloatTNumberExpression : public ASTNumberExpression
{
public:
  ASFloatTNumberExpression(Token token) : ASTNumberExpression(token, Type::getFloat32Ty(), ASTNode::ASTFloatNumberExpressionID, "FloatNumberExpression"){};
  llvm::Value *codegen() override
  {
    return llvm::ConstantFP::get(getType()->getLLVMTy(), std::stof(token.value));
  }
};

class ASTBinaryExpression : public ASTExpression
{
protected:
  Token operatorToken;
  ASTExpression *leftOperand;
  ASTExpression *rightOperand;

public:
  ASTBinaryExpression(Token operatorToken,
                      ASTExpression *leftOperand,
                      ASTExpression *rightOperand) : operatorToken(operatorToken),
                                                     leftOperand(leftOperand),
                                                     rightOperand(rightOperand),
                                                     ASTExpression(ASTNodeID::ASTBinaryExpressionID, "BinaryExpression", operatorToken.value){};
  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    children.push_back(leftOperand);
    children.push_back(rightOperand);
    return std::move(children);
  }

  void evaluateType() override
  {
    leftOperand->evaluateType();
    rightOperand->evaluateType();

    if (leftOperand->getType()->isIntegerTy() && rightOperand->getType()->isIntegerTy() && operatorToken.type == Token::Type::BACKSLASH)
    {
      setType(Type::getFloat64Ty());
    }
    else
    {
      setType(getTypeFromTwoTypes(leftOperand->getType(), rightOperand->getType()));
    }
  }

  void setType(Type *type) override
  {
    leftOperand->setType(type);
    rightOperand->setType(type);
    this->type = type;
  }

  llvm::Value *codegen() override
  {
    Type *leftOperandType = leftOperand->getType();
    Type *rightOperandType = rightOperand->getType();

    llvm::Value *leftValue = leftOperand->codegen();
    llvm::Value *rightValue = rightOperand->codegen();

    if (!leftValue || !rightValue)
    {
      return nullptr;
    }

    if (leftOperandType->isFloatTy() && rightOperandType->isFloatTy())
    {
      switch (operatorToken.type)
      {
      case Token::Type::PLUS:
        return builder->CreateFAdd(leftValue, rightValue, "add_tmp");
      case Token::Type::HYPHEN:
        return builder->CreateFSub(leftValue, rightValue, "sub_tmp");
      case Token::Type::ASTERISK:
        return builder->CreateFMul(leftValue, rightValue, "mul_tmp");
      case Token::Type::BACKSLASH:
        return builder->CreateFDiv(leftValue, rightValue, "div_tmp");
      case Token::Type::PERCENT:
        return builder->CreateFRem(leftValue, rightValue, "rem_tmp");
      case Token::Type::DOUBLE_AMPERSAND:
        return builder->CreateLogicalAnd(leftValue, rightValue, "and_tmp");
      case Token::Type::DOUBLE_VBAR:
        return builder->CreateLogicalOr(leftValue, rightValue, "or_tmp");
      case Token::Type::DOUBLE_EQUALS:
        return builder->CreateFCmpOEQ(leftValue, rightValue, "equal_to_tmp");
      case Token::Type::EXCLAMATION_EQUALS:
        return builder->CreateFCmpONE(leftValue, rightValue, "not_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET:
        return builder->CreateFCmpOGT(leftValue, rightValue, "greater_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET:
        return builder->CreateFCmpOLT(leftValue, rightValue, "lower_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateFCmpOLE(leftValue, rightValue, "lower_than_or_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateFCmpOGE(leftValue, rightValue, "greater_than_or_equal_to_tmp");
      default:
        return nullptr;
      }
    }
    else if (leftOperandType->isIntegerTy() && rightOperandType->isIntegerTy())
    {
      switch (operatorToken.type)
      {
      case Token::Type::PLUS:
        return builder->CreateAdd(leftValue, rightValue, "add_tmp");
      case Token::Type::HYPHEN:
        return builder->CreateSub(leftValue, rightValue, "sub_tmp");
      case Token::Type::ASTERISK:
        return builder->CreateMul(leftValue, rightValue, "mul_tmp");
      case Token::Type::PERCENT:
        return builder->CreateSRem(leftValue, rightValue, "rem_tmp");
      case Token::Type::DOUBLE_AMPERSAND:
        return builder->CreateLogicalAnd(leftValue, rightValue, "and_tmp");
      case Token::Type::DOUBLE_VBAR:
        return builder->CreateLogicalOr(leftValue, rightValue, "or_tmp");
      case Token::Type::DOUBLE_EQUALS:
        return builder->CreateICmpEQ(leftValue, rightValue, "equal_to_tmp");
      case Token::Type::EXCLAMATION_EQUALS:
        return builder->CreateICmpNE(leftValue, rightValue, "not_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET:
        return builder->CreateICmpSGT(leftValue, rightValue, "greater_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET:
        return builder->CreateICmpSLT(leftValue, rightValue, "lower_than_tmp");
      case Token::Type::LEFT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateICmpSLE(leftValue, rightValue, "lower_than_or_equal_to_tmp");
      case Token::Type::RIGHT_ANGULAR_BRACKET_EQUALS:
        return builder->CreateICmpSGE(leftValue, rightValue, "greater_than_or_equal_to_tmp");
      default:
        return nullptr;
      }
    }

    return nullptr;
  }
};

class ASTUnaryExpression : public ASTExpression
{
private:
  Token operatorToken;
  ASTExpression *operand;

public:
  ASTUnaryExpression(Token operatorToken,
                     ASTExpression *operand) : operatorToken(operatorToken),
                                               operand(operand),
                                               ASTExpression(ASTNode::ASTUnaryExpressionID, "UnaryExpression", operatorToken.value){};
  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    children.push_back(operand);
    return std::move(children);
  }

  void setType(Type *type) override
  {
    operand->setType(type);
    this->type = type;
  }

  void evaluateType() override
  {
    operand->evaluateType();
    setType(operand->getType());
  }

  llvm::Value *codegen() override
  {
    Type *operandType = operand->getType();
    llvm::Value *operandValue = operand->codegen();

    if (!operandValue)
    {
      return nullptr;
    }

    if (getType()->isNumberTy())
    {
      switch (operatorToken.type)
      {
      case Token::Type::PLUS:
        return operandValue;
      case Token::Type::HYPHEN:
        return builder->CreateNeg(operandValue, "negative_tmp");
      case Token::Type::EXCLAMATION:
        return builder->CreateNot(operandValue, "not_tmp");
      default:
        return nullptr;
      }
    }

    return nullptr;
  }
};

class ASTVariableStatement : public ASTStatement
{
protected:
  Token token;
  Type *type;
  llvm::AllocaInst *value;

public:
  ASTVariableStatement(Token token,
                       Type *type,
                       ASTNodeID ID = ASTNode::ASTVariableStatementID,
                       std::string showKind = "VariableStatement") : token(token),
                                                                     type(type),
                                                                     ASTStatement(ID, showKind, token.value){};

  std::string getName() { return token.value; }
  Type *getType() { return type; }
  llvm::AllocaInst *getAlocatedValue() { return value; }
  virtual void evaluateType(){};

  virtual llvm::Value *codegen() override
  {
    llvm::Function *function = builder->GetInsertBlock()->getParent();
    value = CreateEntryBlockAlloca(function, type->getLLVMTy(), token.value.c_str());
    return value;
  }
};

ASTVariableStatement *currentVariable;
class ASTBlock : public ASTNode
{
protected:
  std::vector<ASTStatement *> body;
  std::map<std::string, ASTVariableStatement *> namedVariables;

public:
  ASTBlock(std::vector<ASTStatement *> body, std::string showKind = "Block") : body(std::move(body)), ASTNode(ASTNode::ASTBlockID, showKind){};
  ASTBlock(std::string showKind = "Block") : ASTNode(ASTNode::ASTBlockID, showKind){};

  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    // printf("------ DEBUGING ------");
    for (int i = 0; i < body.size(); i++)
    {
      children.push_back(body[i]);
    }

    return std::move(children);
  }

  llvm::Value *codegen() override;
  void newNamedVariable(ASTVariableStatement *variable)
  {
    namedVariables[variable->getName()] = variable;
    currentVariable = variable;
  }

  ASTVariableStatement *namedVariable(std::string variableName)
  {
    currentVariable = namedVariables[variableName];
    return currentVariable;
  }

  void pushStatement(ASTStatement *statement)
  {
    body.push_back(statement);
  }
};

class BlockStack
{
protected:
  std::vector<ASTBlock *> blocks;

public:
  BlockStack() = default;
  ASTBlock *getCurrentBlock() { return blocks.back(); }
  void pushBlock(ASTBlock *block) { blocks.push_back(block); }
  void popBlock() { blocks.pop_back(); }

  ASTVariableStatement *namedVariable(std::string variableName)
  {
    for (int i = blocks.size() - 1; i >= 0; i--)
    {
      ASTVariableStatement *variable = blocks[i]->namedVariable(variableName);
      if (variable)
      {
        return variable;
      }
    }

    return nullptr;
  }
};

BlockStack *globalBlockStack(new BlockStack());
llvm::Value *ASTBlock::codegen()
{
  globalBlockStack->pushBlock(this);

  llvm::Value *FnIR;
  for (int i = 0; i < body.size(); i++)
  {
    FnIR = body[i]->codegen();
  }

  globalBlockStack->popBlock();

  return FnIR;
}

class ASTAssignVariableStatement : public ASTVariableStatement
{
protected:
  ASTExpression *expression;

public:
  ASTAssignVariableStatement(ASTExpression *expression,
                             Token token,
                             Type *type,
                             ASTNodeID ID = ASTNode::ASTAssignVariableStatementID,
                             std::string showKind = "AssignVariableStatement") : expression(expression),
                                                                                 ASTVariableStatement(token, type, ID, showKind)
  {
    globalBlockStack->getCurrentBlock()->newNamedVariable(this);
  };

  virtual void evaluateType() override
  {
    if (!getType()->isEquals(expression->getType()))
    {
      expression->setType(getType());
    }
  }

  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    children.push_back(expression);
    return std::move(children);
  }

  virtual llvm::StoreInst *codegen() override
  {
    llvm::Value *expressionValue = expression->codegen();

    if (!expressionValue)
    {
      return nullptr;
    }

    llvm::Function *function = builder->GetInsertBlock()->getParent();
    value = CreateEntryBlockAlloca(function, type->getLLVMTy(), token.value);
    return builder->CreateStore(expressionValue, value);
  }
};

class ASTIdentifierExpression : public ASTExpression
{
protected:
  Token token;
  ASTVariableStatement *foundVariable;

public:
  ASTIdentifierExpression(Token token) : token(token), ASTExpression(ASTNode::ASTIdentifierExpressionID, "IdentifierExpression", token.value)
  {
    foundVariable = globalBlockStack->namedVariable(token.value);
    if (foundVariable)
    {
      setType(foundVariable->getType());
    }
  };

  llvm::LoadInst *codegen() override
  {
    if (!foundVariable)
    {
      return nullptr;
    }

    llvm::AllocaInst *value = foundVariable->getAlocatedValue();
    if (!value)
    {
      return nullptr;
    }

    return builder->CreateLoad(value->getAllocatedType(), value, token.value.c_str());
  }
};

class ASTCallExpression : public ASTExpression
{
};

void PrettyPrint(ASTNode *node, std::string indent = "", bool is_last = true)
{
  if (node == nullptr)
  {
    return;
  }

  // └── : ├── : │
  std::string token_marker = is_last ? "|- " : "+ ";
  // std::string token_marker = is_last ? "└──" : "├──";

  std::cout << "\033[0;91;1m" << indent << token_marker << node->showKind << "\033[0;92;1m " << node->showValue << "\033[0m" << std::endl;
  std::string last_indent = indent;
  indent += is_last ? "   " : "|  ";
  // indent += is_last ? "   " : "│  ";

  std::vector<ASTNode *> children = node->getChildrenShow();
  for (int i = 0; i < children.size(); i++)
  {
    PrettyPrint(children[i], indent, i == children.size() - 1);
  }
}

int main()
{
  // std::vector<Type *> testTys;
  // Type *type1 = Type::getFloat32Ty();
  // Type *type2 = Type::getInteger64Ty();
  // Type *type3 = getTypeFromTwoTypes(type1, type2);

  // testTys.push_back(type1);
  // testTys.push_back(type2);
  // testTys.push_back(type3);
  // TestTypes(std::move(testTys));

  // binaryExpr2->evaluateType();
  // PrettyPrint(binaryExpr2);
  // llvm::Value *value = binaryExpr2->codegen();
  // value->print(llvm::outs());

  llvm::Function *function = llvm::Function::Create(llvm::FunctionType::get(builder->getInt32Ty(), false), llvm::GlobalValue::ExternalLinkage, "main", module.get());
  llvm::BasicBlock *basicBlock = llvm::BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(basicBlock);

  ASTBlock *block1(new ASTBlock("ProgramBlock"));
  globalBlockStack->pushBlock(block1);

  ASIntTNumberExpression *intExpr4(new ASIntTNumberExpression(Token("2", Token::Type::LITERAL_INT)));
  ASTAssignVariableStatement *variableStm1(new ASTAssignVariableStatement(intExpr4, Token("a", Token::Type::IDENTIFIER), Type::getInteger32Ty()));
  block1->pushStatement(variableStm1);

  ASIntTNumberExpression *intExpr1(new ASIntTNumberExpression(Token("4", Token::Type::LITERAL_INT)));
  ASIntTNumberExpression *intExpr2(new ASIntTNumberExpression(Token("2", Token::Type::LITERAL_INT)));
  ASIntTNumberExpression *intExpr3(new ASIntTNumberExpression(Token("2", Token::Type::LITERAL_INT)));
  ASTUnaryExpression *unaryExpr1(new ASTUnaryExpression(Token("-", Token::Type::HYPHEN), intExpr3));
  ASTIdentifierExpression *identifierExpr1(new ASTIdentifierExpression(Token("a", Token::Type::IDENTIFIER)));

  ASTBinaryExpression *binaryExpr1(new ASTBinaryExpression(Token("+", Token::Type::PLUS), intExpr1, intExpr2));
  ASTBinaryExpression *binaryExpr2(new ASTBinaryExpression(Token("*", Token::Type::ASTERISK), unaryExpr1, binaryExpr1));
  ASTBinaryExpression *binaryExpr3(new ASTBinaryExpression(Token("-", Token::Type::HYPHEN), identifierExpr1, binaryExpr2));

  block1->pushStatement(binaryExpr3);
  // llvm::Value *value = binaryExpr3->codegen();
  // value->print(llvm::outs());

  globalBlockStack->popBlock();
  PrettyPrint(block1);

  return 1;
}