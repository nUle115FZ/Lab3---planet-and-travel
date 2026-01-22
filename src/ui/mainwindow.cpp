#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addplanetdialog.h"
#include "addedgedialog.h"
#include "Dijkstra.h"
#include "piratebattle.h"
#include <cstdlib>
#include <ctime>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trader("Капитан Смит")
{
    ui->setupUi(this);
    
    setWindowTitle("🚀 Навигатор Вольного Торговца - Космическая Логистика");
    
    //создаем виджет визуализации графа
    graphView = new GraphView(&graph, this);
    setCentralWidget(graphView);
    
    //создаем панель инструментов
    QToolBar *toolbar = addToolBar("Главная панель");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(32, 32));
    
    QAction *addPlanetAction = toolbar->addAction("➕ Добавить планету");
    QAction *addEdgeAction = toolbar->addAction("🔗 Добавить переход");
    QAction *removePlanetAction = toolbar->addAction("❌ Удалить планету");
    QAction *removeEdgeAction = toolbar->addAction("✂️ Удалить переход");
    toolbar->addSeparator();
    QAction *findPathAction = toolbar->addAction("🎯 Найти путь");
    toolbar->addSeparator();
    QAction *loadFileAction = toolbar->addAction("📂 Открыть файл");
    QAction *saveFileAction = toolbar->addAction("💾 Сохранить в файл");
    QAction *clearAction = toolbar->addAction("🗑️ Очистить");
    toolbar->addSeparator();
    QAction *traderInfoAction = toolbar->addAction("👤 Торговец");
    
    //создаем меню
    QMenu *fileMenu = menuBar()->addMenu("&Файл");
    fileMenu->addAction(loadFileAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Выход");
    
    QMenu *editMenu = menuBar()->addMenu("&Правка");
    editMenu->addAction(addPlanetAction);
    editMenu->addAction(addEdgeAction);
    editMenu->addSeparator();
    editMenu->addAction(removePlanetAction);
    editMenu->addAction(removeEdgeAction);
    editMenu->addSeparator();
    editMenu->addAction(clearAction);
    
    QMenu *toolsMenu = menuBar()->addMenu("&Инструменты");
    toolsMenu->addAction(findPathAction);
    toolsMenu->addSeparator();
    toolsMenu->addAction(traderInfoAction);
    
    QMenu *helpMenu = menuBar()->addMenu("&Справка");
    QAction *aboutAction = helpMenu->addAction("О программе");
    
    //подключаем сигналы
    connect(addPlanetAction, &QAction::triggered, this, &MainWindow::onAddPlanet);
    connect(addEdgeAction, &QAction::triggered, this, &MainWindow::onAddEdge);
    connect(removePlanetAction, &QAction::triggered, this, &MainWindow::onRemovePlanet);
    connect(removeEdgeAction, &QAction::triggered, this, &MainWindow::onRemoveEdge);
    connect(findPathAction, &QAction::triggered, this, &MainWindow::onFindPath);
    connect(loadFileAction, &QAction::triggered, this, &MainWindow::onLoadFromFile);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::onSaveToFile);
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClearGraph);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(traderInfoAction, &QAction::triggered, this, &MainWindow::onShowTraderInfo);
    
    //статус бар
    statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);
    updateStatusBar();
    
    //устанавливаем размер окна
    resize(1200, 800);
    
    logMessage("Добро пожаловать в Навигатор Вольного Торговца!");
    logMessage("📍 Торговец: " + QString::fromStdString(trader.getName()));
    logMessage("⚠️ Загрузите граф или создайте первую планету для размещения торговца");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddPlanet()
{
    AddPlanetDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString planetName = dialog.getPlanetName();
        
        //дополнительная проверка (на случай если диалог пропустил)
        if (planetName.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", 
                "Имя планеты не может быть пустым!");
            return;
        }
        
        try {
            graph.AddVertex(planetName.toStdString());
            
            //если это первая планета - размещаем торговца
            if (graph.GetVertexCount() == 1 && !trader.getIsPlaced()) {
                int planetId = graph.GetVertexIndex(planetName.toStdString());
                trader.placeOnPlanet(planetId);
                logMessage("🚀 Торговец \"" + QString::fromStdString(trader.getName()) + 
                          "\" высадился на планете \"" + planetName + "\"");
            }
            
            graphView->setTrader(&trader);  //передаём торговца в graphview
            graphView->update();
            updateStatusBar();
            logMessage("✓ Планета \"" + planetName + "\" добавлена");
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", 
                QString("Не удалось добавить планету: %1").arg(e.what()));
        }
    }
}

