#include "StarGraph.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

//planetData реализация
PlanetData::PlanetData() : name(""), id(-1), hasArtifact(false) {}

PlanetData::PlanetData(const std::string& planetName, int planetId, bool artifact) 
    : name(planetName), id(planetId), hasArtifact(artifact) {}

//edgeData реализация
EdgeData::EdgeData() : distance(0.0), riskFactor(0.0) {}

EdgeData::EdgeData(double dist, double risk) : distance(dist), riskFactor(risk) {}

double EdgeData::getTotalCost() const {
    return distance * (1.0 + riskFactor);
}

bool EdgeData::operator<(const EdgeData& other) const {
    return getTotalCost() < other.getTotalCost();
}

bool EdgeData::operator>(const EdgeData& other) const {
    return getTotalCost() > other.getTotalCost();
}

bool EdgeData::operator==(const EdgeData& other) const {
    return distance == other.distance && riskFactor == other.riskFactor;
}

//edge реализация
Edge::Edge() : from(-1), to(-1) {}

Edge::Edge(int source, int destination, EdgeData edgeData) 
    : from(source), to(destination), data(edgeData) {}

//starGraph реализация
StarGraph::StarGraph() : vertexCount(0), nextVertexId(0) {}

int StarGraph::AddVertex(const std::string& name) {
    //проверка на дубликат имени
    if (nameToIndex.find(name) != nameToIndex.end()) {
        throw std::invalid_argument("Planet with this name already exists");
    }
    
    int id = nextVertexId++;
    planetData[id] = PlanetData(name, id);
    nameToIndex[name] = id;
    adjacencyList[id] = DynamicArray<Edge>();
    vertexCount++;
    
    return id;
}

void StarGraph::AddEdge(int from, int to, const EdgeData& edgeData) {
    if (adjacencyList.find(from) == adjacencyList.end()) {
        throw std::invalid_argument("Source vertex does not exist");
    }
    if (adjacencyList.find(to) == adjacencyList.end()) {
        throw std::invalid_argument("Destination vertex does not exist");
    }
    
    adjacencyList[from].Append(Edge(from, to, edgeData));
}

void StarGraph::AddEdge(const std::string& fromName, const std::string& toName, const EdgeData& edgeData) {
    if (nameToIndex.find(fromName) == nameToIndex.end()) {
        throw std::invalid_argument("Source planet not found: " + fromName);
    }
    if (nameToIndex.find(toName) == nameToIndex.end()) {
        throw std::invalid_argument("Destination planet not found: " + toName);
    }
    
    AddEdge(nameToIndex.at(fromName), nameToIndex.at(toName), edgeData);
}

void StarGraph::RemoveVertex(int vertex) {
    if (!HasVertex(vertex)) {
        throw std::invalid_argument("Vertex does not exist");
    }
    
    //получаем имя планеты для удаления из nameToIndex
    std::string vertexName = GetVertexName(vertex);
    
    //удаляем все ребра, которые ведут к этой вершине
    for (auto& pair : adjacencyList) {
        DynamicArray<Edge>& edges = pair.second;
        //создаем новый массив без ребер к удаляемой вершине
        DynamicArray<Edge> newEdges;
        for (int i = 0; i < edges.GetSize(); i++) {
            if (edges.Get(i).to != vertex) {
                newEdges.Append(edges.Get(i));
            }
        }
        pair.second = newEdges;
    }
    
    //удаляем саму вершину и её исходящие ребра
    adjacencyList.erase(vertex);
    planetData.erase(vertex);
    nameToIndex.erase(vertexName);
    vertexCount--;
}

void StarGraph::RemoveVertex(const std::string& name) {
    if (!HasVertex(name)) {
        throw std::invalid_argument("Planet not found: " + name);
    }
    RemoveVertex(nameToIndex.at(name));
}

