#include <vector>
#include <iostream>
#include <iomanip>
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

vector<Vertex> Graph::getSensitiveNodes() {
	vector<Vertex> result;
	for(Vertex &v : vertices) {
		if(v.isSensitive()) {
			result.push_back(v);
		}
	}
	return result;
}

void rewriteStackAnalysis(string checksum) {
	ifstream filein("../code/StackAnalysis.c"); //File to read from
	ofstream fileout("../code/NewStackAnalysis.c"); //Temporary file
	if(!filein || !fileout){
		cout << "Error opening files!" << endl;
		return;
	}

	string strReplace = "	char *expectedHash = \"";

	string strTemp;
	while(getline(filein, strTemp)) {
		if(strTemp.find(strReplace) != string::npos) {
			strTemp = "	char *expectedHash = \"" + checksum + "\";";
		}
		strTemp += "\n";
		fileout << strTemp;
	}
}

void Graph::writeGraphFile() {
	vector<Vertex> paths = getPathsToSensitiveNodes();
	ofstream outFile;
	ofstream outChecksum;

	outFile.open("graph.txt");
	vector<Vertex> verticesOnPath;
	vector<Edge> edgesOnPath;
	for(Edge &e : edges) {
		// Only add edges that are contained in a path to a sensitive function to get a
		// smaller adjacency matrix in the stack analysis
		if(find(paths.begin(), paths.end(), e.getDestination()) != paths.end()
				&& find(paths.begin(), paths.end(), e.getOrigin()) != paths.end()) {
					edgesOnPath.push_back(e);
					if( find(verticesOnPath.begin(), verticesOnPath.end(), e.getOrigin()) == verticesOnPath.end())
						verticesOnPath.push_back(e.getOrigin());
					if( find(verticesOnPath.begin(), verticesOnPath.end(), e.getDestination()) == verticesOnPath.end())
						verticesOnPath.push_back(e.getDestination());
		}
	}
	sort(edgesOnPath.begin(), edgesOnPath.end(), [](Edge e1, Edge e2)->bool{
		return e1.getDestination().str() < e2.getDestination().str();
	});
	outFile << verticesOnPath.size() << endl;
	outFile << edgesOnPath.size() << endl;
	for(Edge &e : edgesOnPath)
		outFile << e.str() << "\n";
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
		ss << setfill('0') << setw(2) << std::hex << (int)c[i];

	string checksum = ss.str();

	// Write checksum to file
	outChecksum.open("checksum.txt");
	outChecksum << checksum << endl;
	outChecksum.close();
	rewriteStackAnalysis(checksum);
	return;
	// TODO: Get the checksum into StackAnalysis.c
	// Idea: write a default checksum to the c file, copy StackAnalysis to a new file and
	// replace the checksum, then use the new file for compilation...
}

vector<Vertex> Graph::getPathsToSensitiveNodes() {
	vector<Vertex> pathToSensitiveFunctions = getSensitiveNodes();
	int size = pathToSensitiveFunctions.size();
	for(int i = 0; i < size; i++) {
		vector<Vertex> newDominators = getCallers(pathToSensitiveFunctions[i]);
		for(auto it = newDominators.begin() ; it != newDominators.end() ; ++it) {
			if(find(pathToSensitiveFunctions.begin(), pathToSensitiveFunctions.end(), *it)
					== pathToSensitiveFunctions.end()) {
				pathToSensitiveFunctions.push_back(*it);
				size++;
			}
		}
	}
	return pathToSensitiveFunctions;
}