void MainWindow::onAddEdge()
{
    if (graph.GetVertexCount() < 2) {
        QMessageBox::information(this, "Информация", 
            "Для создания перехода необходимо минимум 2 планеты!");
        return;
    }
    
    AddEdgeDialog dialog(&graph, this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            QString from = dialog.getFromPlanet();
            QString to = dialog.getToPlanet();
            double distance = dialog.getDistance();
            double risk = dialog.getRisk();
            
            graph.AddEdge(from.toStdString(), to.toStdString(), 
                         EdgeData(distance, risk));
            graphView->update();
            
            double totalCost = distance * (1.0 + risk);
            logMessage(QString("✓ Переход %1 → %2 создан (стоимость: %3)")
                      .arg(from).arg(to).arg(totalCost, 0, 'f', 2));
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", 
                QString("Не удалось создать переход: %1").arg(e.what()));
        }
    }
}

void MainWindow::onRemovePlanet()
{
    if (graph.GetVertexCount() == 0) {
        QMessageBox::information(this, "Информация", 
            "Граф пуст!");
        return;
    }
    
    //получаем список всех планет
    QStringList planets;
    const auto& allPlanets = graph.GetAllPlanets();
    for (const auto& pair : allPlanets) {
        planets << QString::fromStdString(pair.second);
    }
    
    //выбор планеты для удаления
    bool ok;
    QString planetName = QInputDialog::getItem(this, "Удаление планеты",
        "Выберите планету для удаления:", planets, 0, false, &ok);
    
    if (ok && !planetName.isEmpty()) {
        //проверяем, не находится ли торговец на этой планете
        if (trader.getIsPlaced()) {
            int planetId = graph.GetVertexIndex(planetName.toStdString());
            if (trader.getCurrentPlanetId() == planetId) {
                QMessageBox::warning(this, "Ошибка",
                    QString("⚠️ Невозможно удалить планету \"%1\"!\n\n"
                           "На этой планете находится торговец \"%2\".\n"
                           "Переместите торговца на другую планету перед удалением.")
                        .arg(planetName)
                        .arg(QString::fromStdString(trader.getName())));
                return;
            }
        }
        
        QMessageBox::StandardButton reply = QMessageBox::question(this, 
            "Подтверждение", 
            QString("Удалить планету \"%1\" и все связанные с ней переходы?").arg(planetName),
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            try {
                graph.RemoveVertex(planetName.toStdString());
                graphView->update();
                updateStatusBar();
                logMessage(QString("✓ Планета \"%1\" удалена").arg(planetName));
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Ошибка", 
                    QString("Не удалось удалить планету: %1").arg(e.what()));
            }
        }
    }
}

void MainWindow::onRemoveEdge()
{
    if (graph.GetVertexCount() < 2) {
        QMessageBox::information(this, "Информация", 
            "Недостаточно планет!");
        return;
    }
    
    //получаем список всех планет
    QStringList planets;
    const auto& allPlanets = graph.GetAllPlanets();
    for (const auto& pair : allPlanets) {
        planets << QString::fromStdString(pair.second);
    }
    
    //выбор начальной планеты
    bool ok1;
    QString fromPlanet = QInputDialog::getItem(this, "Удаление перехода",
        "Выберите начальную планету:", planets, 0, false, &ok1);
    
    if (!ok1 || fromPlanet.isEmpty()) {
        return;
    }
    
    //выбор конечной планеты
    bool ok2;
    QString toPlanet = QInputDialog::getItem(this, "Удаление перехода",
        "Выберите конечную планету:", planets, 0, false, &ok2);
    
    if (!ok2 || toPlanet.isEmpty()) {
        return;
    }
    
    if (fromPlanet == toPlanet) {
        QMessageBox::warning(this, "Ошибка", 
            "Начальная и конечная планеты не могут совпадать!");
        return;
    }
    
    try {
        //проверяем существует ли такое ребро
        int fromId = graph.GetVertexIndex(fromPlanet.toStdString());
        int toId = graph.GetVertexIndex(toPlanet.toStdString());
        
        const DynamicArray<Edge>& edges = graph.GetEdges(fromId);
        bool edgeExists = false;
        for (int i = 0; i < edges.GetSize(); i++) {
            if (edges.Get(i).to == toId) {
                edgeExists = true;
                break;
            }
        }
        
        if (!edgeExists) {
            QMessageBox::warning(this, "Ошибка", 
                QString("Переход %1 → %2 не существует!").arg(fromPlanet).arg(toPlanet));
            return;
        }
        
        graph.RemoveEdge(fromPlanet.toStdString(), toPlanet.toStdString());
        graphView->update();
        logMessage(QString("✓ Переход %1 → %2 удален").arg(fromPlanet).arg(toPlanet));
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", 
            QString("Не удалось удалить переход: %1").arg(e.what()));
    }
}

