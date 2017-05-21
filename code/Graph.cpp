#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "Graph.h"

using namespace std;


Vertex::Vertex(string method) {
	methodName = method;
}
Vertex::Vertex(){methodName = "";};

string Vertex::getMethodName() {return methodName;}

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
	if(find(vertices.begin(), vertices.end(), origin) == vertices.end())
		vertices.push_back(origin);
	if(find(vertices.begin(), vertices.end(), destination) == vertices.end())
		vertices.push_back(destination);

	if(find(edges.begin(), edges.end(), newEdge) == edges.end())
		edges.push_back(newEdge);
}

vector<Vertex> Graph::getSuccessors(Vertex v) {
	vector<Vertex> result;
	for(int i = 0; i < edges.size(); i++) {
		if(edges[i].getOrigin() == v) {
			result.push_back(edges[i].getDestination());
		}
	}
	return result;
}

vector<Vertex> Graph::getPredecessors(Vertex v) {
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
		if(getPredecessors(vertices[i]).size() == 0) {
			return vertices[i];
		}
	}
}

vector<Vertex> Graph::getLastNodes() {
	vector<Vertex> result;
	for(int i = 0; i < vertices.size(); i++) {
		if(getSuccessors(vertices[i]).size() == 0) {
			result.push_back(vertices[i]);
		}
	}
	return result;
}
