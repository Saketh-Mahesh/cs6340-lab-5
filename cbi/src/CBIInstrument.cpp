/*
 * Copyright Â© 2021 Georgia Institute of Technology (Georgia Tech). All Rights Reserved.
 * Template code for CS 6340 Software Analysis
 * Instructors: Mayur Naik and Chris Poch
 * Head TAs: Kelly Parks and Joel Cooper
 *
 * Georgia Tech asserts copyright ownership of this template and all derivative
 * works, including solutions to the projects assigned in this course. Students
 * and other users of this template code are advised not to share it with others
 * or to make it available on publicly viewable websites including repositories
 * such as GitHub and GitLab. This copyright statement should not be removed
 * or edited. Removing it will be considered an academic integrity issue.
 *
 * We do grant permission to share solutions privately with non-students such
 * as potential employers as long as this header remains in full. However,
 * sharing with other current or future students or using a medium to share
 * where the code is widely available on the internet is prohibited and
 * subject to being investigated as a GT honor code violation.
 * Please respect the intellectual ownership of the course materials
 * (including exam keys, project requirements, etc.) and do not distribute them
 * to anyone not enrolled in the class. Use of any previous semester course
 * materials, such as tests, quizzes, homework, projects, videos, and any other
 * coursework, is prohibited in this course. */
#include "CBIInstrument.h"

using namespace llvm;

namespace instrument {

static const char *CBIBranchFunctionName = "__cbi_branch__";
static const char *CBIReturnFunctionName = "__cbi_return__";

FunctionType *getSanitizerFunctionType(LLVMContext &Context, unsigned numArgs) {
  Type *RetType = Type::getVoidTy(Context);
  std::vector<Type *> ArgTypes(numArgs, Type::getInt32Ty(Context));
  return FunctionType::get(RetType, ArgTypes, false);
}

/*
 * Implement instrumentation for the branch scheme of CBI.
 */
void instrumentCBIBranches(Module *M, Function &F, BranchInst &I) {
  LLVMContext &Context = M->getContext();
  FunctionType *FuncType = getSanitizerFunctionType(Context, 3);
  auto CalleeFunc = M->getOrInsertFunction(*CBIBranchFunctionName, FuncType);

  DebugLoc loc = I.getDebugLoc();

  if (loc) {
    unsigned raw_line = loc.getLine();
    unsigned raw_col = loc.getCol();

    IRBuilder<> Builder(&I);
    llvm::Value *line = Builder.getInt32(raw_line);
    llvm::Value *col = Builder.getInt32(raw_col);
    llvm::Value *cond = Builder.getInt32(I.getCondition());

    std::vector<Value *> Args;
    Args.push_back(line);
    Args.push_back(col);
    Args.push_back(cond);


    Builder.CreateCall(CalleeFunc, Args);
  }
}

/*
 * Implement instrumentation for the return scheme of CBI.
 */
void instrumentCBIReturns(Module *M, Function &F, CallInst &I) {
  LLVMContext &Context = M->getContext();
  FunctionType *FuncType = getSanitizerFunctionType(Context, 3);
  auto CalleeFunc = M->getOrInsertFunction(*CBIReturnFunctionName, FuncType);

  DebugLoc loc = I.getDebugLoc();
  if (loc) {
    unsigned raw_line = loc.getLine();
    unsigned raw_col = loc.getCol();

    llvm::Value *rv = I.getReturnValue();
    if (rv != nullptr) {
      IRBuilder<> Builder(&I);
      llvm::Value *line = Builder.getInt32(raw_line);
      llvm::Value *col = Builder.getInt32(raw_col);

      std::vector<Value *> Args;
      Args.push_back(line);
      Args.push_back(col);
      Args.push_back(rv);

      Builder.CreateCall(CalleeFunc, Args);
    }
  }
}

bool Instrument::runOnFunction(Function &F) {
  LLVMContext &Context = F.getContext();
  Module *M = F.getParent();

  std::vector<Instruction *> div_instructions;

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      if (auto *BI = dyn_cast<BranchInst>(&I)) {
        if (BI->isConditional()) {
          instrumentCBIBranches(M, F, *BI);
        }
      }
      if (auto *RI = dyn_cast<ReturnInst>(&I)) {
        instrumentCBIReturns(M, F, *RI);
      }
    }
  }

  return true;
}

char CBIInstrument::ID = 1;
static RegisterPass<CBIInstrument> X("CBIInstrument",
                                     "Instrumentations for CBI", false, false);

} // namespace instrument
