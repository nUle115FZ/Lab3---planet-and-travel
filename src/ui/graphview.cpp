#include "graphview.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QtMath>
#include <QFont>
#include <QTimer>
#include <QMessageBox>
#include <cmath>
#include <cstdlib>
#include <algorithm>

GraphView::GraphView(StarGraph* g, QWidget *parent)
    : QWidget(parent), graph(g), trader(nullptr), selectedNode(-1), isDragging(false),
      animationStep(0), isAnimating(false), blackHole(nullptr)
{
    setMinimumSize(800, 600);
    setMouseTracking(true);
    
    //инициализируем статичные звезды (один раз)
    initStars();
    
    //═══ инициализация эффектов (только если включены) ═══
    if constexpr (ENABLE_VISUAL_EFFECTS) {
        //таймер для анимации пути
        if constexpr (ENABLE_PATH_ANIMATION) {
            animationTimer = new QTimer(this);
            connect(animationTimer, &QTimer::timeout, this, &GraphView::onAnimationStep);
        }
        
        //таймер для метеоритов
        if constexpr (ENABLE_METEORS) {
            meteorTimer = new QTimer(this);
            connect(meteorTimer, &QTimer::timeout, this, &GraphView::onMeteorUpdate);
            initMeteors();
            meteorTimer->start(METEOR_UPDATE_INTERVAL_MS);
        }
    }
    
    //═══ инициализация черной дыры ═══
    //таймер спавна (60 секунд)
    blackHoleSpawnTimer = new QTimer(this);
    connect(blackHoleSpawnTimer, &QTimer::timeout, this, &GraphView::onBlackHoleSpawn);
    blackHoleSpawnTimer->start(60000);  //60000 мс = 60 сек
    
    //таймер обновления позиции (50 мс)
    blackHoleUpdateTimer = new QTimer(this);
    connect(blackHoleUpdateTimer, &QTimer::timeout, this, &GraphView::onBlackHoleUpdate);
    blackHoleUpdateTimer->start(16);  //16 мс = ~60 FPS
}

