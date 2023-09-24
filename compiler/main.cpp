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
    ASTNumberExpressionID,
    ASTIntExpressionID,
    ASTFloatExpressionID,
    ASTIdentifierExpressionID,
    ASTFucntionID,
    ASTPrototypeID,
    ASTStatementID,
    ASTReturnStatementID,
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

class ASTExpression : public ASTNode
{
protected:
  Type *type;

public:
  ASTExpression(ASTNodeID ID, std::string showKind = "", std::string showValue = "") : ASTNode(ID, showKind, showValue){};
  ASTExpression(Type *type, ASTNodeID ID, std::string showKind = "", std::string showValue = "") : type(type), ASTNode(ID, showKind, showValue){};
  Type *getType() { return type; }
  virtual void setType(Type *type) { this->type = type; }
  virtual void evaluateType(){};
};

class ASTNumberExpression : public ASTExpression
{
protected:
  Token token;

public:
  ASTNumberExpression(Token token, Type *type) : token(token), ASTExpression(type, ASTNodeID::ASTNumberExpressionID, "NumberExpression", token.value){};
  llvm::Value *codegen() override
  {
    if (getType()->isIntegerTy())
    {
      return llvm::ConstantInt::get(getType()->getLLVMTy(), std::stol(token.value), true);
    }
    else if (getType()->isFloatTy())
    {
      return llvm::ConstantFP::get(getType()->getLLVMTy(), std::stof(token.value));
    }

    return nullptr;
  }
};

class ASTBinaryExpression : public ASTExpression
{
protected:
  Token operatorToken;
  std::unique_ptr<ASTExpression> leftOperand;
  std::unique_ptr<ASTExpression> rightOperand;

public:
  ASTBinaryExpression(Token operatorToken,
                      std::unique_ptr<ASTExpression> leftOperand,
                      std::unique_ptr<ASTExpression> rightOperand) : operatorToken(operatorToken),
                                                                     leftOperand(std::move(leftOperand)),
                                                                     rightOperand(std::move(rightOperand)),
                                                                     ASTExpression(ASTNodeID::ASTBinaryExpressionID, "BinaryExpression", operatorToken.value){};
  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    children.push_back(&(*leftOperand));
    children.push_back(&(*rightOperand));
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
  std::unique_ptr<ASTExpression> operand;

public:
  ASTUnaryExpression(Token operatorToken,
                     std::unique_ptr<ASTExpression> operand) : operatorToken(operatorToken),
                                                               operand(std::move(operand)),
                                                               ASTExpression(ASTNode::ASTUnaryExpressionID, "UnaryExpression", operatorToken.value){};
  std::vector<ASTNode *> getChildrenShow() override
  {
    std::vector<ASTNode *> children;
    children.push_back(&(*operand));
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

  std::unique_ptr<ASTNumberExpression> intExpr1(new ASTNumberExpression(Token("4", Token::Type::LITERAL_INT), Type::getInteger32Ty()));
  std::unique_ptr<ASTNumberExpression> intExpr2(new ASTNumberExpression(Token("2", Token::Type::LITERAL_INT), Type::getInteger32Ty()));
  std::unique_ptr<ASTNumberExpression> intExpr3(new ASTNumberExpression(Token("2", Token::Type::LITERAL_INT), Type::getInteger32Ty()));
  std::unique_ptr<ASTUnaryExpression> unaryExpr1(new ASTUnaryExpression(Token("-", Token::Type::HYPHEN), std::move(intExpr3)));

  std::unique_ptr<ASTBinaryExpression> binaryExpr1(new ASTBinaryExpression(Token("+", Token::Type::PLUS), std::move(intExpr1), std::move(intExpr2)));
  std::unique_ptr<ASTBinaryExpression> binaryExpr2(new ASTBinaryExpression(Token("*", Token::Type::ASTERISK), std::move(unaryExpr1), std::move(binaryExpr1)));

  binaryExpr2->evaluateType();
  PrettyPrint(&(*binaryExpr2));
  llvm::Value *value = binaryExpr2->codegen();
  value->print(llvm::outs());

  return 1;
}