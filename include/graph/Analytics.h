#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "StarGraph.h"
#include "Dijkstra.h"
#include <random>
#include <string>

//Результат измерения производительности
struct PerformanceResult {
    int graphSize;          //количество вершин
    int edgeCount;          //количество ребер
    double executionTime;   //время выполнения в микросекундах
    bool pathFound;         //найден ли путь
    double pathCost;        //стоимость найденного пути
    
    PerformanceResult();
};


 //Класс для аналитики и тестирования производительности
class Analytics {
private:
    std::random_device rd;
    std::mt19937 gen;
    
    //генерация случайного графа
    StarGraph generateRandomGraph(int vertexCount, int edgeCount);

public:
    Analytics();
    
    //измерение времени выполнения алгоритма Дейкстры
    PerformanceResult measurePerformance(const StarGraph& graph, int startVertex, int endVertex);
    
    //запуск серии тестов производительности на графах разного размера
    DynamicArray<PerformanceResult> runPerformanceTests(
        const DynamicArray<int>& graphSizes,
        int edgesMultiplier = 3,
        int runsPerSize = 5
    );
};

#endif //aNALYTICS_H