void GraphView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    //фон - космос
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(10, 10, 30));
    gradient.setColorAt(1, QColor(20, 20, 50));
    painter.fillRect(rect(), gradient);
    
    //═══ метеориты на фоне (если включены) ═══
    if constexpr (ENABLE_VISUAL_EFFECTS && ENABLE_METEORS) {
        drawMeteors(painter);
    }
    
    //рисуем статичные звезды на фоне
    drawStars(painter);
    
    if (graph->GetVertexCount() == 0) {
        //показываем подсказку, если граф пуст
        painter.setPen(QColor(150, 150, 200));
        QFont font = painter.font();
        font.setPointSize(16);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, 
            "Граф пуст\n\nДобавьте планеты для начала работы");
        
        //показываем лог
        if (!logMessages.isEmpty()) {
            painter.setPen(QColor(200, 200, 220));
            font.setPointSize(10);
            painter.setFont(font);
            
            int y = height() - 20;
            for (int i = logMessages.size() - 1; i >= 0 && i >= logMessages.size() - 5; i--) {
                painter.drawText(10, y, logMessages[i]);
                y -= 20;
            }
        }
        
        return;
    }
    
    updateNodePositions();
    
    //рисуем ребра
    DynamicArray<int> vertices = graph->GetAllVertices();
    for (int i = 0; i < vertices.GetSize(); i++) {
        int fromId = vertices.Get(i);
        if (!nodePositions.contains(fromId)) continue;
        
        const DynamicArray<Edge>& edges = graph->GetEdges(fromId);
        for (int j = 0; j < edges.GetSize(); j++) {
            const Edge& edge = edges.Get(j);
            int toId = edge.to;
            
            if (!nodePositions.contains(toId)) continue;
            
            //проверяем, является ли ребро частью подсвеченного пути
            bool isHighlighted = false;
            for (int k = 0; k < highlightedPath.GetSize() - 1; k++) {
                if (highlightedPath.Get(k) == fromId && 
                    highlightedPath.Get(k + 1) == toId) {
                    isHighlighted = true;
                    break;
                }
            }
            
            drawEdge(painter, 
                    nodePositions[fromId].position,
                    nodePositions[toId].position,
                    edge.data,
                    isHighlighted);
        }
    }
    
    //рисуем узлы
    for (auto it = nodePositions.begin(); it != nodePositions.end(); ++it) {
        bool isHighlighted = false;
        for (int i = 0; i < highlightedPath.GetSize(); i++) {
            if (highlightedPath.Get(i) == it.key()) {
                isHighlighted = true;
                break;
            }
        }
        drawNode(painter, it.value(), isHighlighted);
    }
    
    //═══ рисуем торговца ═══
    if (trader && trader->getIsPlaced() && nodePositions.contains(trader->getCurrentPlanetId())) {
        NodePosition traderNode = nodePositions[trader->getCurrentPlanetId()];
        QPointF center = traderNode.position;
        
        //иконка корабля 🚀
        painter.setPen(Qt::NoPen);
        
        //корпус корабля (белый треугольник)
        QPolygonF ship;
        ship << QPointF(center.x(), center.y() - 20)           //нос
             << QPointF(center.x() - 10, center.y() + 10)      //левый край
             << QPointF(center.x() + 10, center.y() + 10);     //правый край
        
        painter.setBrush(QColor(255, 255, 255));
        painter.drawPolygon(ship);
        
        //окно кабины (голубое)
        painter.setBrush(QColor(100, 180, 255));
        painter.drawEllipse(center, 4, 4);
        
        //пламя двигателя (оранжевое)
        painter.setBrush(QColor(255, 150, 50, 180));
        painter.drawEllipse(QPointF(center.x(), center.y() + 12), 3, 5);
        
        //текст с именем торговца
        painter.setPen(QColor(255, 255, 255));
        QFont font = painter.font();
        font.setPointSize(8);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(QRectF(center.x() - 50, center.y() + 20, 100, 20),
                        Qt::AlignCenter,
                        QString::fromStdString(trader->getName()));
    }
    
    //═══ рисуем черную дыру ═══
    drawBlackHole(painter);
    
    
    //лог сообщений
    if (!logMessages.isEmpty()) {
        painter.setPen(QColor(200, 200, 220));
        QFont font("Monospace");
        font.setPointSize(9);
        painter.setFont(font);
        
        int y = height() - 15;
        for (int i = logMessages.size() - 1; i >= 0 && i >= logMessages.size() - 8; i--) {
            //полупрозрачный фон для читаемости
            QRect textRect = painter.fontMetrics().boundingRect(logMessages[i]);
            textRect.moveBottomLeft(QPoint(10, y));
            textRect.adjust(-5, -2, 5, 2);
            painter.fillRect(textRect, QColor(0, 0, 0, 150));
            
            painter.drawText(10, y, logMessages[i]);
            y -= 18;
        }
    }
}

void GraphView::drawNode(QPainter& painter, const NodePosition& node, bool isHighlighted)
{
    const int radius = 30;
    QPointF center = node.position;
    
    //тень
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.drawEllipse(center + QPointF(3, 3), radius + 2, radius + 2);
    
    //свечение для подсвеченных узлов
    if (isHighlighted) {
        QRadialGradient glowGradient(center, radius + 15);
        glowGradient.setColorAt(0, QColor(255, 215, 0, 150));
        glowGradient.setColorAt(0.5, QColor(255, 140, 0, 80));
        glowGradient.setColorAt(1, QColor(255, 69, 0, 0));
        painter.setBrush(glowGradient);
        painter.drawEllipse(center, radius + 15, radius + 15);
    }
    
    //градиент планеты
    QRadialGradient gradient(center - QPointF(10, 10), radius * 1.5);
    if (isHighlighted) {
        gradient.setColorAt(0, QColor(255, 215, 0));
        gradient.setColorAt(0.6, QColor(255, 140, 0));
        gradient.setColorAt(1, QColor(200, 100, 0));
    } else {
        //═══ цвет зависит от количества связей (если включена градация) ═══
        QColor baseColor = getNodeColorByConnections(node.id);
        QColor darkColor = baseColor.darker(150);
        QColor darkerColor = baseColor.darker(200);
        
        gradient.setColorAt(0, baseColor);
        gradient.setColorAt(0.6, darkColor);
        gradient.setColorAt(1, darkerColor);
    }
    
    painter.setBrush(gradient);
    painter.setPen(QPen(isHighlighted ? QColor(255, 215, 0) : QColor(200, 220, 255), 2));
    painter.drawEllipse(center, radius, radius);
    
    //текст
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(9);
    font.setBold(isHighlighted);
    painter.setFont(font);
    
    QRect textRect(center.x() - radius, center.y() - radius, 
                   radius * 2, radius * 2);
    painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, node.name);
}

