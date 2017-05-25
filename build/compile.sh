clang-3.9 -emit-llvm -c ../something.c -o ../something.bc
opt-3.9 -load code/libFunctionPass.so -i sensitiveList.txt -functionpass < ../something.bc > something_pass.bc
opt-3.9 -O3 < something_pass.bc > something_opt.bc
llc-3.9 something_opt.bc
gcc -c something_opt.s -o something_opt.o
g++ -c ../code/StackAnalysis.cpp -o StackAnalysis.o -lssl -lcrypto -std=c++11
gcc something_opt.o StackAnalysis.o -o something -lssl -lcrypto
./something
