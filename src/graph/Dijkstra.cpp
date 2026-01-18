#include "Dijkstra.h"
#include <algorithm>

//dijkstraResult реализация
DijkstraResult::DijkstraResult() 
    : pathExists(false), totalCost(std::numeric_limits<double>::infinity()) {}

//dijkstra реализация
Dijkstra::Dijkstra(const StarGraph& g) : graph(g) {}

DynamicArray<int> Dijkstra::reconstructPath(const std::unordered_map<int, int>& previous, 
                                           int start, int end) const {
    DynamicArray<int> path;
    
    //если путь не существует
    if (previous.find(end) == previous.end() && start != end) {
        return path;
    }
    
    //восстанавливаем путь от конца к началу
    int current = end;
    while (current != start) {
        path.Prepend(current);
        auto it = previous.find(current);
        if (it == previous.end()) {
            //путь не существует
            path.Clear();
            return path;
        }
        current = it->second;
    }
    path.Prepend(start);
    
    return path;
}

DijkstraResult Dijkstra::FindShortestPath(int start, int end) const {
    DijkstraResult result;
    
    //проверка существования вершин
    if (!graph.HasVertex(start) || !graph.HasVertex(end)) {
        return result;
    }
    
    //инициализация расстояний
    std::unordered_map<int, double> distances;
    std::unordered_map<int, int> previous;
    std::unordered_map<int, bool> visited;
    
    //получаем все вершины графа
    DynamicArray<int> allVertices = graph.GetAllVertices();
    for (int i = 0; i < allVertices.GetSize(); i++) {
        int vertex = allVertices.Get(i);
        distances[vertex] = std::numeric_limits<double>::infinity();
        visited[vertex] = false;
    }
    
    distances[start] = 0.0;
    
    //очередь с приоритетом (приоритет = расстояние)
    PriorityQueue<int, double> pq;
    pq.Enqueue(start, 0.0);
    
    while (!pq.IsEmpty()) {
        int current = pq.Dequeue();
        
        //если уже посетили эту вершину, пропускаем
        if (visited[current]) {
            continue;
        }
        
        visited[current] = true;
        
        //если достигли конечной вершины, можем остановиться
        if (current == end) {
            break;
        }
        
        //проверяем всех соседей
        const DynamicArray<Edge>& edges = graph.GetEdges(current);
        for (int i = 0; i < edges.GetSize(); i++) {
            const Edge& edge = edges.Get(i);
            int neighbor = edge.to;
            double edgeCost = edge.data.getTotalCost();
            
            //релаксация ребра
            double newDistance = distances[current] + edgeCost;
            if (newDistance < distances[neighbor]) {
                distances[neighbor] = newDistance;
                previous[neighbor] = current;
                pq.Enqueue(neighbor, newDistance);
            }
        }
    }
    
    //формируем результат
    result.totalCost = distances[end];
    result.pathExists = (distances[end] != std::numeric_limits<double>::infinity());
    
    if (result.pathExists) {
        result.path = reconstructPath(previous, start, end);
        
        //заполняем имена планет
        for (int i = 0; i < result.path.GetSize(); i++) {
            result.pathNames.Append(graph.GetVertexName(result.path.Get(i)));
        }
    }
    
    return result;
}

DijkstraResult Dijkstra::FindShortestPath(const std::string& startName, 
                                         const std::string& endName) const {
    try {
        int start = graph.GetVertexIndex(startName);
        int end = graph.GetVertexIndex(endName);
        return FindShortestPath(start, end);
    } catch (const std::exception& e) {
        return DijkstraResult();
    }
}

std::unordered_map<int, double> Dijkstra::FindAllShortestPaths(int start) const {
    std::unordered_map<int, double> distances;
    std::unordered_map<int, bool> visited;
    
    //инициализация
    DynamicArray<int> allVertices = graph.GetAllVertices();
    for (int i = 0; i < allVertices.GetSize(); i++) {
        int vertex = allVertices.Get(i);
        distances[vertex] = std::numeric_limits<double>::infinity();
        visited[vertex] = false;
    }
    
    distances[start] = 0.0;
    
    //очередь с приоритетом
    PriorityQueue<int, double> pq;
    pq.Enqueue(start, 0.0);
    
    while (!pq.IsEmpty()) {
        int current = pq.Dequeue();
        
        if (visited[current]) {
            continue;
        }
        
        visited[current] = true;
        
        //обрабатываем всех соседей
        const DynamicArray<Edge>& edges = graph.GetEdges(current);
        for (int i = 0; i < edges.GetSize(); i++) {
            const Edge& edge = edges.Get(i);
            int neighbor = edge.to;
            double edgeCost = edge.data.getTotalCost();
            
            double newDistance = distances[current] + edgeCost;
            if (newDistance < distances[neighbor]) {
                distances[neighbor] = newDistance;
                pq.Enqueue(neighbor, newDistance);
            }
        }
    }
    
    return distances;
}