void GraphView::drawEdge(QPainter& painter, const QPointF& from, const QPointF& to, 
                        const EdgeData& data, bool isHighlighted)
{
    const int nodeRadius = 30;
    
    //вычисляем направление
    QLineF line(from, to);
    double angle = line.angle() * M_PI / 180.0;
    
    //укорачиваем линию, чтобы не заходила на узлы
    QPointF start = from + QPointF(qCos(angle) * nodeRadius, -qSin(angle) * nodeRadius);
    QPointF end = to - QPointF(qCos(angle) * nodeRadius, -qSin(angle) * nodeRadius);
    
    //цвет в зависимости от риска
    QColor edgeColor;
    if (isHighlighted) {
        edgeColor = QColor(255, 215, 0);
    } else {
        int green = 255 - static_cast<int>(data.riskFactor * 255);
        edgeColor = QColor(255, green, 50, 180);
    }
    
    //рисуем линию
    painter.setPen(QPen(edgeColor, isHighlighted ? 4 : 2, 
                       isHighlighted ? Qt::SolidLine : Qt::DashLine));
    painter.drawLine(start, end);
    
    //стрелка
    QPointF arrowP1 = end - QPointF(qCos(angle + M_PI / 6) * 12,
                                    -qSin(angle + M_PI / 6) * 12);
    QPointF arrowP2 = end - QPointF(qCos(angle - M_PI / 6) * 12,
                                    -qSin(angle - M_PI / 6) * 12);
    
    painter.setBrush(edgeColor);
    QPolygonF arrowHead;
    arrowHead << end << arrowP1 << arrowP2;
    painter.drawPolygon(arrowHead);
    
    //вес ребра (посередине)
    QPointF midPoint = (start + end) / 2;
    QString costText = QString::number(data.getTotalCost(), 'f', 1);
    
    //фон для текста
    QRect textRect = painter.fontMetrics().boundingRect(costText);
    textRect.moveCenter(midPoint.toPoint());
    textRect.adjust(-3, -2, 3, 2);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(20, 20, 50, 200));
    painter.drawRoundedRect(textRect, 3, 3);
    
    //текст
    painter.setPen(isHighlighted ? Qt::yellow : Qt::white);
    QFont font = painter.font();
    font.setPointSize(8);
    font.setBold(isHighlighted);
    painter.setFont(font);
    painter.drawText(textRect, Qt::AlignCenter, costText);
}

void GraphView::updateNodePositions()
{
    DynamicArray<int> vertices = graph->GetAllVertices();
    
    //добавляем новые узлы
    for (int i = 0; i < vertices.GetSize(); i++) {
        int id = vertices.Get(i);
        if (!nodePositions.contains(id)) {
            QString name = QString::fromStdString(graph->GetVertexName(id));
            nodePositions[id] = NodePosition(getRandomPosition(), name, id);
        }
    }
    
    //удаляем несуществующие узлы
    QList<int> toRemove;
    for (auto it = nodePositions.begin(); it != nodePositions.end(); ++it) {
        if (!graph->HasVertex(it.key())) {
            toRemove.append(it.key());
        }
    }
    for (int id : toRemove) {
        nodePositions.remove(id);
    }
}

QPointF GraphView::getRandomPosition()
{
    const int margin = 60;
    int x = QRandomGenerator::global()->bounded(margin, width() - margin);
    int y = QRandomGenerator::global()->bounded(margin, height() - margin - 150);
    return QPointF(x, y);
}

int GraphView::getNodeAtPosition(const QPoint& pos)
{
    const int radius = 30;
    for (auto it = nodePositions.begin(); it != nodePositions.end(); ++it) {
        QPointF nodePos = it.value().position;
        if (QLineF(nodePos, pos).length() <= radius) {
            return it.key();
        }
    }
    return -1;
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        selectedNode = getNodeAtPosition(event->pos());
        if (selectedNode != -1) {
            isDragging = true;
            dragOffset = event->pos() - nodePositions[selectedNode].position.toPoint();
        }
    }
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && selectedNode != -1) {
        nodePositions[selectedNode].position = event->pos() - dragOffset;
        update();
    }
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    
    //если перетаскивали планету - пересчитываем расстояния
    if (isDragging && selectedNode != -1) {
        updateEdgeDistances(selectedNode);
    }
    
    isDragging = false;
    selectedNode = -1;
}