void MainWindow::onFindPath()
{
    if (graph.GetVertexCount() < 2) {
        QMessageBox::information(this, "Информация", 
            "Недостаточно планет для построения маршрута!");
        return;
    }
    
    if (!trader.getIsPlaced()) {
        QMessageBox::warning(this, "Ошибка",
            "⚠️ Торговец не размещён на карте!\n\nСоздайте планету или загрузите граф.");
        return;
    }
    
    //получаем список всех планет
    QStringList planets;
    const auto& allPlanets = graph.GetAllPlanets();
    for (const auto& pair : allPlanets) {
        planets << QString::fromStdString(pair.second);
    }
    
    //текущая позиция торговца как начальная точка
    std::string currentPlanetName = graph.GetVertexName(trader.getCurrentPlanetId());
    QString fromPlanet = QString::fromStdString(currentPlanetName);
    
    logMessage(QString("🚀 Торговец находится на планете: %1").arg(fromPlanet));
    
    //выбор конечной планеты
    bool ok;
    QString toPlanet = QInputDialog::getItem(this, "Пункт назначения",
        QString("Торговец на: %1\nВыберите пункт назначения:").arg(fromPlanet),
        planets, 0, false, &ok);
    
    if (!ok) return;
    
    if (fromPlanet == toPlanet) {
        QMessageBox::information(this, "Информация",
            "Торговец уже находится на этой планете!");
        return;
    }
    
    //поиск пути
    Dijkstra dijkstra(graph);
    auto start = std::chrono::high_resolution_clock::now();
    DijkstraResult result = dijkstra.FindShortestPath(
        fromPlanet.toStdString(), toPlanet.toStdString());
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    showPathResult(result, fromPlanet, toPlanet, duration.count());
    
    //визуализируем путь
    if (result.pathExists) {
        graphView->highlightPath(result.path);
        
        //перемещаем торговца в конечную точку
        int destinationId = graph.GetVertexIndex(toPlanet.toStdString());
        //═══ ПРОВЕРКА НАПАДЕНИЯ ПИРАТОВ ═══
        bool defeatedByPirates = false;
        for (int i = 0; i < result.path.GetSize() - 1; i++) {
            int fromId = result.path.Get(i);
            int toId = result.path.Get(i + 1);
            
            const DynamicArray<Edge>& edges = graph.GetEdges(fromId);
            for (int j = 0; j < edges.GetSize(); j++) {
                if (edges.Get(j).to == toId) {
                    double riskFactor = edges.Get(j).data.riskFactor;
                    double random = (rand() % 100) / 100.0;
                    
                    if (random < riskFactor) {
                        QString fromName = QString::fromStdString(graph.GetVertexName(fromId));
                        QString toName = QString::fromStdString(graph.GetVertexName(toId));
                        
                        logMessage(QString("🏴‍☠️ ТРЕВОГА! Пираты атакуют на пути %1 → %2!")
                                  .arg(fromName).arg(toName));
                        
                        PirateBattle battle(riskFactor, this);
                        battle.exec();
                        
                        if (!battle.isVictory()) {
                            defeatedByPirates = true;
                            QMessageBox::critical(this, "💀 GAME OVER",
                                QString("💀 Пираты уничтожили ваш корабль!\n\n"
                                       "🏴‍☠️ Место гибели: между %1 и %2\n"
                                       "🚀 Статистика:\n"
                                       "   • Завершено маршрутов: %3\n"
                                       "   • Пройдено расстояние: %4\n\n"
                                       "Игра окончена!")
                                .arg(fromName).arg(toName)
                                .arg(trader.getCompletedRoutes())
                                .arg(trader.getTotalDistance()));
                            
                            graph.Clear();
                            trader.reset();
                            updateTraderDisplay();
                            logMessage("💀 GAME OVER - Корабль уничтожен пиратами!");
                            return;
                        } else {
                            logMessage(QString("⚔️ Пираты повержены! Путь %1 → %2 свободен!")
                                      .arg(fromName).arg(toName));
                        }
                    }
                    break;
                }
            }
            
            if (defeatedByPirates) break;
        }
        
        if (!defeatedByPirates) {
        trader.setCurrentPlanet(destinationId);
        trader.completeRoute(static_cast<int>(result.totalCost));
        
        logMessage(QString("✓ Торговец прибыл на планету: %1").arg(toPlanet));
        logMessage(QString("📊 Всего маршрутов: %1 | Пройдено: %2")
                  .arg(trader.getCompletedRoutes())
                  .arg(trader.getTotalDistance()));
        
        updateTraderDisplay();
    }
}
        }

