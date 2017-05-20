#include <stdio.h>

void foobar(){
	printf("foobar\n");
	return;
}

void bar(){
	printf("bar\n");
	foobar();
	return;
}

void foo(int a){
	bar();
	return;
}

int main(){
	printf("something\n");
	bar();
	foo(7);
	return 0;
}
