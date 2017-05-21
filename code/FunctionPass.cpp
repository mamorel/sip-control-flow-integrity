// clang-3.9 -emit-llvm something.c -c something.bc
// mkdir build; cd build
// cmake ..
// make
// opt-3.9 -load code/libFunctionPass.so -functionpass < ../something.bc > /dev/null
#include <stdio.h>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include "Graph.h"

using namespace llvm;

namespace{
struct OurFunctionPass : public FunctionPass {
	static char ID;
	OurFunctionPass() : FunctionPass(ID) {}
	
	virtual bool runOnFunction(Function &function) {
		errs() << "Function: " << function.getName().str() << "\n";
		for (BasicBlock &block : function) {
			for (Instruction &instruction: block) {
				if (auto *callInstruction = dyn_cast<CallInst>(&instruction)) {
					Function *called = callInstruction->getCalledFunction();
					if(called)
						errs() << "Calling function " << called->getName().str() << "\n";
				}
			}
		}
	}
};
}
char OurFunctionPass::ID = 0;
static RegisterPass<OurFunctionPass> X("functionpass", "Function Pass", false, false);