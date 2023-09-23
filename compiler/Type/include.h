#pragma once

#include <vector>
#include <string>
#include "assert.h"
#include "llvm/IR/Type.h"
#include "../Settings/include.h"

class Float16Type;
class Float32Type;
class Float64Type;
class Float80Type;
class Float128Type;
class IntegerType;
class PointerType;
class VoidType;
class FunctionType;
class ArrayType;

class Type
{
public:
  enum TypeID
  {
    Float16TyID = 0,
    Float32TyID,
    Float64TyID,
    Float80TyID,
    Float128TyID,
    VoidTyID,

    IntegerTyID,
    FunctionTyID,
    PointerTyID,
    StructTyID,
    ArrayTyID,
    FixedVectorTyID,
    ScalableVectorTyID,
    TypedPointerTyID,
  };

protected:
  TypeID ID;
  std::vector<Type *> ContainedTys;
  unsigned SubclassData = 24;

  explicit Type(TypeID tid) : ID(tid), SubclassData(0) {}
  explicit Type(TypeID tid, unsigned subClassData) : ID(tid), SubclassData(subClassData) {}
  ~Type() = default;

  void setSubclassData(unsigned val)
  {
    SubclassData = val;
    assert(getSubclassData() == val && "Subclass data too large for field");
  }

public:
  TypeID getTypeID() const { return ID; }

  Type *getElementTy() { return ContainedTys[0]; }
  Type *getContainedType(unsigned i) const
  {
    assert(i < ContainedTys.size() && "Index out of range!");
    return ContainedTys[i];
  }

  unsigned getContainedTysLength() { return ContainedTys.size(); }

  static Float16Type *getFloat16Ty();
  static Float32Type *getFloat32Ty();
  static Float64Type *getFloat64Ty();
  static Float80Type *getFloat80Ty();
  static Float128Type *getFloat128Ty();

  static IntegerType *getIntegerTy(unsigned numOfBits);
  static IntegerType *getInteger1Ty();
  static IntegerType *getInteger8Ty();
  static IntegerType *getInteger16Ty();
  static IntegerType *getInteger32Ty();
  static IntegerType *getInteger64Ty();

  static FunctionType *getFunctionTy(std::vector<Type *> Params, Type *Result, bool IsVarArgs);
  static FunctionType *getFunctionTy(std::vector<Type *> Params, Type *Result);
  static FunctionType *getFunctionTy(Type *Result, bool IsVarArgs);
  static FunctionType *getFunctionTy(Type *Result);

  static PointerType *getPointerTy(Type *ElType, unsigned AddrSpace);
  static PointerType *getPointerTy(Type *ElType);

  static VoidType *getVoidTy();

  static ArrayType *getArrayTy(Type *ElTy, uint64_t NumElements);

  bool isFloat16Ty() { return getTypeID() == TypeID::Float16TyID; }
  bool isFloat32Ty() { return getTypeID() == TypeID::Float32TyID; }
  bool isFloat64Ty() { return getTypeID() == TypeID::Float64TyID; }
  bool isFloat80Ty() { return getTypeID() == TypeID::Float80TyID; }
  bool isFloat128Ty() { return getTypeID() == TypeID::Float128TyID; }
  bool isIntegerTy() { return getTypeID() == TypeID::IntegerTyID; }
  bool isFunctionTy() { return getTypeID() == TypeID::FunctionTyID; }
  bool isPointerTy() { return getTypeID() == TypeID::PointerTyID; }
  bool isVoidTy() { return getTypeID() == TypeID::VoidTyID; }
  bool isStructTy() { return getTypeID() == TypeID::StructTyID; }
  bool isArrayTy() { return getTypeID() == TypeID::ArrayTyID; }

  bool isFloatTy() { return isFloat16Ty() || isFloat32Ty() || isFloat64Ty() || isFloat80Ty() || isFloat128Ty(); }
  bool isNumberTy() { return isFloatTy() || isIntegerTy(); }

  unsigned getSubclassData() const { return SubclassData; }
  virtual llvm::Type *getLLVMTy() = 0;
  virtual std::string getManglingName() = 0;

  virtual Type *copy() = 0;

  virtual bool isEquals(Type *type)
  {
    if (getTypeID() != type->getTypeID() || getContainedTysLength() != type->getContainedTysLength() || getSubclassData() != type->getSubclassData())
    {
      return false;
    }

    for (int i = 0; i < type->getContainedTysLength(); i++)
    {
      if (!getContainedType(i)->isEquals(type->getContainedType(i)))
      {
        return false;
      }
    }

    return true;
  }
};

