#ifndef PIRATEBATTLE_H
#define PIRATEBATTLE_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

//диалог сражения с пиратами
class PirateBattle : public QDialog {
    Q_OBJECT

public:
    explicit PirateBattle(double riskFactor, QWidget *parent = nullptr);
    
    bool isVictory() const { return victory; }

private slots:
    void onAttackClick();      //клик по кнопке атаки
    void onTimerTick();        //обновление таймера
    void onBattleEnd();        //конец битвы

private:
    void updateDisplay();      //обновление отображения


    QLabel* infoLabel;         //информация о битве
    QLabel* timerLabel;        //отображение времени
    QLabel* clicksLabel;       //отображение кликов
    QPushButton* attackButton; //кнопка атаки
    
    QTimer* battleTimer;       //таймер обратного отсчёта
    QTimer* endTimer;          //таймер завершения битвы
    
    int clicksCount;           //текущее количество кликов
    int timeLeft;              //оставшееся время (в десятых долях секунды)
    double riskFactor;         //сложность битвы
    bool victory;              //победа или поражение
    
    static constexpr int REQUIRED_CLICKS = 10;  //нужно кликов
    static constexpr int BATTLE_TIME = 50;      //5 секунд = 50 * 0.1 сек
};

#endif // PIRATEBATTLE_H
