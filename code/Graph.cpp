#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "Graph.h"
#include <openssl/sha.h>

using namespace std;


Vertex::Vertex(string method) {
	methodName = method;
	sensitive = false;
}

Vertex::Vertex(string method, bool sensitive) {
	methodName = method;
	this->sensitive = sensitive;
}

Vertex::Vertex(){methodName = "";sensitive=false;}

string Vertex::getMethodName() {return methodName;}

bool Vertex::isSensitive() {return sensitive;}

Edge::Edge(Vertex org, Vertex dest) {
	origin = org;
	destination = dest;
}

Vertex Edge::getOrigin() {return origin;}
Vertex Edge::getDestination() {return destination;}


Graph::Graph() {}

void Graph::insert(Vertex v) {
	vertices.push_back(v);
}

bool Graph::contains(Vertex v){
	return (find(vertices.begin(), vertices.end(), v) != vertices.end());
}

int Graph::addEdge(Vertex origin, Vertex destination) {
	Edge newEdge(origin, destination);
	if(find(vertices.begin(), vertices.end(), origin) == vertices.end()) {
		vertices.push_back(origin);
	} else if(origin.isSensitive()) {
		int index = find(vertices.begin(), vertices.end(), origin) - vertices.begin();
		vertices[index] = origin;
	}
	if(find(vertices.begin(), vertices.end(), destination) == vertices.end()) {
		vertices.push_back(destination);
	} else if(destination.isSensitive()) {
		int index = find(vertices.begin(), vertices.end(), destination) - vertices.begin();
		vertices[index] = destination;
	}
	if(find(edges.begin(), edges.end(), newEdge) == edges.end())
		edges.push_back(newEdge);
}

vector<Vertex> Graph::getCallees(Vertex v) {
	vector<Vertex> result;
	for(int i = 0; i < edges.size(); i++) {
		if(edges[i].getOrigin() == v) {
			result.push_back(edges[i].getDestination());
		}
	}
	return result;
}

vector<Vertex> Graph::getCallers(Vertex v) {
	vector<Vertex> result;
	for(int i = 0; i < edges.size(); i++) {
		if(edges[i].getDestination() == v) {
			result.push_back(edges[i].getOrigin());
		}
	}
	return result;
}

Vertex Graph::getFirstNode() {
	for(int i = 0; i < vertices.size(); i++) {
		if(getCallers(vertices[i]).size() == 0) {
			return vertices[i];
		}
	}
}

vector<Vertex> Graph::getLastNodes() {
	vector<Vertex> result;
	for(int i = 0; i < vertices.size(); i++) {
		if(getCallees(vertices[i]).size() == 0) {
			result.push_back(vertices[i]);
		}
	}
	return result;
}

void Graph::writeGraphFile() {
	ofstream outFile;
	outFile.open("graph.txt");
	outFile << this->str();
    outFile.close();

    unsigned char c[SHA256_DIGEST_LENGTH];
    FILE *inFile = fopen ("graph.txt", "rb");
    if(inFile == NULL) {
        printf("graph.txt can't be opened.\n");
        return;
    }
    SHA256_CTX sha256;
    int bytes;
	unsigned char data[1024];
    SHA256_Init(&sha256);
    while((bytes = fread(data, 1, 1024, inFile)) != 0)
        SHA256_Update(&sha256, data, bytes);
    fclose (inFile);

    SHA256_Final(c, &sha256);

    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    	ss << std::hex << (int)c[i];

	string checksum = ss.str();
	// TODO: Get the checksum into StackAnalysis.c
	// Idea: write a default checksum to the c file, copy StackAnalysis to a new file and 
	// replace the checksum, then use the new file for compilation...	
}