class Float16Type : public Type
{
protected:
  explicit Float16Type() : Type(TypeID::Float16TyID, 16){};

public:
  static Float16Type *get() { return new Float16Type(); }
  Float16Type *copy() override { return new Float16Type(); }

  llvm::Type *getLLVMTy() override { return llvm::Type::getHalfTy(*context); }
  std::string getManglingName() override { return "float16"; }
};

class Float32Type : public Type
{
protected:
  explicit Float32Type() : Type(TypeID::Float32TyID, 32){};

public:
  static Float32Type *get() { return new Float32Type(); }
  Float32Type *copy() override { return new Float32Type(); }

  llvm::Type *getLLVMTy() override { return llvm::Type::getFloatTy(*context); }
  std::string getManglingName() override { return "float32"; }
};

class Float64Type : public Type
{
protected:
  explicit Float64Type() : Type(TypeID::Float64TyID, 64){};

public:
  static Float64Type *get() { return new Float64Type(); }
  Float64Type *copy() override { return new Float64Type(); }

  llvm::Type *getLLVMTy() override { return llvm::Type::getDoubleTy(*context); }
  std::string getManglingName() override { return "float64"; }
};

class Float80Type : public Type
{
protected:
  explicit Float80Type() : Type(TypeID::Float80TyID, 80){};

public:
  static Float80Type *get() { return new Float80Type(); }
  Float80Type *copy() override { return new Float80Type(); }

  llvm::Type *getLLVMTy() override { return llvm::Type::getX86_FP80Ty(*context); }
  std::string getManglingName() override { return "float80"; }
};

class Float128Type : public Type
{
protected:
  explicit Float128Type() : Type(TypeID::Float128TyID, 128){};

public:
  static Float128Type *get() { return new Float128Type(); }
  Float128Type *copy() override { return new Float128Type(); }

  llvm::Type *getLLVMTy() override { return llvm::Type::getFP128Ty(*context); }
  std::string getManglingName() override { return "float128"; }
};

class VoidType : public Type
{
protected:
  explicit VoidType() : Type(TypeID::VoidTyID, 32){};

public:
  static VoidType *get() { return new VoidType(); }
  VoidType *copy() override { return new VoidType(); }

  llvm::Type *getLLVMTy() override { return llvm::Type::getVoidTy(*context); }
  std::string getManglingName() override { return "void"; }
};

class IntegerType : public Type
{
protected:
  explicit IntegerType(unsigned numOfBits) : Type(TypeID::IntegerTyID)
  {
    setSubclassData(numOfBits);
  }

public:
  static IntegerType *get(unsigned numOfBits) { return new IntegerType(numOfBits); }
  IntegerType *copy() override { return new IntegerType(getSubclassData()); }

  llvm::Type *getLLVMTy() override { return llvm::Type::getIntNTy(*context, getSubclassData()); }
  std::string getManglingName() override { return "int" + std::to_string(getSubclassData()); }
};

class PointerType : public Type
{
protected:
  PointerType(Type *ElType, unsigned AddrSpace) : Type(TypeID::PointerTyID, AddrSpace)
  {
    ContainedTys.push_back(ElType);
  }

  PointerType(Type *ElType) : Type(TypeID::PointerTyID, ElType->getSubclassData())
  {
    ContainedTys.push_back(ElType);
  }

public:
  static PointerType *get(Type *ElType, unsigned AddrSpace) { return new PointerType(ElType, AddrSpace); }
  static PointerType *get(Type *ElType) { return new PointerType(ElType); }
  PointerType *copy() override { return new PointerType(getElementTy(), getSubclassData()); }

  llvm::PointerType *getLLVMTy() override { return llvm::PointerType::get(*context, getSubclassData()); }

  inline unsigned getAddressSpace() const { return getSubclassData(); }
  std::string getManglingName() override { return "ptr<" + getElementTy()->getManglingName() + ">"; }
};

class FunctionType : public Type
{
protected:
  bool IsVarArgs;
  FunctionType(std::vector<Type *> Params, Type *Result, bool IsVarArgs = false) : Type(TypeID::FunctionTyID, Result->getSubclassData()), IsVarArgs(IsVarArgs)
  {
    ContainedTys = std::move(Params);
    ContainedTys.push_back(Result);
  }

