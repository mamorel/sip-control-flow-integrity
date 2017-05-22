#include <stdio.h>

void foobar(){
	printf("%d + %d = %d\n", 1, 1, 2);
	return;
}

void bar(){
	printf("%d + %d = %d\n", 7, 4, 11);
	foobar();
	return;
}

void foo(char a[]){
	printf("%s\n", a);
	return;
}

int main(){
	printf("something\n");
	bar();
	foo("7");
	return 0;
}
