#include "common.h"
#include <string>
#include <map>
#include <cstring>
#include <fstream>
#include <vector>
#include <iostream>
#include <set>

/**
 * @brief 
 * This structure will be a replacement for a Java Map. It will store names and scores
 * in order of names so that corresponding index numbers will give correct name, pair values
 * 
 * Mix of structs and C++ is owing to the last minute change of language to C++ for convenience of
 * handling graphs
 * 
 * In this header, vectors have been used while parsing file for easier working with maps instead
 * of C Arrays. Those were getting too complex and were marred with Segmentation faults and bus errors
 */
struct Scores {     
    std::vector<std::string> nameVector;
    std::vector<int> scoreVector;
};

/**
 * @brief 
 * Parsing input file and creating vector arrays
 * @param scores 
 * @return struct Scores 
 */
struct Scores populateScoreFromFile(Scores scores) {
    std::string name;
    int score;

    std::ifstream file(scoreFile);
    if (!file) {
        perror("ServerS: Couldn't find scores.txt");
        exit(1);
    }
    std::map<std::string, int> nameScoreMap;
    if (file.is_open()) {
        while (file >> name >> score) {
            nameScoreMap[name] = score;
        }
    }

    // Picked up the syntax online. Using C++ Map for the first time. A major reason why the C++ shift happened
    for (std::map<std::string, int>::iterator itr = nameScoreMap.begin(); itr != nameScoreMap.end(); itr++) {
        scores.nameVector.push_back(itr->first);
        scores.scoreVector.push_back(itr->second);
    }
    file.close();
    return scores;
}

/**
 * @brief
 * Populating name matches from the vector array
 * @param nameVector 
 * @param nodeList 
 * @param nodeCount 
 * @return std::vector<std::string> 
 */
std::vector<std::string> getNameMatches(std::vector<std::string> &nameVector, int *&nodeList, int nodeCount) {
    std::vector<std::string> nameList;
    for(int i = 0; i<nodeCount; i++) {
        nameList.push_back(nameVector[nodeList[i]]);
    }
    return nameList;
}

/**
 * @brief
 * Getting score matches from vector array
 * @param scoreVector 
 * @param nodeList 
 * @param nodeCount 
 * @return int* 
 */
int *getScoreMatches(std::vector<int> &scoreVector, int *&nodeList, int nodeCount) {
    int *scoreList = new int[nodeCount];
    for (int i = 0; i < nodeCount; i++) {
        scoreList[i] = scoreVector[nodeList[i]];
    }
    return scoreList;
}