// Requires openSSL development package. Install with apt-get install libssl-dev
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

typedef struct node {
	char *value;
	struct node *next;
} node_t;

node_t *stack = NULL;


void registerFunction(char functionName[]) {
	printf("In function: %s\n", functionName);

	node_t *new_node = malloc(sizeof(node_t));
	new_node->value = functionName;
	new_node->next = stack;
	stack = new_node;

	printf("Stack: ");
	node_t *next = stack;
	while(next != NULL) {
		printf("%s ; ", next->value);
		next = next->next;
	}
	printf("\n");
}

void deregisterFunction(char functionName[]) {
	printf("Exit function: %s\n", functionName);
	
	if (stack == NULL) {
		fprintf(stderr, "Error: No function on shadow stack that can be poped!\n");
        return;
    }
	if(stack->value != functionName) {
		fprintf(stderr, "Error: Wrong function on top of the shadow stack! Want to pop %s but found %s\n",
			functionName, stack->value);
		return;
	}

    node_t *next_node = stack->next;
    free(stack);
    stack = next_node;

	node_t *next = stack;
	printf("Stack: ");
	while(next != NULL) {
		printf("%s ; ", next->value);
		next = next->next;
	}
	printf("\n");
}

int verifyStack() {
	unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

	char *buffer;
	node_t *currentNode = NULL;
	node_t *nextNode = stack;
	while(nextNode->next != currentNode) {
		nextNode = nextNode->next;
	}
	currentNode = nextNode;
	buffer = malloc((strlen(nextNode->value) + SHA256_DIGEST_LENGTH) * sizeof(char));
	strcpy(buffer,hash);
	memcpy(buffer+strlen(hash),nextNode->value,sizeof(nextNode->value));
	SHA256_Update(&sha256, buffer, sizeof(buffer));
	
    SHA256_Final(hash, &sha256);

}


