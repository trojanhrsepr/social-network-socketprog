#include <algorithm>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

int minimumDistanceIndex(int nodeCount, double *dist, bool *vertexProcessed);
void findIntermediateNodes(int *parent, int target, std::vector<std::string> &nameList, std::vector<std::string> &intermediateNodes);
std::string returnOutputMessage(std::string src, std::string target, std::vector<std::string> &intermediateNodes, double score, bool reverse);

/**
 * @brief 
 * This function implements Dijkstra's algorithm heavily inspired by 
 * https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/ for implementation. Changes in
 * the algorithm have been attempted but overall functionality is similar.
 * 
 * @param nodeCount 
 * @param adjacencyMatrix 
 * @param nameList 
 * @param src 
 * @param target 
 * @return std::pair<std::string, std::string> 
 */
std::pair<std::string, std::string> findCompatibleMatch(int nodeCount, double **&adjacencyMatrix, std::vector<std::string> &nameList, int src, int target) {
    if (src == target) {
        printf("Invalid case both nodes can't be same\n");
        exit(1);
    }
    
    double *dist = new double[nodeCount];
    bool *vertexProcessed = new bool[nodeCount];

    // Shortest path tree
    int *parent = new int[nodeCount];

    // Initialize starting conditions for Dijkstra (Distance infinite and vertexProcessed false)
    for (int i = 0; i < nodeCount; i++) {
        parent[i] = -1;
        dist[i] = FLT_MAX;
        vertexProcessed[i] = false;
    }

    // Initialize source distance
    dist[src] = 0;
    int u = src;

    while (u >= 0) {
        // Mark the picked vertex as processed
        vertexProcessed[u] = true;

        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < nodeCount; v++)
            if (!vertexProcessed[v] && adjacencyMatrix[u][v] != 0 && dist[u] + adjacencyMatrix[u][v] < dist[v]) {
                parent[v] = u;
                dist[v] = dist[u] + adjacencyMatrix[u][v];
            }
        
        // Pick the minimum distance vertex from the set of vertices not yet processed. u is always equal to src in first iteration.
        u = minimumDistanceIndex(nodeCount, dist, vertexProcessed);
    }

    std::string clientAOutput;
    std::string clientBOutput;
    
    // If distance is max, no compatibility for both
    if (dist[target] == FLT_MAX) {
        clientAOutput = "Found no compatibility for " + nameList[src] + " and " + nameList[target] + ".\n";
        clientBOutput = "Found no compatibility for " + nameList[target] + " and " + nameList[src] + ".\n";
    }
    // If there is match, populate output string with intermediate nodes
    else {
        std::vector<std::string> intermediateNodes;
        findIntermediateNodes(parent, target, nameList, intermediateNodes);
        intermediateNodes.pop_back();
        
        clientAOutput = returnOutputMessage(nameList[src], nameList[target], intermediateNodes, dist[target], false);
        clientBOutput = returnOutputMessage(nameList[target], nameList[src], intermediateNodes, dist[target], true);
    }
    return std::make_pair(clientAOutput, clientBOutput);
}

/**
 * @brief 
 * Find index with minimum distance
 * @param nodeCount 
 * @param dist 
 * @param vertexProcessed 
 * @return int 
 */
int minimumDistanceIndex(int nodeCount, double *dist, bool *vertexProcessed) {
    // Start with minimum value and default negative index
    double min = FLT_MAX;
    int minimumIndex = -1;

    int i = 0;
    while(i < nodeCount) {
        if (dist[i] <= min && !vertexProcessed[i]) {
            min = dist[i];
            minimumIndex = i;
        }
        i++;
    };
    return minimumIndex;
}

/**
 * @brief 
 * Recursive algorithm to find intermediate nodes. Part of Dijkstra implementation
 * @param parent 
 * @param target 
 * @param nameList 
 * @param intermediateNodes 
 */
void findIntermediateNodes(int *parent, int target, std::vector<std::string> &nameList, std::vector<std::string> &intermediateNodes) {
    // Base Case : If j is source
    if (parent[target] == -1)
        return;
    findIntermediateNodes(parent, parent[target], nameList, intermediateNodes);
    intermediateNodes.push_back(nameList[target]);
    return;
}

/**
 * @brief 
 * Create a weighted score matrix for Dijkstra
 * 
 * @param nodeCount 
 * @param scores_list 
 * @param adjacencyMatrixStart 
 * @return double** 
 */
double **createAdjacencyMatrix(int nodeCount, int *&scores_list, int **&adjacencyMatrixStart) {
    double **adjacencyMatrixEnd = new double *[nodeCount];

    for (int i = 0; i < nodeCount; i++) {
        adjacencyMatrixEnd[i] = new double[nodeCount];
        for (int j = 0; j < nodeCount; j++)
        {
            if (adjacencyMatrixStart[i][j] == 0 || i == j) {
                adjacencyMatrixEnd[i][j] = 0;
            }
            else {
                double score = (abs(scores_list[i] - scores_list[j]) * 1.0) / (scores_list[i] + scores_list[j]);
                adjacencyMatrixEnd[i][j] = score;
            }
        }
    }

    // Print weighted matrix generated
    /* Removed as Piazza files displayed too many values
    for (int i = 0; i < nodeCount; i++) {
        for (int j = 0; j < nodeCount; j++) {
            printf("%.2f ", adjacencyMatrixEnd[i][j]);
        }
        printf("\n");
    }*/
    return adjacencyMatrixEnd;
}

template<class T>
std::string toString(const T &value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

// ToDo: Add reverse case for other client
/**
 * @brief 
 * Return output message string for Client A and Client B
 * @param src 
 * @param target 
 * @param intermediateNodes 
 * @param score 
 * @param reverse 
 * @return std::string 
 */
std::string returnOutputMessage(std::string src, std::string target, std::vector<std::string> &intermediateNodes, double score, bool reverse)
{
    std::string outputMessage = "Found compatibility for " + src + " and " + target + ":\n";
    outputMessage.append(src);
    outputMessage.append(" --- ");

    if (reverse)
        std::reverse(intermediateNodes.begin(), intermediateNodes.end());

    for (int i = 0; i < intermediateNodes.size(); i++) {
        outputMessage.append(intermediateNodes[i]);
        outputMessage.append(" --- ");
    }
    outputMessage.append(target + "\n");
    std::string valueStr = toString(score);
    outputMessage.append("Matching gap: " + valueStr + "\n");

    return outputMessage;
}