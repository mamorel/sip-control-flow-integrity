#include <sstream>
using namespace std;

class Vertex {
public:
    Vertex(string method);
    Vertex();
    string getMethodName();
    bool operator== (const Vertex& rhs) const {
    	return (methodName == rhs.methodName);
    };
    
    bool operator!= (const Vertex& rhs) const {
    	return (methodName != rhs.methodName);
    };
    string str() {
		return this->methodName;
	}

private:
    string methodName;
};


class Edge {
public:
    Edge(Vertex org, Vertex dest);

    Vertex getOrigin();
    Vertex getDestination();
    bool operator== (const Edge& rhs) const {
    	return (origin == rhs.origin) && (destination == rhs.destination);
    };
    string str() {
    	ostringstream os;
		os << this->origin.str() << " -> " << this->destination.str();
		return os.str();
	}
private:
    Vertex origin;
    Vertex destination;
};


class Graph {
public:
    Graph();
    bool contains(Vertex v);
    void insert(Vertex v);

    void printGraph();

    int addEdge(Vertex origin, Vertex destination);

    vector<Vertex> getCallees(Vertex v);

    vector<Vertex> getCallers(Vertex v);

    Vertex getFirstNode();
    
    vector<Vertex> getLastNodes();
    
    vector<Edge> getEdges();
    
    vector<Vertex> getDominators(Vertex v);
    
    bool existsPathFromTo(Vertex v, Vertex w);
    
    vector<Vertex> nodesOnEveryPathFromTo(Vertex v, Vertex w, vector<Vertex> currentPath);
    
    string str() {
    	ostringstream os;
		for(Edge e : this->edges)
  			os << e.str() << endl;
		return os.str();
	}
private:
    vector<Vertex> vertices;
    vector<Edge> edges;
};
