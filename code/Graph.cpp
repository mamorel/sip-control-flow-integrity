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

vector<Vertex> Graph::getDominators(Vertex v) {
	vector<Vertex> pathToVertex;
	pathToVertex.push_back(getFirstNode());
	int size = 1;
	for(int i = 0; i < size; i++) {
		vector<Vertex> next = getCallees(pathToVertex[i]);
		// Add all nodes in next to path if they are not already contained and not v
		for(auto it = next.begin() ; it != next.end() ; ++it) {
			if((*it != v) && find(pathToVertex.begin(), pathToVertex.end(), *it) == pathToVertex.end()) {
				pathToVertex.push_back(*it);
				size++;
			}
		}
	} // Now, pathToVertex contains all nodes from start to v (and some more)

	vector<Vertex> preds = getCallers(v);
	size = preds.size();
	for(int i = 0; i < size; i++) {
		vector<Vertex> newDominators = getCallers(preds[i]);
		for(auto it = newDominators.begin() ; it != newDominators.end() ; ++it) {
			if(find(preds.begin(), preds.end(), *it) == preds.end()) {
				preds.push_back(*it);
				size++;
			}
		}
	} // Now, preds contains all nodes from v to start (backwards)

	vector<Vertex> result;
	for(auto it = pathToVertex.begin() ; it != pathToVertex.end() ; ++it) {
		if(find(preds.begin(), preds.end(), *it) != preds.end()) {
			result.push_back(*it);
		}
	} // Result only contains vertices that are contained in both paths
	return result;
}

/**
 * Returns true if at least one path from v to w exists
 */
bool Graph::existsPathFromTo(Vertex v, Vertex w) {
	vector<Vertex> pathToVertex;
	pathToVertex.push_back(v);
	int size = 1;
	for(int i = 0; i < size; i++) {
		vector<Vertex> next = getCallees(pathToVertex[i]);
		if(find(next.begin(), next.end(), w) == next.end()) {
			// Add all nodes in next to path if they are not already contained
			for(auto it = next.begin() ; it != next.end() ; ++it) {
				if(find(pathToVertex.begin(), pathToVertex.end(), *it) == pathToVertex.end()) {
					pathToVertex.push_back(*it);
					size++;
				}
			}
		} else {
			// w is in one of the next nodes -> There is a path!
			return true;
		}
	}
	
	return false;
}

/**
 * Returns the nodes which are on every path from v to w. Start with empty currentPath.
 */
vector<Vertex> Graph::nodesOnEveryPathFromTo(Vertex v, Vertex w, vector<Vertex> currentPath) {
	vector<Vertex> next = getCallees(v);
	vector<vector<Vertex>> allNextNodes;
	for(auto it = next.begin() ; it != next.end() ; ++it) {
		if(*it != v && find(currentPath.begin(), currentPath.end(), *it) == currentPath.end()) {
			// It next node is not v and not already in the path, search for next nodes
			vector<Vertex> nextCurrentPath = currentPath;
			nextCurrentPath.push_back(v);
			allNextNodes.push_back(nodesOnEveryPathFromTo(*it, w, nextCurrentPath));
		}
	}
	if(allNextNodes.size() == 0) {
		vector<Vertex> result;
		return result;
	}
	
	// Now, take the union of all vectors in allNextNodes and add them to pathToVertex.
	vector<Vertex> result;
	for(Vertex &value : allNextNodes[0]) {
		bool isContainedEverywhere = true;
		for(vector<Vertex> &nextVector : allNextNodes) {
			if(find(nextVector.begin(), nextVector.end(), value) == nextVector.end()) {
				isContainedEverywhere = false;
			}
		}
		if(isContainedEverywhere) {
			result.push_back(value);
		}
	}
	return result;
}
