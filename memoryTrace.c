#include <stdio.h>
#include <stdlib.h>

int counter;

void foo();

void foobar() {
	printf("foobar\n");
}

void bar() {
	foo();
}

void foo() {
	counter--;
	printf("Counter: %d\n", counter);
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
