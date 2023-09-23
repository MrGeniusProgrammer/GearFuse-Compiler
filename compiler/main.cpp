#include <iostream>
#include "./Settings/include.h"
#include "./Type/include.h"
#include "./AST/include.h"

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
  std::vector<Type *> testTys;
  Type *type1 = Type::getFloat32Ty();
  Type *type2 = Type::getInteger64Ty();
  Type *type3 = getTypeFromTwoTypes(type1, type2);

  testTys.push_back(type1);
  testTys.push_back(type2);
  testTys.push_back(type3);
  TestTypes(std::move(testTys));
  return 1;
}