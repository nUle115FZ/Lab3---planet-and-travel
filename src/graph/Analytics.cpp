#include "Analytics.h"
#include <chrono>
#include <iostream>

//performanceResult реализация
PerformanceResult::PerformanceResult() 
    : graphSize(0), edgeCount(0), executionTime(0.0), pathFound(false), pathCost(0.0) {}

//analytics реализация
Analytics::Analytics() : gen(rd()) {}

StarGraph Analytics::generateRandomGraph(int vertexCount, int edgeCount) {
    StarGraph graph;
    
    //добавляем вершины
    for (int i = 0; i < vertexCount; i++) {
        std::string planetName = "Planet_" + std::to_string(i);
        graph.AddVertex(planetName);
    }
    
    //распределения для генерации весов
    std::uniform_int_distribution<> vertexDist(0, vertexCount - 1);
    std::uniform_real_distribution<> distanceDist(10.0, 1000.0);
    std::uniform_real_distribution<> riskDist(0.0, 0.5);
    
    //добавляем случайные ребра
    int addedEdges = 0;
    int attempts = 0;
    int maxAttempts = edgeCount * 10; //защита от бесконечного цикла
    
    while (addedEdges < edgeCount && attempts < maxAttempts) {
        int from = vertexDist(gen);
        int to = vertexDist(gen);
        
        //избегаем петель
        if (from != to) {
            double distance = distanceDist(gen);
            double risk = riskDist(gen);
            
            try {
                graph.AddEdge(from, to, EdgeData(distance, risk));
                addedEdges++;
            } catch (...) {
                //ребро уже существует или другая ошибка
            }
        }
        attempts++;
    }
    
    return graph;
}

PerformanceResult Analytics::measurePerformance(const StarGraph& graph, 
                                               int startVertex, int endVertex) {
    PerformanceResult result;
    result.graphSize = graph.GetVertexCount();
    
    //подсчет количества ребер
    int edgeCount = 0;
    DynamicArray<int> vertices = graph.GetAllVertices();
    for (int i = 0; i < vertices.GetSize(); i++) {
        edgeCount += graph.GetEdges(vertices.Get(i)).GetSize();
    }
    result.edgeCount = edgeCount;
    
    //замер времени
    auto start = std::chrono::high_resolution_clock::now();
    
    Dijkstra dijkstra(graph);
    DijkstraResult pathResult = dijkstra.FindShortestPath(startVertex, endVertex);
    
    auto end = std::chrono::high_resolution_clock::now();
    
    //вычисляем время в микросекундах
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    result.executionTime = duration.count();
    
    result.pathFound = pathResult.pathExists;
    result.pathCost = pathResult.totalCost;
    
    return result;
}

DynamicArray<PerformanceResult> Analytics::runPerformanceTests(
    const DynamicArray<int>& graphSizes,
    int edgesMultiplier,
    int runsPerSize
) {
    DynamicArray<PerformanceResult> results;
    
    std::cout << "Запуск тестов производительности...\n" << std::endl;
    
    for (int i = 0; i < graphSizes.GetSize(); i++) {
        int size = graphSizes.Get(i);
        int edges = size * edgesMultiplier;
        
        std::cout << "Тестирование графа размера " << size 
                  << " (с ~" << edges << " ребрами)..." << std::endl;
        
        //запускаем несколько тестов для усреднения
        double totalTime = 0.0;
        int successfulRuns = 0;
        
        for (int run = 0; run < runsPerSize; run++) {
            //генерируем случайный граф
            StarGraph testGraph = generateRandomGraph(size, edges);
            
            //выбираем случайные начальную и конечную вершины
            std::uniform_int_distribution<> vertexDist(0, size - 1);
            int start = vertexDist(gen);
            int end = vertexDist(gen);
            
            //измеряем производительность
            PerformanceResult result = measurePerformance(testGraph, start, end);
            
            if (result.executionTime > 0) {
                totalTime += result.executionTime;
                successfulRuns++;
            }
        }
        
        //сохраняем средний результат
        if (successfulRuns > 0) {
            PerformanceResult avgResult;
            avgResult.graphSize = size;
            avgResult.edgeCount = edges;
            avgResult.executionTime = totalTime / successfulRuns;
            results.Append(avgResult);
            
            std::cout << "  Среднее время: " << avgResult.executionTime 
                      << " мкс" << std::endl;
        }
    }
    
    std::cout << "\nТестирование завершено!" << std::endl;
    return results;
}