void GraphView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    //пересоздаем звезды при изменении размера окна
    initStars();
    update();
}

void GraphView::highlightPath(const DynamicArray<int>& path)
{
    highlightedPath = path;
    
    //═══ анимация пути (если включена) ═══
    if constexpr (ENABLE_VISUAL_EFFECTS && ENABLE_PATH_ANIMATION) {
        if (path.GetSize() > 0) {
            animationStep = 0;
            isAnimating = true;
            //сбрасываем подсветку всех узлов
            for (auto it = nodePositions.begin(); it != nodePositions.end(); ++it) {
                it.value().isHighlighted = false;
            }
            animationTimer->start(PATH_ANIMATION_DELAY_MS);
            return;  //не обновляем сразу, будет через анимацию
        }
    }
    
    update();
}

void GraphView::addLogMessage(const QString& message)
{
    logMessages.append(message);
    if (logMessages.size() > 20) {
        logMessages.removeFirst();
    }
    update();
}

//═══════════════════════════════════════════════════════════════
// ВИЗУАЛЬНЫЕ ЭФФЕКТЫ (легко удалить изменив visual_effects.h)
//═══════════════════════════════════════════════════════════════

void GraphView::onAnimationStep()
{
    if constexpr (!ENABLE_VISUAL_EFFECTS || !ENABLE_PATH_ANIMATION) {
        return;
    }
    
    if (animationStep < highlightedPath.GetSize()) {
        //подсвечиваем следующую планету в пути
        int nodeId = highlightedPath.Get(animationStep);
        if (nodePositions.contains(nodeId)) {
            nodePositions[nodeId].isHighlighted = true;
        }
        animationStep++;
        update();
    } else {
        //анимация завершена
        animationTimer->stop();
        isAnimating = false;
    }
}

void GraphView::onMeteorUpdate()
{
    if constexpr (!ENABLE_VISUAL_EFFECTS || !ENABLE_METEORS) {
        return;
    }
    
    //обновляем позиции метеоритов
    for (auto& meteor : meteors) {
        meteor.position += meteor.velocity;
        
        //если вылетел за экран - возвращаем наверх
        if (meteor.position.y() > height() || meteor.position.x() > width() || 
            meteor.position.x() < 0) {
            meteor.position = QPointF(
                rand() % width(),
                -10
            );
        }
    }
    
    update();
}

void GraphView::initMeteors()
{
    if constexpr (!ENABLE_VISUAL_EFFECTS || !ENABLE_METEORS) {
        return;
    }
    
    meteors.clear();
    for (int i = 0; i < METEOR_COUNT; i++) {
        Meteor meteor;
        meteor.position = QPointF(
            rand() % width(),
            rand() % height()
        );
        
        //случайная скорость и направление
        double speed = METEOR_MIN_SPEED + 
                      (rand() % (METEOR_MAX_SPEED - METEOR_MIN_SPEED + 1));
        double angle = (rand() % 60) + 60;  //угол от 60 до 120 градусов
        
        meteor.velocity = QPointF(
            speed * cos(angle * M_PI / 180.0),
            speed * sin(angle * M_PI / 180.0)
        );
        
        meteor.size = 1 + (rand() % 3);
        meteor.opacity = 0.3 + (rand() % 40) / 100.0;
        
        meteors.append(meteor);
    }
}

void GraphView::initStars()
{
    //генерируем статичные звезды один раз
    stars.clear();
    
    //используем фиксированный seed для воспроизводимости
    srand(42);
    
    for (int i = 0; i < 150; i++) {
        QPointF pos(rand() % 2000, rand() % 2000);  //больше чем экран
        int brightness = 100 + (rand() % 156);      //100-255
        stars.append(Star(pos, brightness));
    }
}

void GraphView::drawStars(QPainter& painter)
{
    //рисуем статичные звезды
    for (const auto& star : stars) {
        //рисуем только те что в видимой области
        if (star.position.x() >= 0 && star.position.x() <= width() &&
            star.position.y() >= 0 && star.position.y() <= height()) {
            
            painter.setPen(QColor(star.brightness, star.brightness, star.brightness));
            painter.drawPoint(star.position);
        }
    }
}

