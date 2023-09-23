#include <iostream>
#include "./Settings/include.h"
#include "./Type/include.h"
#include "./Token/include.h"
// #include "./AST/include.h"

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

  std::string show_kind;
  std::string show_value;

protected:
  ASTNodeID ID;

public:
  ASTNode(ASTNodeID ID, std::string show_kind = "", std::string show_value = "") : ID(ID), show_kind(show_kind), show_value(show_value){};
  ASTNode() = default;
  virtual ~ASTNode() = default;

  virtual std::vector<ASTNode *> getChildrenShow() { return std::vector<ASTNode *>(); };
  virtual llvm::Value *codegen() { return nullptr; };
  ASTNodeID getASTNodeID() { return ID; }
};

class ASTExpression : public ASTNode
{
protected:
  Type *type;

public:
  ASTExpression(ASTNodeID ID, std::string show_kind = "", std::string show_value = "") : ASTNode(ID, show_kind, show_value){};
  Type *getType() { return type; }
  void setType(Type *type) { this->type = type; }
};

class ASTIntExpression : public ASTExpression
{
protected:
  Token token;

public:
  ASTIntExpression(Token token) : token(token), ASTExpression(ASTNodeID::ASTIntExpressionID, "IntExpression", token.value){};
};

class ASTFloatExpression : public ASTExpression
{
protected:
  Token token;

public:
  ASTFloatExpression(Token token) : token(token), ASTExpression(ASTNodeID::ASTFloatExpressionID, "FloatExpression", token.value){};
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

  std::cout << "\033[0;91;1m" << indent << token_marker << node->show_kind << "\033[0;92;1m " << node->show_value << "\033[0m" << std::endl;
  std::string last_indent = indent;
  indent += is_last ? "   " : "|  ";
  // indent += is_last ? "   " : "│  ";

  std::vector<ASTNode *> children = node->getChildrenShow();
  for (int i = 0; i < children.size(); i++)
  {
    PrettyPrint(children[i], indent, i == children.size() - 1);
  }
}

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
  Token token1("1", Token::Type::LITERAL_INT);
  std::unique_ptr<ASTIntExpression> expr1(new ASTIntExpression(token1));

  Token token2("2", Token::Type::LITERAL_INT);
  std::unique_ptr<ASTIntExpression> expr2(new ASTIntExpression(token2));

  Token token3("+", Token::Type::PLUS);
  ASTBinaryExpression *expr3(new ASTBinaryExpression(token3, std::move(expr1), std::move(expr2)));
  PrettyPrint(expr3);

  return 1;
}