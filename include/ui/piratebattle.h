#ifndef PIRATEBATTLE_H
#define PIRATEBATTLE_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

//типы пиратов
enum class PirateType {
    ROOKIE,    //новички (risk < 0.25)
    VETERAN,   //ветераны (0.25 <= risk <= 0.4)
    BOSS       //босс-пираты (risk > 0.4)
};

//диалог сражения с пиратами
class PirateBattle : public QDialog {
    Q_OBJECT

public:
    explicit PirateBattle(double riskFactor, QWidget *parent = nullptr);
    
    bool isVictory() const { return victory; }
    PirateType getPirateType() const { return pirateType; }

private slots:
    void onAttackClick();      //клик по кнопке атаки
    void onTimerTick();        //обновление таймера
    void onBattleEnd();        //конец битвы

private:
    void updateDisplay();                    //обновление отображения
    PirateType determinePirateType(double risk); //определение типа пирата


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
    PirateType pirateType;     //тип пирата
    
    int requiredClicks;        //необходимое количество кликов (зависит от типа)
    int battleTime;            //время битвы (зависит от типа)
};

#endif // PIRATEBATTLE_H
