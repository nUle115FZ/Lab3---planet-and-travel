#include "test_framework.h"
#include "../include/graph/StarGraph.h"
#include <stdexcept>

//===========================================
// тесты добавления вершин
//===========================================

TEST(AddVertex_SingleVertex_Success) {
    StarGraph graph;
    int id = graph.AddVertex("Терра");
    
    ASSERT_EQUAL(graph.GetVertexCount(), 1);
    ASSERT_TRUE(graph.HasVertex(id));
    ASSERT_TRUE(graph.HasVertex("Терра"));
    ASSERT_EQUAL(graph.GetVertexName(id), "Терра");
}

TEST(AddVertex_MultipleVertices_Success) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    int id3 = graph.AddVertex("Юпитер");
    
    ASSERT_EQUAL(graph.GetVertexCount(), 3);
    ASSERT_TRUE(graph.HasVertex(id1));
    ASSERT_TRUE(graph.HasVertex(id2));
    ASSERT_TRUE(graph.HasVertex(id3));
    ASSERT_TRUE(graph.HasVertex("Терра"));
    ASSERT_TRUE(graph.HasVertex("Марс"));
    ASSERT_TRUE(graph.HasVertex("Юпитер"));
}

TEST(AddVertex_DuplicateName_ThrowsException) {
    StarGraph graph;
    graph.AddVertex("Терра");
    
    ASSERT_THROWS(graph.AddVertex("Терра"));
}

TEST(AddVertex_GetVertexIndex_Success) {
    StarGraph graph;
    int id = graph.AddVertex("Терра");
    
    ASSERT_EQUAL(graph.GetVertexIndex("Терра"), id);
}

//===========================================
// тесты добавления ребер
//===========================================

TEST(AddEdge_SingleEdge_Success) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    
    EdgeData data(100.0, 0.2);
    graph.AddEdge(id1, id2, data);
    
    const DynamicArray<Edge>& edges = graph.GetEdges(id1);
    ASSERT_EQUAL(edges.GetSize(), 1);
    ASSERT_EQUAL(edges.Get(0).from, id1);
    ASSERT_EQUAL(edges.Get(0).to, id2);
    ASSERT_EQUAL(edges.Get(0).data.distance, 100.0);
    ASSERT_EQUAL(edges.Get(0).data.riskFactor, 0.2);
}

TEST(AddEdge_MultipleEdges_Success) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    int id3 = graph.AddVertex("Юпитер");
    
    graph.AddEdge(id1, id2, EdgeData(100.0, 0.1));
    graph.AddEdge(id1, id3, EdgeData(200.0, 0.2));
    
    const DynamicArray<Edge>& edges = graph.GetEdges(id1);
    ASSERT_EQUAL(edges.GetSize(), 2);
}

TEST(AddEdge_ByName_Success) {
    StarGraph graph;
    graph.AddVertex("Терра");
    graph.AddVertex("Марс");
    
    graph.AddEdge("Терра", "Марс", EdgeData(100.0, 0.1));
    
    int id1 = graph.GetVertexIndex("Терра");
    const DynamicArray<Edge>& edges = graph.GetEdges(id1);
    ASSERT_EQUAL(edges.GetSize(), 1);
}

TEST(AddEdge_NonexistentVertex_ThrowsException) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    
    ASSERT_THROWS(graph.AddEdge(id1, 999, EdgeData(100.0, 0.1)));
    ASSERT_THROWS(graph.AddEdge(999, id1, EdgeData(100.0, 0.1)));
}

TEST(AddEdge_NonexistentVertexByName_ThrowsException) {
    StarGraph graph;
    graph.AddVertex("Терра");
    
    ASSERT_THROWS(graph.AddEdge("Терра", "НесуществующаяПланета", EdgeData(100.0, 0.1)));
}

//===========================================
// тесты удаления вершин
//===========================================

TEST(RemoveVertex_SingleVertex_Success) {
    StarGraph graph;
    int id = graph.AddVertex("Терра");
    
    graph.RemoveVertex(id);
    
    ASSERT_EQUAL(graph.GetVertexCount(), 0);
    ASSERT_FALSE(graph.HasVertex(id));
    ASSERT_FALSE(graph.HasVertex("Терра"));
}

TEST(RemoveVertex_ByName_Success) {
    StarGraph graph;
    graph.AddVertex("Терра");
    
    graph.RemoveVertex("Терра");
    
    ASSERT_EQUAL(graph.GetVertexCount(), 0);
    ASSERT_FALSE(graph.HasVertex("Терра"));
}