void GraphView::drawMeteors(QPainter& painter)
{
    if constexpr (!ENABLE_VISUAL_EFFECTS || !ENABLE_METEORS) {
        return;
    }
    
    for (const auto& meteor : meteors) {
        //рисуем метеорит как яркую точку со шлейфом
        painter.setOpacity(meteor.opacity);
        
        //свечение
        QRadialGradient glow(meteor.position, meteor.size * 3);
        glow.setColorAt(0, QColor(255, 255, 200, 100));
        glow.setColorAt(1, QColor(255, 255, 200, 0));
        painter.setBrush(glow);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(meteor.position, meteor.size * 3, meteor.size * 3);
        
        //сам метеорит
        painter.setBrush(QColor(255, 255, 255));
        painter.drawEllipse(meteor.position, meteor.size, meteor.size);
        
        //шлейф
        QPointF tail = meteor.position - meteor.velocity * 3;
        QLinearGradient trail(tail, meteor.position);
        trail.setColorAt(0, QColor(255, 200, 100, 0));
        trail.setColorAt(1, QColor(255, 255, 200, 150));
        painter.setPen(QPen(QBrush(trail), meteor.size * 0.5));
        painter.drawLine(tail, meteor.position);
        
        painter.setOpacity(1.0);
    }
}

QColor GraphView::getNodeColorByConnections(int nodeId)
{
    if constexpr (!ENABLE_VISUAL_EFFECTS || !ENABLE_PLANET_GRADATION) {
        //стандартный цвет
        return QColor(100, 150, 255);
    }
    
    //считаем количество связей (входящих + исходящих)
    int connectionCount = 0;
    
    //исходящие ребра
    if (graph->HasVertex(nodeId)) {
        const DynamicArray<Edge>& edges = graph->GetEdges(nodeId);
        connectionCount += edges.GetSize();
    }
    
    //входящие ребра
    DynamicArray<int> allVertices = graph->GetAllVertices();
    for (int i = 0; i < allVertices.GetSize(); i++) {
        int v = allVertices.Get(i);
        const DynamicArray<Edge>& edges = graph->GetEdges(v);
        for (int j = 0; j < edges.GetSize(); j++) {
            if (edges.Get(j).to == nodeId) {
                connectionCount++;
            }
        }
    }
    
    //градация от синего (мало связей) к красному (много связей)
    double ratio = static_cast<double>(connectionCount) / MAX_CONNECTIONS_RED;
    ratio = std::min(ratio, 1.0);
    
    int red = static_cast<int>(50 + ratio * 205);     //50 → 255
    int green = static_cast<int>(150 * (1 - ratio));  //150 → 0
    int blue = static_cast<int>(255 - ratio * 155);   //255 → 100
    
    return QColor(red, green, blue);
}

//═══════════════════════════════════════════════════════════════
// КОНЕЦ ВИЗУАЛЬНЫХ ЭФФЕКТОВ
//═══════════════════════════════════════════════════════════════


//═══════════════════════════════════════════════════════════════
// АВТОМАТИЧЕСКИЙ ПЕРЕСЧЁТ РАССТОЯНИЙ ПРИ ПЕРЕМЕЩЕНИИ ПЛАНЕТ
//═══════════════════════════════════════════════════════════════

void GraphView::updateEdgeDistances(int vertexId)
{
    //проверяем что вершина существует
    if (!graph->HasVertex(vertexId)) {
        return;
    }
    
    //получаем позицию перемещённой планеты
    if (!nodePositions.contains(vertexId)) {
        return;
    }
    QPointF movedPos = nodePositions[vertexId].position;
    
    //получаем все вершины графа
    DynamicArray<int> allVertices = graph->GetAllVertices();
    
    //масштабный коэффициент (пиксели -> условные единицы расстояния)
    const double SCALE_FACTOR = 2.0;
    
    for (int i = 0; i < allVertices.GetSize(); i++) {
        int otherId = allVertices.Get(i);
        
        //пропускаем саму перемещённую планету
        if (otherId == vertexId) {
            continue;
        }
        
        //проверяем есть ли позиция для другой планеты
        if (!nodePositions.contains(otherId)) {
            continue;
        }
        QPointF otherPos = nodePositions[otherId].position;
        
        //вычисляем евклидово расстояние в пикселях
        double pixelDistance = QLineF(movedPos, otherPos).length();
        
        //переводим в условные единицы
        double logicalDistance = pixelDistance / SCALE_FACTOR;
        
        //минимум 1.0
        if (logicalDistance < 1.0) {
            logicalDistance = 1.0;
        }
        
        //обновляем рёбра в обе стороны (если они существуют)
        //из перемещённой планеты в другую
        graph->UpdateEdgeDistance(vertexId, otherId, logicalDistance);
        
        //из другой планеты в перемещённую
        graph->UpdateEdgeDistance(otherId, vertexId, logicalDistance);
    }
}

