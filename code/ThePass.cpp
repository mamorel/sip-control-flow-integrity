// clang-3.9 -emit-llvm something.c -c something.bc
// mkdir build; cd build
// cmake ..
// make
// opt-3.9 -load code/libThePass.so -thepass < ../something.bc > /dev/null
#include <stdio.h>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"

#include "Graph.h"

using namespace llvm;

Graph graph;

namespace {
	struct ThePass : public CallGraphSCCPass {
		static char ID;
		ThePass() : CallGraphSCCPass(ID) {}

		void getAnalysisUsage(AnalysisUsage &AU) const override {
			AU.addRequired<CallGraphWrapperPass>();
			AU.setPreservesAll();
		}

		virtual bool doInitialization(CallGraph &G) {
			errs() << "Module:  " << G.getModule().getName() << "\n";
			return false;
		}

		virtual bool doFinalization(CallGraph &G) {return false;}

		virtual bool runOnSCC(CallGraphSCC &SCC) {
			for(auto it = SCC.begin() ; it != SCC.end() ; ++it) {
				Function* func = (*it)->getFunction();

				if(!func || func->size() == 0) continue;

				printf("Function: %s\n", func->getName().str().c_str());
				
				for(auto iter = (*it)->begin() ; iter < (*it)->end() ; iter++) {
					if(!iter->second) continue;

					Function *f = iter->second->getFunction();
					if(f) {
						printf("Func: %s\n", f->getName().str().c_str());
					}
				}
				(*it)->dump();
			}
			return false;
		}
	};
}

char ThePass::ID = 0;
static RegisterPass<ThePass> X("thepass", "The Pass", false, false);