TEST(RemoveVertex_WithIncomingEdges_Success) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    int id3 = graph.AddVertex("Юпитер");
    
    graph.AddEdge(id1, id2, EdgeData(100.0, 0.1));
    graph.AddEdge(id3, id2, EdgeData(200.0, 0.2));
    
    graph.RemoveVertex(id2);
    
    //проверяем что вершина удалена
    ASSERT_EQUAL(graph.GetVertexCount(), 2);
    ASSERT_FALSE(graph.HasVertex(id2));
    
    //проверяем что ребра к удаленной вершине тоже удалены
    const DynamicArray<Edge>& edges1 = graph.GetEdges(id1);
    const DynamicArray<Edge>& edges3 = graph.GetEdges(id3);
    ASSERT_EQUAL(edges1.GetSize(), 0);
    ASSERT_EQUAL(edges3.GetSize(), 0);
}

TEST(RemoveVertex_WithOutgoingEdges_Success) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    int id3 = graph.AddVertex("Юпитер");
    
    graph.AddEdge(id1, id2, EdgeData(100.0, 0.1));
    graph.AddEdge(id1, id3, EdgeData(200.0, 0.2));
    
    graph.RemoveVertex(id1);
    
    ASSERT_EQUAL(graph.GetVertexCount(), 2);
    ASSERT_FALSE(graph.HasVertex(id1));
}

TEST(RemoveVertex_Nonexistent_ThrowsException) {
    StarGraph graph;
    
    ASSERT_THROWS(graph.RemoveVertex(999));
    ASSERT_THROWS(graph.RemoveVertex("НесуществующаяПланета"));
}

//===========================================
// тесты удаления ребер
//===========================================

TEST(RemoveEdge_SingleEdge_Success) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    
    graph.AddEdge(id1, id2, EdgeData(100.0, 0.1));
    
    const DynamicArray<Edge>& edgesBefore = graph.GetEdges(id1);
    ASSERT_EQUAL(edgesBefore.GetSize(), 1);
    
    graph.RemoveEdge(id1, id2);
    
    const DynamicArray<Edge>& edgesAfter = graph.GetEdges(id1);
    ASSERT_EQUAL(edgesAfter.GetSize(), 0);
}

TEST(RemoveEdge_ByName_Success) {
    StarGraph graph;
    graph.AddVertex("Терра");
    graph.AddVertex("Марс");
    
    graph.AddEdge("Терра", "Марс", EdgeData(100.0, 0.1));
    graph.RemoveEdge("Терра", "Марс");
    
    int id1 = graph.GetVertexIndex("Терра");
    const DynamicArray<Edge>& edges = graph.GetEdges(id1);
    ASSERT_EQUAL(edges.GetSize(), 0);
}

TEST(RemoveEdge_MultipleEdges_RemovesOnlyOne) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    int id3 = graph.AddVertex("Юпитер");
    
    graph.AddEdge(id1, id2, EdgeData(100.0, 0.1));
    graph.AddEdge(id1, id3, EdgeData(200.0, 0.2));
    
    graph.RemoveEdge(id1, id2);
    
    const DynamicArray<Edge>& edges = graph.GetEdges(id1);
    ASSERT_EQUAL(edges.GetSize(), 1);
    ASSERT_EQUAL(edges.Get(0).from, id1);
    ASSERT_EQUAL(edges.Get(0).to, id3);
}

TEST(RemoveEdge_Nonexistent_ThrowsException) {
    StarGraph graph;
    int id1 = graph.AddVertex("Терра");
    int id2 = graph.AddVertex("Марс");
    
    //не добавляем ребро, пытаемся удалить - должно работать без ошибки
    graph.RemoveEdge(id1, id2);
    
    //но если вершина не существует - должна быть ошибка
    ASSERT_THROWS(graph.RemoveEdge(id1, 999));
    ASSERT_THROWS(graph.RemoveEdge(999, id1));
}

//===========================================
// тесты EdgeData
//===========================================

TEST(EdgeData_TotalCost_CalculatesCorrectly) {
    EdgeData data(100.0, 0.5);
    
    //стоимость = distance * (1 + riskFactor)
    //100 * (1 + 0.5) = 100 * 1.5 = 150
    ASSERT_EQUAL(data.getTotalCost(), 150.0);
}

TEST(EdgeData_TotalCost_ZeroRisk) {
    EdgeData data(100.0, 0.0);
    ASSERT_EQUAL(data.getTotalCost(), 100.0);
}

TEST(EdgeData_TotalCost_HighRisk) {
    EdgeData data(100.0, 1.0);
    ASSERT_EQUAL(data.getTotalCost(), 200.0);
}

