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

	node_t *next = stack;
	while(next != NULL) {
		if(next->value == functionName) {
			return;
		}
		next = next->next;
	}

	node_t *new_node = malloc(sizeof(node_t));
	new_node->value = functionName;
	new_node->next = stack;
	stack = new_node;

	printf("Stack: ");
	next = stack;
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

/**
 * Computes a hash over the shadow stack as follows:
 * h(h(h(...h(h(main) | h(f1))...)| h(f4))| h(f5)) | h(sensitiveFunction))
 * if the stack is: sensitiveFunction, f5, f4, ..., f1, main
 *
 * Idea: Compute Merkle tree of known good paths and compare the hash over the stack with 
 * one of the last hashes in the tree:
 *                                          Root node
 *                                         /    ... \
 *     (h(h(main) ...| h(f5)) | h(sensitiveFunction)) ...  Some more stuff
 *                     î
 *       Verify if this is the correct hash
 */
int verifyStack() {
	node_t *currentNode = NULL;
	node_t *nextNode = stack;

	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned char nextHash[SHA256_DIGEST_LENGTH];
	unsigned char buffer[SHA256_DIGEST_LENGTH * 2];

	do {
		nextNode = stack;
    	SHA256_CTX sha256;
    	SHA256_Init(&sha256);
    	SHA256_CTX sha256_next;
    	SHA256_Init(&sha256_next);

		while(nextNode->next != currentNode) {
			nextNode = nextNode->next;
		}
		currentNode = nextNode;
		SHA256_Update(&sha256_next, nextNode->value, sizeof(nextNode->value));
    	SHA256_Final(nextHash, &sha256_next);

		strcpy(buffer,hash);
		memcpy(buffer+strlen(hash),nextHash, SHA256_DIGEST_LENGTH);
		SHA256_Update(&sha256, buffer, sizeof(buffer));
    	SHA256_Final(hash, &sha256);
	} while(currentNode != stack);
	
	// TODO Check against known good hashes.
	return 0;
}


