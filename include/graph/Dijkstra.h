#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "StarGraph.h"
#include "PriorityQueue.h"
#include "DynamicArray.h"
#include <unordered_map>
#include <limits>
#include <string>

/**
 * Результат работы алгоритма Дейкстры
 */
struct DijkstraResult {
    bool pathExists;                    //существует ли путь
    double totalCost;                   //общая стоимость пути
    DynamicArray<int> path;            //последовательность вершин в пути
    DynamicArray<std::string> pathNames; //названия планет в пути
    
    DijkstraResult();
};

/**
 * Класс для работы с алгоритмом Дейкстры
 */
class Dijkstra {
private:
    const StarGraph& graph;
    
    //восстановление пути из карты предшественников
    DynamicArray<int> reconstructPath(const std::unordered_map<int, int>& previous, 
                                     int start, int end) const;

public:
    explicit Dijkstra(const StarGraph& g);
    
    //поиск кратчайшего пути между двумя вершинами
    DijkstraResult FindShortestPath(int start, int end) const;
    
    //поиск кратчайшего пути по именам планет
    DijkstraResult FindShortestPath(const std::string& startName, const std::string& endName) const;
    
    //поиск всех кратчайших путей от заданной вершины
    std::unordered_map<int, double> FindAllShortestPaths(int start) const;
};

#endif //dIJKSTRA_H