//═══════════════════════════════════════════════════════════════
// ЧЕРНАЯ ДЫРА - ОПАСНОСТЬ ДЛЯ ПЛАНЕТ
//═══════════════════════════════════════════════════════════════

void GraphView::onBlackHoleSpawn()
{
    //если уже есть активная черная дыра - не спавним новую
    if (blackHole && blackHole->getIsActive()) {
        return;
    }
    
    //удаляем старую черную дыру если она была
    if (blackHole) {
        delete blackHole;
        blackHole = nullptr;
    }
    
    spawnBlackHole();
}

void GraphView::spawnBlackHole()
{
    //проверяем что есть планеты в графе
    if (graph->GetVertexCount() == 0) {
        return;
    }
    
    //случайная позиция за пределами экрана (слева или справа)
    QPointF startPos;
    QPointF velocity;
    
    //50% шанс слева, 50% справа
    if (rand() % 2 == 0) {
        //слева направо
        startPos = QPointF(-100, rand() % height());
        velocity = QPointF(7.5 + (rand() % 500) / 100.0, (rand() % 200 - 100) / 100.0);
    } else {
        //справа налево
        startPos = QPointF(width() + 100, rand() % height());
        velocity = QPointF(-7.5 - (rand() % 500) / 100.0, (rand() % 200 - 100) / 100.0);
    }
    
    //создаем черную дыру
    blackHole = new BlackHole(startPos, velocity, 50.0);
    
    //логируем событие
    addLogMessage("⚠️ ВНИМАНИЕ! Черная дыра появилась в секторе!");
}

void GraphView::onBlackHoleUpdate()
{
    if (!blackHole || !blackHole->getIsActive()) {
        return;
    }
    
    //обновляем позицию черной дыры (deltaTime = 0.016 сек для 60 FPS)
    blackHole->update(0.016);
    
    //проверяем коллизии с планетами
    checkBlackHoleCollisions();
    
    
    //проверяем коллизии с путями (рёбрами)
    checkBlackHoleEdgeCollisions();
    //проверяем коллизии с путями (рёбрами)
    
    //проверяем что черная дыра пересекла весь экран и вышла с другой стороны
    QPointF pos = blackHole->getPosition();
    QPointF vel = blackHole->getVelocity();
    
    bool exitedScreen = false;
    
    //если летела слева направо (velocity.x > 0) - проверяем правый край
    if (vel.x() > 0 && pos.x() > width() + 200) {
        exitedScreen = true;
    }
    
    //если летела справа налево (velocity.x < 0) - проверяем левый край
    if (vel.x() < 0 && pos.x() < -200) {
        exitedScreen = true;
    }
    
    //удаляем только если улетела за экран ИЛИ время жизни вышло
    if (exitedScreen || !blackHole->getIsActive()) {
        delete blackHole;
        blackHole = nullptr;
        addLogMessage("✓ Черная дыра покинула сектор");
    }
    
    update();
}

void GraphView::checkBlackHoleCollisions()
{
    if (!blackHole || !blackHole->getIsActive()) {
        return;
    }
    
    //получаем все планеты
    DynamicArray<int> vertices = graph->GetAllVertices();
    
    for (int i = 0; i < vertices.GetSize(); i++) {
        int planetId = vertices.Get(i);
        
        //проверяем есть ли позиция планеты
        if (!nodePositions.contains(planetId)) {
            continue;
        }
        
        QPointF planetPos = nodePositions[planetId].position;
        
        //проверяем столкновение с черной дырой
        if (blackHole->collidesWithPoint(planetPos, 30.0)) {  //30 = радиус планеты
            //получаем имя планеты
            QString planetName = QString::fromStdString(graph->GetVertexName(planetId));
            
            //проверяем был ли на планете торговец
            if (trader && trader->getIsPlaced() && trader->getCurrentPlanetId() == planetId) {
                //GAME OVER!
                QMessageBox::critical(this, "💀 GAME OVER", 
                    QString("Черная дыра поглотила планету \"%1\" вместе с торговцем %2!\n\n"
                            "🚀 Статистика:\n"
                            "   • Завершено маршрутов: %3\n"
                            "   • Пройдено расстояние: %4\n\n"
                            "Игра окончена!")
                    .arg(planetName)
                    .arg(QString::fromStdString(trader->getName()))
                    .arg(trader->getCompletedRoutes())
                    .arg(trader->getTotalDistance()));
                
                //сбрасываем игру
                graph->Clear();
                trader->reset();
                
                //удаляем черную дыру
                delete blackHole;
                blackHole = nullptr;
                
                addLogMessage("💀 GAME OVER - Торговец погиб!");
            } else {
                //просто уничтожаем планету
                addLogMessage(QString("💥 Черная дыра поглотила планету \"%1\"!").arg(planetName));
                graph->RemoveVertex(planetId);
            }
            
            //выходим из цикла, так как граф изменился
            break;
        }
    }
}