void MainWindow::onClearGraph()
{
    if (graph.GetVertexCount() == 0) return;
    
    auto reply = QMessageBox::question(this, "Подтверждение",
        "Вы уверены, что хотите очистить граф?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        graph.Clear();
        trader.reset();  //сбрасываем торговца
        graphView->update();
        updateStatusBar();
        logMessage("🗑️ Граф очищен");
        logMessage("⚠️ Создайте планету для размещения торговца");
    }
}

void MainWindow::onLoadFromFile()
{
    //выбираем файл для загрузки
    QString filename = QFileDialog::getOpenFileName(this,
        "Загрузить граф из файла",
        "",
        "Text Files (*.txt);;All Files (*)");
    
    if (filename.isEmpty()) {
        return;  //пользователь отменил выбор
    }
    
    //если граф не пуст, спрашиваем подтверждение
    if (graph.GetVertexCount() > 0) {
        auto reply = QMessageBox::question(this, "Подтверждение",
            "Граф не пуст. Очистить и загрузить из файла?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply != QMessageBox::Yes) return;
        
        //явно очищаем граф и сбрасываем торговца
        graph.Clear();
        trader.reset();
    }
    
    try {
        graph.LoadFromFile(filename.toStdString());
        
        //размещаем торговца на первой планете
        if (graph.GetVertexCount() > 0) {
            DynamicArray<int> allVertices = graph.GetAllVertices();
            if (allVertices.GetSize() > 0) {
                int firstPlanetId = allVertices.Get(0);
                trader.placeOnPlanet(firstPlanetId);
                std::string planetName = graph.GetVertexName(firstPlanetId);
                logMessage("🚀 Торговец \"" + QString::fromStdString(trader.getName()) + 
                          "\" высадился на планете \"" + QString::fromStdString(planetName) + "\"");
            }
        }
        
        graphView->setTrader(&trader);
        graphView->update();
        updateStatusBar();
        
        logMessage("✓ Граф загружен из файла: " + QFileInfo(filename).fileName());
        logMessage(QString("  Содержит %1 планет").arg(graph.GetVertexCount()));
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка",
            QString("Не удалось загрузить граф: %1").arg(e.what()));
    }
}

void MainWindow::onSaveToFile()
{
    //проверяем что есть что сохранять
    if (graph.GetVertexCount() == 0) {
        QMessageBox::information(this, "Информация",
            "Граф пуст. Нечего сохранять.");
        return;
    }
    
    //выбираем файл для сохранения
    QString filename = QFileDialog::getSaveFileName(this,
        "Сохранить граф в файл",
        "my_graph.txt",
        "Text Files (*.txt);;All Files (*)");
    
    if (filename.isEmpty()) {
        return;  //пользователь отменил выбор
    }
    
    try {
        graph.SaveToFile(filename.toStdString());
        logMessage("✓ Граф сохранен в файл: " + QFileInfo(filename).fileName());
        QMessageBox::information(this, "Успех",
            "Граф успешно сохранен!");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка",
            QString("Не удалось сохранить граф: %1").arg(e.what()));
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "О программе",
        "<h2>🚀 Навигатор Вольного Торговца</h2>"
        "<p><b>Версия:</b> 1.0</p>"
        "<p><b>Лабораторная работа №3</b></p>"
        "<p>Курс: Информатика (Алгоритмы на графах)</p>"
        "<hr>"
        "<p><b>Реализованные компоненты:</b></p>"
        "<ul>"
        "<li>M-1.1: Ориентированный граф (5 баллов)</li>"
        "<li>M-2.2.1: Поиск кратчайших путей (5 баллов)</li>"
        "<li>M-3.4: PriorityQueue как наследник Sequence (7 баллов)</li>"
        "</ul>"
        "<p><b>Итоговый рейтинг:</b> 17 баллов</p>"
        "<hr>"
        "<p>Программа использует алгоритм Дейкстры для поиска оптимальных "
        "маршрутов между звездными системами с учетом расстояния и фактора риска.</p>");
}

