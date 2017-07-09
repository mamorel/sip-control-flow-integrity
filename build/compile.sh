#!/bin/bash

usage () {
	printf "Usage: $0 <llvm bitcode file> <desired path to new binary> <sensitive function list>\n"
	printf "Example: '$0 ../something.bc ./something sensitiveList.txt'\n"
	exit 1
}

if [ $# -ne 3 ]
then
	printf "Wrong number of parameters\n"
	usage
fi

opt-3.9 -load code/libFunctionPass.so -i $3 -functionpass < $1 > something_pass.bc
opt-3.9 -O3 < something_pass.bc > something_opt.bc
#llc-3.9 something_opt.bc
#gcc -c something_opt.s -o something_opt.o
#gcc -O1 -c ../code/NewStackAnalysis.c -o StackAnalysis.o -lssl -lcrypto
#gcc -O1 something_opt.o StackAnalysis.o -o $2 -lssl -lcrypto


clang-3.9 -g -c -emit-llvm /cfi/code/NewStackAnalysis.c -o NewStackAnalysis.bc -lssl -lcrypto
llvm-link-3.9 NewStackAnalysis.bc something_opt.bc -o something_tmp.bc
clang-3.9 -g something_tmp.bc -lncurses -o $2 -lssl -lcrypto