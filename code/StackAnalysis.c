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
int stack_len = 0;

void registerFunction(char functionName[]) {
	//printf("In function: %s\n", functionName);

	node_t *next = stack;
	while(next != NULL) {
		if(next->value == functionName) {
			return;
		}
		next = next->next;
	}

	stack_len++;
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
	//printf("Exit function: %s\n", functionName);

	if (stack == NULL) {
		fprintf(stderr, "Error: No function on shadow stack that can be poped!\n");
		return;
	}
	if(stack->value != functionName) {
		return;
	}

	stack_len--;
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

void printHash(unsigned char * array, unsigned char * data, char * name, int in_len, int out_len){
	printf("[%s] Hashing: ", name);
	for(int i = 0; i < in_len; i++) {
		printf("%02x", data[i]);
	}
	printf("\n\tResult: ");
	for(int i = 0; i < out_len; i++) {
		printf("%02x", array[i]);
	}
	printf("\n");
}

/*
* Reads known hashes from file 'hashes.txt' line by line.
* Returns pointer to array of known hashes and number of hashes read.
*/
void readHashes(unsigned char ***known_hashes,  int *line_count){
	FILE * fp;
	int len = 2*SHA256_DIGEST_LENGTH+1;
	char line[len];
	int i = 0, line_num = 0, c = 0, prev_c = 0;

	fp = fopen("hashes.txt", "r");
	if(fp == NULL){
		fprintf(stderr, "Failed to read hash file 'hashes.txt'\n");
		exit(1);
	}

	// count number of lines
	do{
		prev_c = c;
		c = fgetc(fp);
		if(c == '\n')
		line_num++;
	}while(c != EOF);
	if(prev_c != '\n' && line_num != 0)
	line_num++;

	*line_count = line_num;

	//printf("Line count: %d\n", *line_count);
	fseek(fp, 0, SEEK_SET);

	*known_hashes = malloc(line_num*sizeof(char *));
	if(*known_hashes == NULL){
		fprintf(stderr, "Failed to malloc known_hashes.\n");
		abort();
	}

	for(int id = 0 ; id < line_num ; id++){
		(*known_hashes)[id] = malloc((len)*sizeof(char));
		if((*known_hashes)[i] == NULL){
			fprintf(stderr, "Failed to malloc known_hashes[%d].\n", i);
			abort();
		}
	}
	while(fgets(line, sizeof(line), fp)){
		if(line[0] != '\n'){
			line[strcspn(line, "\n")] = 0;
			strncpy((*known_hashes)[i], line, len);
			i++;
		}else
		continue;
	}

	fclose(fp);
	return;
}

/*
* Compares the actual hash with the array of possible hashes.
* Returns position of matching hash if match found, -1 otherwise.
*/
int verify(unsigned char **known, int known_size, unsigned char *computed){
	char tmp[2*SHA256_DIGEST_LENGTH];
	for(int j = 0 ; j < SHA256_DIGEST_LENGTH ; j++){
		sprintf(tmp+(j*2), "%02x", computed[j]);
	}
	for(int i = 0 ; i < known_size; i++){
		printf("Known: '%s' -- Computed '%s'\n", known[i], tmp);
		if( strncmp(tmp, known[i], 2*SHA256_DIGEST_LENGTH) == 0) return i;
	}
	return -1;
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
void mtree(unsigned char **list, int size) {
	if(size == 1){
		return;
	}

	unsigned char buffer[SHA256_DIGEST_LENGTH * 2] = {0};
	unsigned char hash[SHA256_DIGEST_LENGTH] = {0};

	int len;

	for(int i = 0; i < size; i=i+2){
		SHA256_CTX sha256;
		SHA256_Init(&sha256);

		memset(buffer, 0, 2*SHA256_DIGEST_LENGTH);

		// Current hash
		memcpy(buffer, list[i], SHA256_DIGEST_LENGTH); // cpy current hash to buff
		SHA256_Update(&sha256, list[i], SHA256_DIGEST_LENGTH);

		if((i+1) < size){
			// Hash next if existing
			memcpy(buffer+SHA256_DIGEST_LENGTH,list[i+1], SHA256_DIGEST_LENGTH); // cpy next hash to end of buff

			SHA256_Update(&sha256, list[i+1], SHA256_DIGEST_LENGTH);
		}else{
			// Else reuse previous hash
			memcpy(buffer+SHA256_DIGEST_LENGTH, list[i], SHA256_DIGEST_LENGTH); // cpy current hash to buff
			SHA256_Update(&sha256, list[i], SHA256_DIGEST_LENGTH);
		}

		// Compute SHA256( SHA256(current) | SHA256(next) )
		SHA256_Final(hash, &sha256);

		//printHash(hash, buffer, "concat", SHA256_DIGEST_LENGTH*2, SHA256_DIGEST_LENGTH);

		memcpy(list[i/2], hash, SHA256_DIGEST_LENGTH);

		if((i+1) < size){
			memset(list[i+1], 0, SHA256_DIGEST_LENGTH*sizeof(unsigned char));
		}else{
			size++;
		}
	}
	mtree(list, size/2);
	return;
}

void stackToArray(unsigned char ***res){
	*res = malloc(stack_len * sizeof(char*));
	if(*res == NULL){
		fprintf(stderr, "Failed to malloc stack array.\n");
		abort();
	}

	for(int i = 0 ; i < stack_len; i++){
		(*res)[i] = malloc(SHA256_DIGEST_LENGTH * sizeof(char));
		if((*res)[i] == NULL){
			fprintf(stderr, "Failed to malloc stack array[%d].\n", i);
			abort();
		}
	}
	node_t *iter = stack;
	int i = 0;
	while( iter != NULL){
		strncpy((*res)[i], iter->value, SHA256_DIGEST_LENGTH);
		iter = iter->next;
		i++;
	}
	return;
}

void response(){
	exit(1);
}

int verifyStack(){ // verifyStack
	unsigned char buf[SHA256_DIGEST_LENGTH];

	// Get known hashes from file
	int known_count = 0;
	unsigned char **known_hashes;
	readHashes(&known_hashes, &known_count);

	for(int i = 0 ; i < known_count; i++)
	printf("Known hash: %s\n", known_hashes[i]);

	// Build array from stack
	unsigned char **stackArray;
	stackToArray(&stackArray);

	// hash each leaf
	for(int i = 0 ;  i < stack_len ; i++){
		SHA256_CTX ctx;
		SHA256_Init(&ctx);

		SHA256_Update(&ctx, stackArray[i], strlen(stackArray[i]));
		SHA256_Final(buf, &ctx);
		printHash(buf, stackArray[i], "stackArray", strlen(stackArray[i]), SHA256_DIGEST_LENGTH);
		memcpy(stackArray[i], buf, SHA256_DIGEST_LENGTH);
	}

	// Compute root of the merkle tree
	mtree(stackArray, stack_len);
	printf("Root hash: ");
	for(int j = 0; j < SHA256_DIGEST_LENGTH ; j++){
		printf("%02x", stackArray[0][j]);
	}
	printf("\n");

	// verify trace
	int res;
	if( (res = verify(known_hashes, known_count, stackArray[0])) != -1){
		printf("Stack Trace OK, found match at position %d: %s\n", res, known_hashes[res]);
	}else{
		printf("Invalid Stack Trace!\n");
		//response();
	}

	// Free known_hashes
	for(int i = 0 ; i < known_count; i++){
		free(known_hashes[i]);
	}
	free(known_hashes);

	// Free computed hashes
	for(int i = 0 ; i < stack_len ; i++){
		free(stackArray[i]);
	}
	free(stackArray);

	return 0;
}