void StarGraph::RemoveEdge(int from, int to) {
    if (!HasVertex(from)) {
        throw std::invalid_argument("Source vertex does not exist");
    }
    if (!HasVertex(to)) {
        throw std::invalid_argument("Destination vertex does not exist");
    }
    
    //создаем новый массив без удаляемого ребра
    DynamicArray<Edge>& edges = adjacencyList[from];
    DynamicArray<Edge> newEdges;
    
    for (int i = 0; i < edges.GetSize(); i++) {
        if (edges.Get(i).to != to) {
            newEdges.Append(edges.Get(i));
        }
    }
    
    adjacencyList[from] = newEdges;
}

void StarGraph::RemoveEdge(const std::string& fromName, const std::string& toName) {
    if (!HasVertex(fromName)) {
        throw std::invalid_argument("Source planet not found: " + fromName);
    }
    if (!HasVertex(toName)) {
        throw std::invalid_argument("Destination planet not found: " + toName);
    }
    RemoveEdge(nameToIndex.at(fromName), nameToIndex.at(toName));
}

void StarGraph::UpdateEdgeDistance(int from, int to, double newDistance) {
    //проверяем существование вершин
    if (adjacencyList.find(from) == adjacencyList.end()) {
        throw std::invalid_argument("Source vertex does not exist");
    }
    if (adjacencyList.find(to) == adjacencyList.end()) {
        throw std::invalid_argument("Destination vertex does not exist");
    }
    
    //минимальное расстояние = 1.0
    if (newDistance < 1.0) {
        newDistance = 1.0;
    }
    
    //ищем ребро и обновляем его расстояние
    DynamicArray<Edge>& edges = adjacencyList[from];
    for (int i = 0; i < edges.GetSize(); i++) {
        if (edges[i].to == to) {
            edges[i].data.distance = newDistance;
            return;
        }
    }
    
    //ребро не найдено - это нормально, может его просто нет
}

const DynamicArray<Edge>& StarGraph::GetEdges(int vertex) const {
    auto it = adjacencyList.find(vertex);
    if (it == adjacencyList.end()) {
        throw std::invalid_argument("Vertex does not exist");
    }
    return it->second;
}

int StarGraph::GetVertexIndex(const std::string& name) const {
    auto it = nameToIndex.find(name);
    if (it == nameToIndex.end()) {
        throw std::invalid_argument("Planet not found: " + name);
    }
    return it->second;
}

std::string StarGraph::GetVertexName(int index) const {
    auto it = planetData.find(index);
    if (it == planetData.end()) {
        throw std::invalid_argument("Vertex index does not exist");
    }
    return it->second.name;
}

const PlanetData& StarGraph::GetPlanetData(int index) const {
    auto it = planetData.find(index);
    if (it == planetData.end()) {
        throw std::invalid_argument("Vertex index does not exist");
    }
    return it->second;
}

PlanetData& StarGraph::GetPlanetDataMutable(int index) {
    auto it = planetData.find(index);
    if (it == planetData.end()) {
        throw std::invalid_argument("Vertex index does not exist");
    }
    return it->second;
}

void StarGraph::SetArtifact(int vertex, bool hasArtifact) {
    if (!HasVertex(vertex)) {
        throw std::invalid_argument("Vertex does not exist");
    }
    planetData[vertex].hasArtifact = hasArtifact;
}

bool StarGraph::HasArtifact(int vertex) const {
    auto it = planetData.find(vertex);
    if (it == planetData.end()) {
        return false;
    }
    return it->second.hasArtifact;
}

bool StarGraph::HasVertex(int vertex) const {
    return adjacencyList.find(vertex) != adjacencyList.end();
}

bool StarGraph::HasVertex(const std::string& name) const {
    return nameToIndex.find(name) != nameToIndex.end();
}

int StarGraph::GetVertexCount() const {
    return vertexCount;
}

DynamicArray<int> StarGraph::GetAllVertices() const {
    DynamicArray<int> vertices;
    for (const auto& pair : adjacencyList) {
        vertices.Append(pair.first);
    }
    return vertices;
}

