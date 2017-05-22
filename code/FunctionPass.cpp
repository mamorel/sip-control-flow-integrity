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
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/TypeBuilder.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"

#include "Graph.h"

using namespace llvm;

Constant* geti8StrVal(LLVMContext &ctx, char const* str) {
  Constant* strConstant = ConstantDataArray::getString(ctx, str);
  GlobalVariable* GVStr = new GlobalVariable(strConstant->getType(), true,
		GlobalValue::InternalLinkage, strConstant);
  Constant* zero = Constant::getNullValue(IntegerType::getInt32Ty(ctx));
  Constant* indices[] = {zero, zero};
  Constant* strVal = ConstantExpr::getGetElementPtr(strConstant->getType(), strConstant, indices, false);
  return strVal;
}

Function* getFunction(LLVMContext &ctx, Module *mod) {
	FunctionType *printf_type = TypeBuilder<void(char *), false>::get(ctx);

	Function *func = cast<Function>(mod->getOrInsertFunction(
		"registerFunction", printf_type));
	return func;
}

namespace{
struct OurFunctionPass : public FunctionPass {
	static char ID;
	Graph graph;
	OurFunctionPass() : FunctionPass(ID) {}

	virtual bool doInitialization(Module &M){
		graph = Graph();
		return false;
	}
	
	virtual bool doFinalization(Module &M){
		Vertex first = graph.getFirstNode();
		errs() << graph.str();

		return false;
	}
	
	virtual bool runOnFunction(Function &function) {	
		std::string funcName = function.getName().str();
		Vertex funcVertex = Vertex(funcName);
		bool first_instr = true;
	
		for (BasicBlock &block : function) {
			for (Instruction &instruction: block) {
				if(first_instr){
					LLVMContext& Ctx = function.getContext();

					FunctionType *registerType = TypeBuilder<void(char *), false>::get(Ctx);
					Function* registerFunction = cast<Function>(function.getParent()->getOrInsertFunction(
				  		"registerFunction", registerType));

					IRBuilder<> builder(&instruction);
					builder.SetInsertPoint(&block, builder.GetInsertPoint());

					// Insert a call to our function.
					Constant *funcConst = ConstantDataArray::getString(Ctx, funcName.c_str());

					Value *strPtr = builder.CreateGlobalStringPtr(funcName.c_str());
					builder.CreateCall(registerFunction, strPtr);	
					first_instr = false;
				}
				if (auto *callInstruction = dyn_cast<CallInst>(&instruction)) {
					Function *called = callInstruction->getCalledFunction();
					if(called){
						std::string calledName = called->getName().str();
						Vertex calledVertex = Vertex(calledName);
						graph.addEdge(funcVertex, calledVertex);
					}
				}
				if(auto *callInstruction = dyn_cast<ReturnInst>(&instruction)){
					LLVMContext& Ctx = function.getContext();
			
					FunctionType *registerType = TypeBuilder<void(char *), false>::get(Ctx);
					Function* deregisterFunction = cast<Function>(function.getParent()->getOrInsertFunction(
				  		"deregisterFunction", registerType));
		
					IRBuilder<> builder(&instruction);
					builder.SetInsertPoint(&block, builder.GetInsertPoint());

					// Insert a call to our function.
					Constant *funcConst = ConstantDataArray::getString(Ctx, funcName.c_str());
			
					Value *strPtr = builder.CreateGlobalStringPtr(funcName.c_str());
					builder.CreateCall(deregisterFunction, strPtr);
				}
			}
		}
	}
};
}
char OurFunctionPass::ID = 0;
static RegisterPass<OurFunctionPass> X("functionpass", "Function Pass", false, false);
