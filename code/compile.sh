opt-3.9 -load code/libFunctionPass.so -functionpass < ../something.bc > something_pass.bc
opt-3.9 -O3 < something_pass.bc > something_opt.bc
llc-3.9 something_opt.bc
gcc -c something_opt.s -o something_opt.o
gcc -c ../code/NewStackAnalysis.c -o StackAnalysis.o
gcc something_opt.o StackAnalysis.o -o something
./something
