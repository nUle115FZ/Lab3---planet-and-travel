#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include <QPoint>
#include <QMap>
#include <QStringList>
#include <QTimer>
#include "StarGraph.h"
#include "DynamicArray.h"
#include "Trader.h"
#include "visual_effects.h"

struct NodePosition {
    QPointF position;
    QString name;
    int id;
    bool isHighlighted;
    
    NodePosition() : id(-1), isHighlighted(false) {}
    NodePosition(const QPointF& pos, const QString& n, int i) 
        : position(pos), name(n), id(i), isHighlighted(false) {}
};

//структура метеорита (только если включены эффекты)
struct Meteor {
    QPointF position;
    QPointF velocity;
    int size;
    double opacity;
    
    Meteor() : size(2), opacity(0.6) {}
};

//структура звезды (статичная)
struct Star {
    QPointF position;
    int brightness;
    
    Star() : brightness(255) {}
    Star(const QPointF& pos, int b) : position(pos), brightness(b) {}
};

class GraphView : public QWidget
{
    Q_OBJECT

public:
    explicit GraphView(StarGraph* g, QWidget *parent = nullptr);
    
    void highlightPath(const DynamicArray<int>& path);
    void addLogMessage(const QString& message);
    void setTrader(Trader* t) { trader = t; }  //установить торговца

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimationStep();      //шаг анимации пути
    void onMeteorUpdate();       //обновление метеоритов

private:
    StarGraph* graph;
    Trader* trader;                      //торговец (персонаж)
    QMap<int, NodePosition> nodePositions;
    DynamicArray<int> highlightedPath;
    int selectedNode;
    bool isDragging;
    QPoint dragOffset;
    QStringList logMessages;
    
    //вспомогательные методы
    void updateEdgeDistances(int vertexId);  //пересчёт расстояний при перемещении планеты
    
    //═══ визуальные эффекты ═══
    //анимация пути
    QTimer* animationTimer;
    int animationStep;
    bool isAnimating;
    
    //метеориты
    QTimer* meteorTimer;
    QVector<Meteor> meteors;
    
    //статичные звезды (чтобы не мелькали)
    QVector<Star> stars;
    
    void updateNodePositions();
    void drawNode(QPainter& painter, const NodePosition& node, bool isHighlighted);
    void drawEdge(QPainter& painter, const QPointF& from, const QPointF& to, 
                  const EdgeData& data, bool isHighlighted);
    int getNodeAtPosition(const QPoint& pos);
    QPointF getRandomPosition();
    
    //═══ методы для эффектов ═══
    void initMeteors();          //инициализация метеоритов
    void initStars();            //инициализация звезд
    void drawMeteors(QPainter& painter);  //отрисовка метеоритов
    void drawStars(QPainter& painter);    //отрисовка звезд
    QColor getNodeColorByConnections(int nodeId);  //цвет планеты по связям
};

#endif //gRAPHVIEW_H

