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

    vector<Vertex> getSuccessors(Vertex v);

    vector<Vertex> getPredecessors(Vertex v);

    Vertex getFirstNode();
    
    vector<Vertex> getLastNodes();
private:
    vector<Vertex> vertices;
    vector<Edge> edges;
};