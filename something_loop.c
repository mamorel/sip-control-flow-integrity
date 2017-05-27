#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void f1();
void f2();
void f3();
void f4();

void f4(){
	printf("f4\n");
}

void f3(){
	printf("f3\n");
	f1();
}

void f2(){
	printf("f2\n");
	int r = rand() % 3;
	if (r == 1 || r == 2)
		f4();
	f3();
}

void f1(){
	printf("f1\n");
	int r = rand() % 3;
	if(r == 1 || r == 2)
		f2();
}

void foo(char a[]){
	printf("%s\n", a);
	return;
}

int main(){
	srand(time(NULL));
	printf("something\n");
	foo("7");
	f1();
	return 0;
}
