/*
* Matthew Buchanan
* CS-340
* Project 4: Kruskal's and Prim's algorithms
*/

#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include <stdio.h>
#include <vector>
using namespace std;

/* Vertex Class */
class Vertex
{
public:
	int id;
	bool visited;
	vector<int> adjacencies;
	vector<int> adjacencyIndex;//pointers to corresponding edge's index in the priority queue for Prim's

	Vertex()
	{
		visited = false;
	};

	~Vertex()
	{
	};
};

/* Edge Class */
class Edge
{
public:
	Vertex * u;
	Vertex * v;
	int weight;
	int index;//pointer to edge's index in u's adjacency list set only once on initialization of prim's
	int key;//key value for Q

	Edge()
	{
		key = 10000;////set default key value larger than any weight in E
	};

	~Edge()
	{
	};
};

/* Graph class */
class Graph
{
public:
	vector<Vertex> vertices;
	vector<Edge> edges;
	vector<Edge> sortedEdges;//list of sorted edges for Kruskal's algorithm
	vector<Edge> primResult;
	Edge * pQueue;// priority queue for Prim's
	int pQueueSize;

	Graph()
	{
	};

	~Graph()
	{
		delete[] pQueue;
	};

	/* Performs heap sort on the array a for Kruskal's*/
	void heapSort(Edge * a, int s)
	{
		for (int i = s; i >= 1; i--)
		{
			Edge temp = a[1];
			a[1] = a[i];
			a[i] = temp;
			heapify(a, 1, i - 1);
		}
		for (int j = 1; j < s; j++)
			sortedEdges.push_back(a[j]);
	}

	/* Maintains the heap parameters */
	void heapify(Edge * a, int i, int s)
	{
		int left = 2 * i;
		Edge temp;
		temp = a[i];
		while (left <= s)
		{
			if (left < s && a[left + 1].weight > a[left].weight)
				left++;
			if (temp.weight > a[left].weight)
				break;
			else if (temp.weight <= a[left].weight)
			{
				a[left / 2] = a[left];
				left *= 2;
			}
		}
		a[left / 2] = temp;
	}

	/* Builds a heap from the vector edges for heapsort for Kruskal's algorithm */
	void heap()
	{
		Edge * a = new Edge[edges.size() + 1];
		int s = edges.size() + 1;
		for (int n = 1; n < s; n++)
			a[n] = edges[n - 1];
		for (int i = s / 2; i >= 1; i--)
			heapify(a, i, s);
		heapSort(a, s);
	}

	/* Priority queue implementation for Prim's algorithm */
	void minHeap()//construct heap by repeated insertion for use as priority queue
	{
		pQueue = new Edge[edges.size() + 1];
		pQueueSize = 0;
	}

	/* Heapify for the min heap priority queue */
	void minHeapify(Edge * a, int i)
	{
		int smallest;
		int left = 2 * i;
		int right = left + 1;
		if (left <= pQueueSize && a[left].key < a[i].key)
			smallest = left;
		else
			smallest = i;
		if (right <= pQueueSize && a[right].key < a[smallest].key)
			smallest = right;
		if (smallest != i)
		{
			Edge tmp = a[i];
			a[i] = a[smallest];
			a[smallest] = tmp;
			minHeapify(a, smallest);
		}
	}

	/* Pop from min priority queue */
	Edge extractMin()
	{
		if (pQueueSize > 0)
		{
			Edge min = pQueue[1];
			pQueue[1] = pQueue[pQueueSize];
			pQueueSize--;
			minHeapify(pQueue, 1);
			return min;
		}
	}

	/* Push to priority queue */
	void minHeapInsert(Edge e)
	{
		pQueueSize++;
		e.u->adjacencyIndex[e.index] = pQueueSize;//update pointer to edge e's index in the heap
		pQueue[pQueueSize] = e;
		int index = pQueueSize;
		while (&e != &pQueue[1] && e.key < pQueue[index / 2].key)
		{
			Edge tmp = pQueue[index / 2];
			e.u->adjacencyIndex[e.index] = index / 2;//update pointer
			pQueue[index / 2] = e;
			pQueue[index] = tmp;
			index = index / 2;
		}
	}

	/* Decrease key and bubble up in min heap */
	bool decreaseKey(int index, int key)
	{
		if (key > pQueue[index].key)
		{
			return false;
		}
		pQueue[index].key = key;
		while (index > 1 && pQueue[index].key < pQueue[index / 2].key)
		{
			Edge e = pQueue[index];
			pQueue[index] = pQueue[index / 2];
			e.u->adjacencyIndex[e.index] = index / 2;
			pQueue[index / 2] = e;
			index = index / 2;
		}
		return true;
	}

