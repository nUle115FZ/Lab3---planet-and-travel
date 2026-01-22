#ifndef STAR_GRAPH_H
#define STAR_GRAPH_H

#include "DynamicArray.h"
#include <string>
#include <unordered_map>

/**
 * данные планеты (звездной системы)
 */
struct PlanetData {
    std::string name;     //название планеты
    int id;               //уникальный идентификатор
    
    PlanetData();
    PlanetData(const std::string& planetName, int planetId);
};

/**
 * данные ребра графа
 * включает расстояние и риск (активность пиратов)
 */
struct EdgeData {
    double distance;      //физическое расстояние
    double riskFactor;    //фактор риска (0.0 - безопасно, 1.0 - очень опасно)
    
    EdgeData();
    EdgeData(double dist, double risk);
    
    //общая стоимость перехода (расстояние + штраф за риск)
    double getTotalCost() const;
    
    //операторы сравнения
    bool operator<(const EdgeData& other) const;
    bool operator>(const EdgeData& other) const;
    bool operator==(const EdgeData& other) const;
};

/**
 * структура ребра графа
 */
struct Edge {
    int from;         //исходная вершина
    int to;           //целевая вершина
    EdgeData data;    //данные ребра
    
    Edge();
    Edge(int source, int destination, EdgeData edgeData);
};

/**
 * ориентированный взвешенный граф для представления звездных систем
 * вершины - планеты, ребра - гиперпространственные переходы
 */
class StarGraph {
private:
    //список смежности: для каждой вершины хранится список исходящих ребер
    std::unordered_map<int, DynamicArray<Edge>> adjacencyList;
    
    //данные планет (звездных систем)
    std::unordered_map<int, PlanetData> planetData;
    
    //обратная карта: имя -> индекс
    std::unordered_map<std::string, int> nameToIndex;
    
    int vertexCount;
    int nextVertexId;

public:
    StarGraph();
    
    //добавление вершины (планеты) в граф
    int AddVertex(const std::string& name);
    
    //добавление ребра (гиперпространственного перехода)
    void AddEdge(int from, int to, const EdgeData& edgeData);
    void AddEdge(const std::string& fromName, const std::string& toName, const EdgeData& edgeData);
    
    //удаление вершины (планеты) из графа
    void RemoveVertex(int vertex);
    void RemoveVertex(const std::string& name);
    
    //удаление ребра (гиперпространственного перехода)
    void RemoveEdge(int from, int to);
    void RemoveEdge(const std::string& fromName, const std::string& toName);
    
    //обновление расстояния ребра (при перемещении планет)
    void UpdateEdgeDistance(int from, int to, double newDistance);
    
    //получение списка смежных вершин
    const DynamicArray<Edge>& GetEdges(int vertex) const;
    
    //получение индекса вершины по имени
    int GetVertexIndex(const std::string& name) const;
    
    //получение имени планеты по индексу
    std::string GetVertexName(int index) const;
    
    //получение данных планеты по индексу
    const PlanetData& GetPlanetData(int index) const;
    
    //проверка существования вершины
    bool HasVertex(int vertex) const;
    bool HasVertex(const std::string& name) const;
    
    //получение количества вершин
    int GetVertexCount() const;
    
    //получение всех вершин графа
    DynamicArray<int> GetAllVertices() const;
    
    //получение карты всех планет (индекс -> имя)
    const std::unordered_map<int, std::string>& GetAllPlanets() const;
    
    //очистка графа
    void Clear();
    
    //═══ работа с файлами ═══
    //сохранить граф в файл
    void SaveToFile(const std::string& filename) const;
    
    //загрузить граф из файла
    void LoadFromFile(const std::string& filename);
};

#endif //sTAR_GRAPH_H
