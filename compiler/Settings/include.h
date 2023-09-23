#pragma once

#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

std::unique_ptr<llvm::LLVMContext> context(new llvm::LLVMContext());
std::unique_ptr<llvm::IRBuilder<>> builder(new llvm::IRBuilder<>(*context));
std::unique_ptr<llvm::Module> module(new llvm::Module("main.gc", *context));