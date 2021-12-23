#include <string>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <iostream>
#include <queue>

/**
 * @brief 
 * This file uses Geeks for Geeks link: https://www.geeksforgeeks.org/bfs-using-vectors-queue-per-algorithm-clrs/ and
 * https://www.geeksforgeeks.org/breadth-first-search-or-bfs-for-a-graph/ as reference.
 * 
 * BFS Sample implementations using vectors was picked up due to familiarity of handling data type earlier.
 * Though the class implementations for Graphs is similar, function definitions have been tweaked to serve purpose of this 
 * particular problem.
 */

/**
 * @brief 
 * Populate name and scores from file saved
 * @return std::map<std::string, int> 
 */
std::map<std::string, int>  populateNameScoreMapFromFile() {
    std::ifstream file(edgeFile);
    std::string firstName, secondName;
    if (!file) {
        perror("ServerT: Couldn't find edgelist.txt");
        exit(1);
    }
    std::set<std::string> sortedNames;
    if (file.is_open()) {
        while (file >> firstName >> secondName) {
            sortedNames.insert(firstName);
            sortedNames.insert(secondName);
        }
    }
    int count = 0;
    std::map<std::string, int> nameScoreMap;
    
    for (std::set<std::string>::iterator itr = sortedNames.begin(); itr != sortedNames.end(); itr++) {
        nameScoreMap[*itr] = count++;
    }
    file.close();
    return nameScoreMap;
}

class Graph {
    public:
        // Initialise graph
        Graph(int);

        int vertices;
        std::vector<int> *adj;

        // Functions for graphs
        void addEdge(int, int);
        std::vector<int> bfsImplementation(int, int, int[]);
};

/**
 * @brief Construct a new Graph:: Graph object
 * 
 * @param V 
 */
Graph::Graph(int V)
{
    this->vertices = V;
    adj = new std::vector<int>[V + 1];
}

/**
 * @brief 
 * Adding new edge to the graph
 * @param u 
 * @param v 
 */
void Graph::addEdge(int u, int v)
{
    adj[u].push_back(v);
    adj[v].push_back(u);
}

/**
 * @brief Create a Graph From File object
 * 
 * @param nameScoreMap 
 * @return Graph* 
 */
Graph *createGraphFromFile(std::map<std::string, int> nameScoreMap) {
    
    // Initialize new graph
    Graph *networkGraph = new Graph(nameScoreMap.size());
    if (nameScoreMap.size() == 0) {
        perror("ServerT: Name List not populated yet check file I/O");
        exit(1);
    }

    // Read file and add edges from it
    std::ifstream file(edgeFile);
    std::string firstName, secondName;
    if (file.is_open()) {
        while (file >> firstName >> secondName) {
            networkGraph->addEdge(nameScoreMap[firstName], nameScoreMap[secondName]);
        }
    }
    file.close();

    // Sorting each node in graph
    for (int i = 0; i < networkGraph->vertices; i++) {
        std::sort(networkGraph->adj[i].begin(), networkGraph->adj[i].end());
    }
    return networkGraph;
}

/**
 * @brief 
 * Interpolation search used for finding ID in Nodes list. Was covering this as part of a 
 * Leetcode pursuit! 
 * 
 * @param nameList 
 * @param hi 
 * @param key 
 * @return int 
 */
int searchForIdNodesList(int *&nameList, int lo ,int hi, int key) {
    int pos;
     
    // Since array is sorted, an element present in array must be in range defined by corner
    if (lo <= hi && key >= nameList[lo] && key <= nameList[hi]) {
    
        // Probing the position with keeping uniform distribution in mind
        pos = lo + (((hi - lo) / (nameList[hi] - nameList[lo])) * (key - nameList[lo]));
 
        // Element found
        if(nameList[pos] == key)
        return pos;
         
        // Key is right sub array
        if(nameList[pos] < key)
            return searchForIdNodesList(nameList, pos + 1, hi, key);
         
        // Key in left sub array
        if(nameList[pos] > key)
            return searchForIdNodesList(nameList, lo, pos - 1, key);
    }
    return -1;
}

/**
 * @brief 
 * Graph BFS Implementation main code
 * @param componentNum 
 * @param src 
 * @param visited 
 * @return std::vector<int> 
 */
std::vector<int> Graph::bfsImplementation(int componentNum, int src,int visited[]) {
    // BFS queue
    std::queue<int> queue;

    // Push source
    queue.push(src);

    // Assign Component Number
    visited[src] = componentNum;

    // Vector to store all the reachable nodes from 'src'
    std::vector<int> reachableNodes;

    while (!queue.empty())
    {
        // Dequeue a vertex from queue
        int u = queue.front();
        queue.pop();

        reachableNodes.push_back(u);

        // Get all adjacent vertices of the dequeued
        // vertex u. If a adjacent has not been visited,
        // then mark it visited nd enqueue it
        for (std::vector<int>::iterator itr = adj[u].begin();
             itr != adj[u].end(); itr++)
        {
            if (!visited[*itr])
            {
                // Assign Component Number to all the
                // reachable nodes
                visited[*itr] = componentNum;
                queue.push(*itr);
            }
        }
    }
    return reachableNodes;
}

/**
 * @brief 
 * Find all reachable nodes to the element
 * @param node 
 * @param networkGraph 
 * @param nameScoreMap 
 * @return std::vector<int> 
 */
std::vector<int> findReachableNodes(std::string node, Graph *networkGraph, std::map<std::string, int> nameScoreMap) {
    
    // Get the number of nodes in the graph
    int V = networkGraph->vertices;

    // Initialize all elements
    int visited[V + 1];
    memset(visited, 0, sizeof(visited));
    std::vector<int> temp;
    int componentNum = 0;

    // Find node in Network
    if (nameScoreMap.find(node) == nameScoreMap.end())
        return temp;
    int nodeID = nameScoreMap[node];

    // Visit all child nodes and add to graph
    if (!visited[nodeID]) {
        componentNum++;
        temp = networkGraph->bfsImplementation(componentNum, nodeID, visited);
    }
    return temp;
}

/**
 * @brief Made an error while defining this function by myself.
 * Used a sample snippet from StackOverFlow to solve this issue.
 * 
 * ToDo: Modify function to use switch case or a reduced for loop logic
 * @param adjcencyList 
 * @param nameList 
 * @param nodeCount 
 * @return int** 
 */
int **convertVectorAdjecencyListToMatrix(std::vector<int> *adjcencyList, int *&nameList, int nodeCount) {   
    int **adjacencyMatrix = new int *[nodeCount];
    for (int i = 0; i < nodeCount; i++) {
        adjacencyMatrix[i] = new int[nodeCount];
    }
    for (int k = 0; k < nodeCount; k++) {
        int nodeID = nameList[k]; //This is the actual value of the node which is equal to its index in the adjacency list
        int i = 0; int j = 0;
        while (j < nodeCount) {
            if (k == j) {
                adjacencyMatrix[k][j] = 1;
                j++;
            }
            else if (i == adjcencyList[nodeID].size()) {
                adjacencyMatrix[k][j] = 0;
                j++;
            }
            else if (nameList[j] == adjcencyList[nodeID][i]) {
                adjacencyMatrix[k][j] = 1;
                j++;
                i++;
            }
            else if (nameList[j] < adjcencyList[nodeID][i]){
                adjacencyMatrix[k][j] = 0;
                j++;
            }
            else
                i++;
        }
    }
    return adjacencyMatrix;
}