void MainWindow::updateStatusBar()
{
    int vertices = graph.GetVertexCount();
    int edges = 0;
    
    DynamicArray<int> allVertices = graph.GetAllVertices();
    for (int i = 0; i < allVertices.GetSize(); i++) {
        edges += graph.GetEdges(allVertices.Get(i)).GetSize();
    }
    
    QString status = QString("Планет: %1 | Переходов: %2").arg(vertices).arg(edges);
    
    //добавляем информацию о торговце
    if (trader.getIsPlaced()) {
        std::string planetName = graph.GetVertexName(trader.getCurrentPlanetId());
        status += QString(" | 🚀 Торговец: %1 | Маршрутов: %2")
                    .arg(QString::fromStdString(planetName))
                    .arg(trader.getCompletedRoutes());
    } else {
        status += " | ⚠️ Торговец не размещён";
    }
    
    statusLabel->setText(status);
}

void MainWindow::logMessage(const QString& message)
{
    if (graphView) {
        graphView->addLogMessage(message);
    }
}

void MainWindow::showPathResult(const DijkstraResult& result, 
                                const QString& from, const QString& to, 
                                double timeMicrosec)
{
    if (result.pathExists) {
        QString pathStr;
        for (int i = 0; i < result.pathNames.GetSize(); i++) {
            pathStr += QString::fromStdString(result.pathNames.Get(i));
            if (i < result.pathNames.GetSize() - 1) {
                pathStr += " → ";
            }
        }
        
        QString message = QString(
            "✓ Маршрут найден!\n\n"
            "От: %1\n"
            "До: %2\n"
            "Стоимость: %3 единиц\n"
            "Время расчета: %4 мкс\n\n"
            "Путь:\n%5")
            .arg(from)
            .arg(to)
            .arg(result.totalCost, 0, 'f', 2)
            .arg(timeMicrosec, 0, 'f', 2)
            .arg(pathStr);
        
        QMessageBox::information(this, "Маршрут найден", message);
        logMessage(QString("🎯 Найден путь %1 → %2 (стоимость: %3)")
                  .arg(from).arg(to).arg(result.totalCost, 0, 'f', 2));
    } else {
        QMessageBox::warning(this, "Маршрут не найден",
            QString("Не удалось найти путь от %1 до %2.\n"
                   "Планеты не связаны.").arg(from).arg(to));
        logMessage(QString("⚠ Путь %1 → %2 не найден").arg(from).arg(to));
    }
}

//═══════════════════════════════════════════════════════════
//Торговец
//═══════════════════════════════════════════════════════════

void MainWindow::onShowTraderInfo()
{
    if (!trader.getIsPlaced()) {
        QMessageBox::information(this, "Торговец",
            "⚠️ Торговец ещё не размещён на карте.\n\n"
            "Создайте планету или загрузите граф для размещения торговца.");
        return;
    }
    
    std::string currentPlanetName = graph.GetVertexName(trader.getCurrentPlanetId());
    
    QString info = QString(
        "<h2>👤 Информация о торговце</h2>"
        "<table>"
        "<tr><td><b>Имя:</b></td><td>%1</td></tr>"
        "<tr><td><b>Текущая планета:</b></td><td>🪐 %2</td></tr>"
        "<tr><td><b>Выполнено маршрутов:</b></td><td>%3</td></tr>"
        "<tr><td><b>Пройдено расстояние:</b></td><td>%4</td></tr>"
        "</table>")
        .arg(QString::fromStdString(trader.getName()))
        .arg(QString::fromStdString(currentPlanetName))
        .arg(trader.getCompletedRoutes())
        .arg(trader.getTotalDistance());
    
    QMessageBox::information(this, "Торговец", info);
}

void MainWindow::updateTraderDisplay()
{
    //обновляем статус бар с информацией о торговце
    updateStatusBar();
    
    //обновляем graphview если нужно
    if (graphView) {
        graphView->update();
    }
}

