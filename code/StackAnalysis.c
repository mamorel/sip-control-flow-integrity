// Requires openSSL development package. Install with apt-get install libssl-dev
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <string.h>

typedef struct node {
	char *value;
	struct node *next;
} node_t;

node_t *stack = NULL;
int stack_len = 0;

int built_matrix = 0;
char **mapping;
int **adj_mat;
int vertices_count;

void registerFunction(char functionName[]);
void deregisterFunction(char functionName[]);

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
	node_t *new_node = (node_t*)malloc(sizeof(node_t));
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

int find(char *** list, char * str, int len){
	for (int i = 0 ; i < len ; i++){
		//printf("Comparing: %s and %s\n", (*list)[i], str);
		if (strcmp((*list)[i], str) == 0)
			return i;
	}
	return -1;
}
/*
* Reads known edges from file 'X.txt' line by line.
* Returns pointer to array of known edges and number of edges read.
*/
void readEdges(char ***mapping, int ***adj_mat, int *vertices_count){

	FILE *fp;
	int length = 256;
	size_t len = 256; // might want to fix that later
	ssize_t r;
	char * l = (char *)malloc(length*sizeof(char));

	char * toks;

	int next = 0;

	fp = fopen("graph.txt", "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to read graph file.\n");
		exit(1);
	}

	getline(&l, &len, fp);
	*vertices_count = strtol(l, (char **)NULL, 10);

	getline(&l, &len, fp);
	int line_count = strtol(l, (char **)NULL, 10);

	// alloc func buffer
	char ** buffer = (char **)malloc(2*line_count*sizeof(char*));
	if (buffer == NULL){
		fprintf(stderr, "Failed to alloc buffer.\n");
		exit(1);
	}
	for (int i = 0 ; i < 2*line_count ; i++){
		buffer[i] = (char *)malloc(length*sizeof(char));
		if (buffer[i] == NULL){
			fprintf(stderr, "Failed to alloc buffer[%d].", i);
			exit(1);
		}
	}

	// Alloc mapping
	*mapping = (char **)malloc(*vertices_count * sizeof(char *));
	if (*mapping == NULL){
		fprintf(stderr, "Failed to alloc mapping.\n");
		exit(1);
	}
	for(int i = 0 ; i < *vertices_count ; i++){
		(*mapping)[i] = (char *)malloc(length * sizeof(char));
		if ((*mapping)[i] == NULL){
			fprintf(stderr, "Failed to alloc mapping[%d].\n", i);
			exit(1);
		}
		memset((*mapping)[i], 0, length);
	}

	// Alloc adjacency matrix
	*adj_mat = (int **)malloc(*vertices_count * sizeof(int *));
	if (*adj_mat == NULL){
		fprintf(stderr, "Failed to alloc adjacency matrix.\n");
		exit(1);
	}
	for(int i = 0 ; i < *vertices_count ; i++){
		(*adj_mat)[i] = (int *)malloc(*vertices_count * sizeof(int));
		if ((*adj_mat)[i] == NULL){
			fprintf(stderr, "Failed to alloc adj_mat[%d].\n", i);
			exit(1);
		}
		//memset((*adj_mat)[i], 0, length);
	}

	int count = 0;
	while(( r = getline(&l, &len, fp)) != -1){
		toks = strtok(l, " ");
		while(toks != NULL){
			toks[strcspn(toks, "\n")] = 0;
			strncpy(buffer[count], toks, length);
			count++;
			//printf("Tok: %s\n", toks);
			int found = find(mapping, toks, next);
			if (found == -1){
				strncpy((*mapping)[next], toks, len);
				next++;
			}
			toks = strtok(NULL, " ");
		}while(toks != NULL);
	}

	for (int i = 0 ; i < next ; i++){
		printf("List[%d]=%s\n", i, (*mapping)[i]);
	}

	for (int i = 0; i < 2*line_count; i+=2){
		int row = find(mapping, buffer[i], *vertices_count);
		int col = find(mapping, buffer[i+1], *vertices_count);
		if (row == -1 || col == -1){
			printf("Unexpected row/col for %s -> %s\n", buffer[i], buffer[i+1]);
		}
		(*adj_mat)[row][col] = 1;
	}

	free(l);
	for (int i = 0 ; i < 2*line_count ; i++){
		free(buffer[i]);
	}
	free(buffer);
	return;
}

void response(){
	//cout << "Response mechanism.\n";
	//exit(1);
}

void verify(char ***mapping, int ***adj_mat, int vertices_count){
	node_t *curr = stack, *next;
	char *curr_name;
	char *next_name;

	if(curr == NULL || curr->next == NULL){
		return;
	}
	next = curr->next;

	do{
		curr_name = curr->value;
		next_name = next->value;
		printf("Checking edge: %s -> %s\n", next_name, curr_name);

		int col = find(mapping, curr_name, vertices_count);
		int row = find(mapping, next_name, vertices_count);

		if( row == -1) {
			printf("Could not find function %s\n", curr_name);
			response();
		}
		else if ( col == -1){
			printf("Could not find function %s\n", next_name);
			response();
		}
		else{
			if((*adj_mat)[row][col] != 1){
				printf("Error row %d, col %d\n", row, col);
			}
		}
		curr = next;
		next = curr->next;
		}while(next != NULL);
}

void verifyStack(){
	//registerFunction("main");
	//registerFunction("bar");
	//registerFunction("foobar");
	// TODO: verify file? I suppose here that it is a regular file
	if(!built_matrix){
		readEdges(&mapping, &adj_mat, &vertices_count);
		built_matrix = 1;
		for(int j = 0 ; j < vertices_count ; j ++){
			for(int k = 0 ; k < vertices_count; k++){
				printf("%d ", adj_mat[j][k]);
			}
			printf("\n");
		}
	}

	verify(&mapping, &adj_mat, vertices_count);
}
