// clang-3.9 -emit-llvm something.c -c something.bc
// mkdir build; cd build
// cmake ..
// make
// opt-3.9 -load code/libThePass.so -thepass < ../something.bc > /dev/null
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

		virtual bool doInitialization(CallGraph &G){
			errs() << "Module:  " << G.getModule().getName() << "\n";
			return false;
		}
		virtual bool doFinalization(CallGraph &G){return false;}

		virtual bool runOnSCC(CallGraphSCC &SCC) {
		for(auto it = SCC.begin() ; it != SCC.end() ; ++it){
			Function* func = (*it)->getFunction();

			if (func == 0) continue; 

			for(auto iter = (*it)->begin() ; iter < (*it)->end() ; iter++){
				Function *f = iter->second->getFunction();
				if(f)
					errs() << "Func: " << f->getName() << "\n";
			}

			(*it)->dump();
		    }
		return false;
		}
	};
}

char ThePass::ID = 0;
static RegisterPass<ThePass> X("thepass", "The Pass", false, false);