const std::unordered_map<int, std::string>& StarGraph::GetAllPlanets() const {
    //временная карта для совместимости
    static std::unordered_map<int, std::string> namesMap;
    namesMap.clear();
    for (const auto& pair : planetData) {
        namesMap[pair.first] = pair.second.name;
    }
    return namesMap;
}

void StarGraph::Clear() {
    adjacencyList.clear();
    planetData.clear();
    nameToIndex.clear();
    vertexCount = 0;
    nextVertexId = 0;
}

//═══════════════════════════════════════════════════════════
//работа с файлами
//═══════════════════════════════════════════════════════════

void StarGraph::SaveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("не удалось открыть файл для записи: " + filename);
    }
    
    //═══ формат файла ═══
    //# комментарий
    //[PLANETS]
    //имя_планеты
    //...
    //[EDGES]
    //от к расстояние риск
    //...
    
    file << "# Cosmic Logistics - Star Graph File\n";
    file << "# Format: [PLANETS] section with planet names, [EDGES] section with transitions\n";
    file << "\n";
    
    //секция планет
    file << "[PLANETS]\n";
    for (const auto& pair : planetData) {
        file << pair.second.name << "\n";
    }
    file << "\n";
    
    //секция рёбер
    file << "[EDGES]\n";
    file << "# from to distance risk\n";
    for (const auto& pair : adjacencyList) {
        int fromVertex = pair.first;
        const DynamicArray<Edge>& edges = pair.second;
        
        for (int i = 0; i < edges.GetSize(); i++) {
            const Edge& edge = edges[i];
            std::string fromName = GetVertexName(fromVertex);
            std::string toName = GetVertexName(edge.to);
            
            file << fromName << " " 
                 << toName << " " 
                 << std::fixed << std::setprecision(2) << edge.data.distance << " "
                 << edge.data.riskFactor << "\n";
        }
    }
    
    file.close();
}

void StarGraph::LoadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("не удалось открыть файл для чтения: " + filename);
    }
    
    //очищаем текущий граф
    Clear();
    
    std::string line;
    enum class Section { NONE, PLANETS, EDGES };
    Section currentSection = Section::NONE;
    
    while (std::getline(file, line)) {
        //убираем пробелы с краёв
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        //пропускаем пустые строки и комментарии
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        //проверяем секции
        if (line == "[PLANETS]") {
            currentSection = Section::PLANETS;
            continue;
        } else if (line == "[EDGES]") {
            currentSection = Section::EDGES;
            continue;
        }
        
        //обрабатываем данные в зависимости от секции
        if (currentSection == Section::PLANETS) {
            //добавляем планету
            AddVertex(line);
            
        } else if (currentSection == Section::EDGES) {
            //парсим ребро: "от к расстояние риск"
            //имена могут содержать пробелы, поэтому используем более сложный парсинг
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;
            
            while (iss >> token) {
                tokens.push_back(token);
            }
            
            //нужно минимум 4 токена: имя1 имя2 расстояние риск
            if (tokens.size() >= 4) {
                //последние 2 токена - это числа (расстояние и риск)
                double distance = std::stod(tokens[tokens.size() - 2]);
                double risk = std::stod(tokens[tokens.size() - 1]);
                
                //все остальные токены делим пополам для двух имён
                //предполагаем что имена разделены одним пробелом
                //и оба имени занимают равное количество токенов
                int nameTokenCount = (tokens.size() - 2) / 2;
                
                std::string fromName;
                for (int i = 0; i < nameTokenCount; i++) {
                    if (i > 0) fromName += " ";
                    fromName += tokens[i];
                }
                
                std::string toName;
                for (size_t i = nameTokenCount; i < tokens.size() - 2; i++) {
                    if (i > static_cast<size_t>(nameTokenCount)) toName += " ";
                    toName += tokens[i];
                }
                
                try {
                    AddEdge(fromName, toName, EdgeData(distance, risk));
                } catch (const std::exception& e) {
                    //если не удалось добавить ребро - пропускаем
                    //но не прерываем загрузку
                }
            }
        }
    }
    
    file.close();
}


