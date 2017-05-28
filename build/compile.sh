#!/bin/bash

usage () {
	printf "Usage: $0 <source file> <desired path to new binary> <sensitive function list>\n"
	printf "Example: '$0 ../something.c ./something sensitiveList.txt'\n"
	exit 1
}

if [ $# -ne 3 ]
then
	printf "Wrong number of parameters\n"
	usage
fi

clang-3.9 -emit-llvm -c $1 -o ../something.bc
opt-3.9 -load code/libFunctionPass.so -i $3 -functionpass < ../something.bc > something_pass.bc
opt-3.9 -O3 < something_pass.bc > something_opt.bc
llc-3.9 something_opt.bc
gcc -c something_opt.s -o something_opt.o
gcc -O1 -c ../code/NewStackAnalysis.c -o StackAnalysis.o -lssl -lcrypto
gcc -O1 something_opt.o StackAnalysis.o -o $2 -lssl -lcrypto