void GraphView::drawBlackHole(QPainter& painter)
{
    if (!blackHole || !blackHole->getIsActive()) {
        return;
    }
    
    QPointF center = blackHole->getPosition();
    double radius = blackHole->getRadius();
    
    //эффект аккреционного диска (вращающиеся частицы)
    double age = blackHole->getAge();
    for (int i = 0; i < 12; i++) {
        double angle = (age * 50.0 + i * 30.0) * M_PI / 180.0;
        double dist = radius * 1.5;
        QPointF particlePos = center + QPointF(cos(angle) * dist, sin(angle) * dist);
        
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 150, 0, 180));
        painter.drawEllipse(particlePos, 3, 3);
    }
    
    //свечение вокруг черной дыры
    QRadialGradient glowGradient(center, radius * 2.0);
    glowGradient.setColorAt(0.0, QColor(255, 100, 0, 200));
    glowGradient.setColorAt(0.5, QColor(200, 50, 0, 100));
    glowGradient.setColorAt(1.0, QColor(100, 0, 0, 0));
    painter.setBrush(glowGradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, radius * 2.0, radius * 2.0);
    
    //горизонт событий (черный круг)
    painter.setBrush(QColor(10, 10, 10));
    painter.setPen(QPen(QColor(255, 50, 0), 2));
    painter.drawEllipse(center, radius, radius);
    
    //центральная сингулярность
    painter.setBrush(QColor(0, 0, 0));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center, radius * 0.3, radius * 0.3);
    
    //предупреждающий текст
    painter.setPen(QColor(255, 100, 0));
    QFont font = painter.font();
    font.setPointSize(10);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(QRectF(center.x() - 100, center.y() + radius + 10, 200, 30),
                    Qt::AlignCenter,
                    "⚠️ ОПАСНО ⚠️");
}

void GraphView::checkBlackHoleEdgeCollisions()
{
    if (!blackHole || !blackHole->getIsActive()) {
        return;
    }
    
    QPointF blackHolePos = blackHole->getPosition();
    double blackHoleRadius = blackHole->getRadius();
    
    DynamicArray<int> vertices = graph->GetAllVertices();
    
    for (int i = 0; i < vertices.GetSize(); i++) {
        int fromId = vertices.Get(i);
        
        if (!nodePositions.contains(fromId)) {
            continue;
        }
        
        const DynamicArray<Edge>& edges = graph->GetEdges(fromId);
        
        for (int j = 0; j < edges.GetSize(); j++) {
            const Edge& edge = edges.Get(j);
            int toId = edge.to;
            
            if (!nodePositions.contains(toId)) {
                continue;
            }
            
            QPointF fromPos = nodePositions[fromId].position;
            QPointF toPos = nodePositions[toId].position;
            
            QPointF lineVec = toPos - fromPos;
            QPointF pointVec = blackHolePos - fromPos;
            
            double lineLen = QLineF(fromPos, toPos).length();
            if (lineLen < 0.001) continue;
            
            double t = QPointF::dotProduct(pointVec, lineVec) / (lineLen * lineLen);
            t = qBound(0.0, t, 1.0);
            
            QPointF closestPoint = fromPos + lineVec * t;
            
            double distance = QLineF(blackHolePos, closestPoint).length();
            
            if (distance <= blackHoleRadius) {
                QString fromName = QString::fromStdString(graph->GetVertexName(fromId));
                QString toName = QString::fromStdString(graph->GetVertexName(toId));
                
                addLogMessage(QString("💥 Черная дыра разорвала путь %1 → %2!")
                             .arg(fromName).arg(toName));
                
                graph->RemoveEdge(fromId, toId);
                break;
            }
        }
    }
}
