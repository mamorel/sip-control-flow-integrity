#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	char *value;
	struct node *next;
} node_t;

node_t *list = NULL;


void registerFunction(char functionName[]) {
	printf("In function: %s\n", functionName);

	node_t *new_node = malloc(sizeof(node_t));
	new_node->value = functionName;
	new_node->next = list;
	list = new_node;
	printf("Stack: ");
	node_t *next = list;
	while(next != NULL) {
		printf("%s ; ", next->value);
		next = next->next;
	}
	printf("\n");
}

void deregisterFunction(char functionName[]) {
	printf("Exit function: %s\n", functionName);
	
	if (list == NULL) {
		fprintf(stderr, "Error: No function on shadow stack that can be poped!\n");
        return;
    }
	if(list->value != functionName) {
		fprintf(stderr, "Error: Wrong function on top of the shadow stack! Want to pop %s but found %s\n",
			functionName, list->value);
		return;
	}

    node_t *next_node = list->next;
    free(list);
    list = next_node;
	node_t *next = list;
	printf("Stack: ");
	while(next != NULL) {
		printf("%s ; ", next->value);
		next = next->next;
	}
	printf("\n");
}


