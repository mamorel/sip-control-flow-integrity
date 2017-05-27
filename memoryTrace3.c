#include <stdio.h>
#include <stdlib.h>

int counter;

void foo();

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

long fac(int n){
	long res = 1;
	while (n > 0){
		res *= n;
		n--;
	}
	return res;
}

void baz(){
	int i = 0;
	while ( i < 10){
		fac(15);
		i++;
	}
}

void barbaz(){
	baz();
}

void foobar() {
	printf("foobar\n");
}

void bar() {
	foo();
}

void foo() {
	counter--;
	if(counter > 0)
		bar();
	else
		foobar();
}

int main(int argc, char *argv[]) {
	if(argc != 2) {printf("Wrong args\n"); return 1;}
	counter = atoi(argv[1]);
	bar();
	return 0;
}