	/* Prim's algorithm to find MST */
	void mstPrim()
	{
		for (int i = 0; i < edges.size(); i++)//add all edges to prioriy queue
			minHeapInsert(edges[i]);
		Vertex * tempV = &vertices[0];//pick a vertex to start
		for (int i = 0; i < vertices.size() - 1; i++) // for the V-1 edges in the final MST
		{
			tempV->visited = true;
			for (int j = 0; j < tempV->adjacencies.size(); j++)
			{
				decreaseKey(tempV->adjacencyIndex[j], pQueue[tempV->adjacencyIndex[j]].weight);  //decrease tempV's adjacent edge's keys by assigning their weights, which each edge holds, to their keys
			}
			bool b = false;//Used to track whether a light edge has been chosen and added to MST
			while (pQueueSize > 1 && !b)//pop off the priority queue until a safe edge is found
			{
				Edge tempE = extractMin();
				if (!tempE.u->visited)
				{
					tempV = &vertices[tempE.u->id - 1];//advance to next vertex
					primResult.push_back(tempE);//store edge in mst
					b = true;
				}
				else if (!tempE.v->visited)
				{
					tempV = &vertices[tempE.v->id - 1];
					primResult.push_back(tempE);
					b = true;
				}
			}
		}
	}
};

/* Class to represent disjoint sets for Kruskal's algorithm */
class DisjointSets
{
public://Node structure for disjoint set forest
	struct Node
	{
		int rank;
		int id;
		int position;
	};

	Node * sets;

	DisjointSets()
	{
	};

	~DisjointSets()
	{
		delete sets;
	}

	void makeSet(Node &n)
	{
		n.rank = 0;
	}

	/* Return representative of set */
	Node& findSet(Node &x)//Implements path compression heuristic
	{
		if (x.id != x.position)
			sets[x.position] = findSet(sets[x.position]);
		return sets[x.position];
	}

	void link(Node &x, Node &y)
	{
		if (x.rank > y.rank)
		{
			sets[y.position] = x;
			sets[y.position].position = x.position;
		}
		else
		{
			sets[x.position] = y;
			sets[x.position].position = y.position;
			if (x.rank == y.rank)
				y.rank++;
		}
	}

	/* Combine two sets */
	void Union(Node &x, Node &y)//Implements union by rank heuristic
	{
		link(findSet(x), findSet(y));
	}

	void buildSets(Graph * g)
	{
		int tmp = g->vertices.size();
		sets = new Node[tmp];
		for (int i = 0; i < tmp; i++)
		{
			Node newNode;
			makeSet(newNode);
			newNode.id = i;
			newNode.position = i;
			sets[i] = newNode;
		}
	}

	/* Kruskal's algorithm to find MST */
	vector<Edge> mstKruskal(Graph * g)
	{
		vector<Edge> a;
		for (int i = 0; i < g->sortedEdges.size(); i++)//for each edge in the list of sorted edges
		{
			if ((findSet(sets[g->sortedEdges[i].u->id - 1]).id != (findSet(sets[g->sortedEdges[i].v->id - 1]).id)))//if no cycle is created by adding edge a
			{
				a.push_back(g->sortedEdges[i]);
				Union((sets[g->sortedEdges[i].u->id - 1]), (sets[g->sortedEdges[i].v->id - 1]));
			}
		}
		return a;
	}
};


/* Function prototypes */
void readFile(string f, Graph * g);
void output(vector<Edge> edges, string fileName);


/* Begin Main*//////////////////////////////////
int main()
{
	string fileName = "graphin.txt";

	/* Build graph and perform Prim's */
	Graph * newGraph = new Graph;
	readFile(fileName, newGraph);
	newGraph->minHeap();
	newGraph->mstPrim();
	output(newGraph->primResult, "primout.txt");
	delete newGraph;

	/* Build graph and perform Kruskal's */
	Graph * newGraph2 = new Graph;
	readFile(fileName, newGraph2);
	newGraph2->heap();
	DisjointSets ds;
	ds.buildSets(newGraph2);
	vector<Edge> kruskal;
	kruskal = ds.mstKruskal(newGraph2);
	output(kruskal, "kruskalout.txt");
	delete newGraph2;

	system("PAUSE");
}
/* End Main *///////////////////////////////////


/* Function to output results to .txt file */
void output(vector<Edge> edges, string fileName)
{
	ofstream outputFile;
	outputFile.open(fileName);
	for (int i = 0; i < edges.size(); i++)
		outputFile << edges[i].u->id << " " << edges[i].v->id << endl;
	outputFile.close();
	outputFile.clear();
}

/* Function to read an input .txt file and build the graph */
void readFile(string f, Graph * g)
{
	ifstream in;
	in.open(f);
	assert(in.is_open());
	int i = 0;
	int adjacency;
	string line;
	char temp2;
	while (!in.eof())
	{
		Vertex newVertex;
		g->vertices.push_back(newVertex);
		in >> g->vertices[i].id >> temp2;//assign node id and discard colon into temp2
		getline(in, line);
		istringstream is(line);
		int temp = 0;
		while (is >> adjacency)//use string stream to parse ints from line of text
		{
			g->vertices[i].adjacencies.push_back(adjacency);
			Edge e;
			is >> e.weight;
			e.index = temp;//map each edge to its corresponding adjacency list representation's index
			temp++;
			g->edges.push_back(e);
		}
		line = "";
		i++;
		is.clear();
	}
	in.close();
	in.clear();
	int tmp = 0;
	for (int i = 0; i < g->vertices.size(); i++)//connect each edge's pointers to their respective vertexs
	{
		for (int j = 0; j < g->vertices[i].adjacencies.size(); j++)
		{
			g->edges[tmp].u = &(g->vertices[i]);
			g->edges[tmp].v = &(g->vertices[g->vertices[i].adjacencies[j] - 1]);
			g->vertices[i].adjacencyIndex.push_back(0);//
			tmp++;
		}
	}
}