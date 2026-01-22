#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QTimer>
#include "StarGraph.h"
#include "Dijkstra.h"
#include "graphview.h"
#include "Trader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddPlanet();
    void onAddEdge();
    void onRemovePlanet();
    void onRemoveEdge();
    void onFindPath();
    void onLoadFromFile();      //загрузить граф из файла
    void onSaveToFile();        //сохранить граф в файл
    void onClearGraph();
    void onAbout();
    
    //═══ торговец ═══
    void onShowTraderInfo();    //показать информацию о торговце
    
    //═══ игровой таймер ═══
    void onGameTimerTick();     //обновление игрового таймера

private:
    Ui::MainWindow *ui;
    StarGraph graph;
    GraphView *graphView;
    QLabel *statusLabel;
    QTextEdit *logWidget;
    Trader trader;              //торговец игрока
    
    //═══ игровая механика ═══
    QTimer* gameTimer;          //таймер игры
    QLabel* timerLabel;         //отображение времени
    QLabel* artifactLabel;      //отображение счетчика артефактов
    int gameTimeSeconds;        //прошедшее время в секундах
    int collectedArtifacts;     //собранные артефакты
    bool gameStarted;           //началась ли игра
    
    static constexpr int REQUIRED_ARTIFACTS = 10; //цель игры
    static constexpr int MIN_PLANETS_TO_START = 5; //минимум планет для старта
    
    void updateStatusBar();
    void logMessage(const QString& message);
    void showPathResult(const DijkstraResult& result, const QString& from, const QString& to, double timeMicrosec);
    void updateTraderDisplay(); //обновить отображение торговца
    
    //═══ управление игрой ═══
    void startGame();           //начать игру
    void checkGameStart();      //проверить условия старта
    void spawnArtifact();       //создать артефакт на случайной планете
    void collectArtifact(int planetId); //собрать артефакт
    void checkVictory();        //проверить победу
};

#endif //mAINWINDOW_H