TEST(EdgeData_Comparison_LessThan) {
    EdgeData data1(100.0, 0.0);  //cost = 100
    EdgeData data2(100.0, 0.5);  //cost = 150
    
    ASSERT_TRUE(data1 < data2);
    ASSERT_FALSE(data2 < data1);
}

//===========================================
// тесты PlanetData
//===========================================

TEST(PlanetData_Constructor_Success) {
    PlanetData planet("Терра", 0);
    
    ASSERT_EQUAL(planet.name, "Терра");
    ASSERT_EQUAL(planet.id, 0);
}

TEST(PlanetData_GetPlanetData_Success) {
    StarGraph graph;
    int id = graph.AddVertex("Терра");
    
    const PlanetData& planet = graph.GetPlanetData(id);
    ASSERT_EQUAL(planet.name, "Терра");
    ASSERT_EQUAL(planet.id, id);
}

//===========================================
// тесты Clear
//===========================================

TEST(Clear_EmptyGraph_Success) {
    StarGraph graph;
    graph.Clear();
    ASSERT_EQUAL(graph.GetVertexCount(), 0);
}

TEST(Clear_NonEmptyGraph_Success) {
    StarGraph graph;
    graph.AddVertex("Терра");
    graph.AddVertex("Марс");
    graph.AddVertex("Юпитер");
    
    graph.Clear();
    
    ASSERT_EQUAL(graph.GetVertexCount(), 0);
    ASSERT_FALSE(graph.HasVertex("Терра"));
    ASSERT_FALSE(graph.HasVertex("Марс"));
    ASSERT_FALSE(graph.HasVertex("Юпитер"));
}

//===========================================
// комплексные тесты
//===========================================

TEST(Complex_AddRemoveMultipleOperations) {
    StarGraph graph;
    
    //добавляем планеты
    int terra = graph.AddVertex("Терра");
    int mars = graph.AddVertex("Марс");
    int jupiter = graph.AddVertex("Юпитер");
    
    //добавляем ребра
    graph.AddEdge(terra, mars, EdgeData(100.0, 0.1));
    graph.AddEdge(mars, jupiter, EdgeData(200.0, 0.2));
    graph.AddEdge(terra, jupiter, EdgeData(300.0, 0.3));
    
    ASSERT_EQUAL(graph.GetVertexCount(), 3);
    ASSERT_EQUAL(graph.GetEdges(terra).GetSize(), 2);
    
    //удаляем одно ребро
    graph.RemoveEdge(terra, jupiter);
    ASSERT_EQUAL(graph.GetEdges(terra).GetSize(), 1);
    
    //удаляем планету
    graph.RemoveVertex(mars);
    ASSERT_EQUAL(graph.GetVertexCount(), 2);
    ASSERT_EQUAL(graph.GetEdges(terra).GetSize(), 0);
    
    //добавляем новую планету
    int saturn = graph.AddVertex("Сатурн");
    graph.AddEdge(terra, saturn, EdgeData(400.0, 0.4));
    
    ASSERT_EQUAL(graph.GetVertexCount(), 3);
    ASSERT_TRUE(graph.HasVertex("Сатурн"));
}

TEST(Complex_CycleDetection) {
    StarGraph graph;
    
    int id1 = graph.AddVertex("A");
    int id2 = graph.AddVertex("B");
    int id3 = graph.AddVertex("C");
    
    //создаем цикл: A → B → C → A
    graph.AddEdge(id1, id2, EdgeData(10.0, 0.0));
    graph.AddEdge(id2, id3, EdgeData(10.0, 0.0));
    graph.AddEdge(id3, id1, EdgeData(10.0, 0.0));
    
    //граф должен корректно работать с циклами
    ASSERT_EQUAL(graph.GetEdges(id1).GetSize(), 1);
    ASSERT_EQUAL(graph.GetEdges(id2).GetSize(), 1);
    ASSERT_EQUAL(graph.GetEdges(id3).GetSize(), 1);
    
    //удаляем вершину из цикла
    graph.RemoveVertex(id2);
    
    //остальные вершины должны остаться
    ASSERT_EQUAL(graph.GetVertexCount(), 2);
    ASSERT_TRUE(graph.HasVertex(id1));
    ASSERT_TRUE(graph.HasVertex(id3));
    
    //ребро A→B удалено (входит в B)
    //ребро B→C удалено (исходит из B)
    //ребро C→A остается (не связано с B напрямую)
    ASSERT_EQUAL(graph.GetEdges(id1).GetSize(), 0);  //A→B удалено
    ASSERT_EQUAL(graph.GetEdges(id3).GetSize(), 1);  //C→A остается
}

//главная функция
int main() {
    RUN_ALL_TESTS();
}