  FunctionType(Type *Result, bool isVarArgs = false) : Type(TypeID::FunctionTyID, Result->getSubclassData()), IsVarArgs(isVarArgs)
  {
    ContainedTys.push_back(Result);
  }

public:
  static FunctionType *get(std::vector<Type *> Params, Type *Result, bool IsVarArgs) { return new FunctionType(std::move(Params), Result, IsVarArgs); }
  static FunctionType *get(std::vector<Type *> Params, Type *Result) { return new FunctionType(std::move(Params), Result); }
  static FunctionType *get(Type *Result, bool IsVarArgs) { return new FunctionType(Result, IsVarArgs); }
  static FunctionType *get(Type *Result) { return new FunctionType(Result); }
  FunctionType *copy() override
  {
    std::vector<Type *> CopiedParams;
    for (int i = 0; i < getNumParams(); i++)
    {
      CopiedParams.push_back(getParamType(i));
    }

    return new FunctionType(std::move(CopiedParams), getReturnType(), IsVarArgs);
  }

  llvm::FunctionType *getLLVMTy() override
  {
    std::vector<llvm::Type *> Params;
    for (int i = 0; i < getNumParams(); i++)
    {
      Params.push_back(getParamType(i)->getLLVMTy());
    }

    return llvm::FunctionType::get(getReturnType()->getLLVMTy(), Params, IsVarArgs);
  }

  Type *getReturnType() const { return ContainedTys[getNumParams()]; }
  Type *getParamType(unsigned i) const { return ContainedTys[i]; }
  unsigned getNumParams() const { return ContainedTys.size() - 1; }

  std::string getManglingName() override
  {
    std::string result;
    result += "(";
    for (int i = 0; i < getNumParams(); i++)
    {
      result += getParamType(i)->getManglingName();
      if (i != getNumParams() - 1)
      {
        result += ",";
      }
    }
    result += ")";

    return result;
  }
};

class ArrayType : public Type
{
protected:
  uint64_t NumElements;
  ArrayType(Type *ElTy, uint64_t NumElements) : Type(TypeID::ArrayTyID, ElTy->getSubclassData() * NumElements), NumElements(NumElements)
  {
    ContainedTys.push_back(ElTy);
  }

public:
  static ArrayType *get(Type *ElTy, uint64_t NumElements) { return new ArrayType(ElTy, NumElements); }
  ArrayType *copy() override { return new ArrayType(getElementTy(), NumElements); }

  llvm::ArrayType *getLLVMTy() override
  {
    return llvm::ArrayType::get(getElementTy()->getLLVMTy(), NumElements);
  }

  unsigned getNumElements() const { return NumElements; }
  std::string getManglingName() override
  {
    return "[" + getElementTy()->getManglingName() + "]";
  }
};

Float16Type *Type::getFloat16Ty()
{
  return Float16Type::get();
}

Float32Type *Type::getFloat32Ty()
{
  return Float32Type::get();
}

Float64Type *Type::getFloat64Ty()
{
  return Float64Type::get();
}

Float80Type *Type::getFloat80Ty()
{
  return Float80Type::get();
}

Float128Type *Type::getFloat128Ty()
{
  return Float128Type::get();
}

IntegerType *Type::getIntegerTy(unsigned numOfBits)
{
  return IntegerType::get(numOfBits);
}

IntegerType *Type::getInteger1Ty()
{
  return getIntegerTy(1);
}

IntegerType *Type::getInteger8Ty()
{
  return getIntegerTy(8);
}

IntegerType *Type::getInteger16Ty()
{
  return getIntegerTy(16);
}

IntegerType *Type::getInteger32Ty()
{
  return getIntegerTy(32);
}

IntegerType *Type::getInteger64Ty()
{
  return getIntegerTy(64);
}

FunctionType *Type::getFunctionTy(std::vector<Type *> Params, Type *Result, bool IsVarArgs)
{
  return FunctionType::get(std::move(Params), Result, IsVarArgs);
}

FunctionType *Type::getFunctionTy(std::vector<Type *> Params, Type *Result)
{
  return FunctionType::get(std::move(Params), Result);
}

FunctionType *Type::getFunctionTy(Type *Result, bool IsVarArgs)
{
  return FunctionType::get(Result, IsVarArgs);
}

FunctionType *Type::getFunctionTy(Type *Result)
{
  return FunctionType::get(Result);
}

PointerType *Type::getPointerTy(Type *ElType, unsigned AddrSpace)
{
  return PointerType::get(ElType, AddrSpace);
}

PointerType *Type::getPointerTy(Type *ElType)
{
  return PointerType::get(ElType);
}

VoidType *Type::getVoidTy()
{
  return VoidType::get();
}

ArrayType *Type::getArrayTy(Type *ElTy, uint64_t NumElements)
{
  return ArrayType::get(ElTy, NumElements);
}