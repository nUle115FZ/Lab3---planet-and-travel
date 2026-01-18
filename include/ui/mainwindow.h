#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
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

private:
    Ui::MainWindow *ui;
    StarGraph graph;
    GraphView *graphView;
    QLabel *statusLabel;
    QTextEdit *logWidget;
    Trader trader;              //торговец игрока
    
    void updateStatusBar();
    void logMessage(const QString& message);
    void showPathResult(const DijkstraResult& result, const QString& from, const QString& to, double timeMicrosec);
    void updateTraderDisplay(); //обновить отображение торговца
};

#endif //mAINWINDOW_H

