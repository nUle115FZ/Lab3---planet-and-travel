#include "piratebattle.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QApplication>

PirateBattle::PirateBattle(double risk, QWidget *parent)
    : QDialog(parent), clicksCount(0), timeLeft(BATTLE_TIME), 
      riskFactor(risk), victory(false)
{
    setWindowTitle("⚔️ НАПАДЕНИЕ ПИРАТОВ!");
    setModal(true);
    setMinimumSize(400, 300);
    
    //создаём layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    
    //═══ информация ═══
    infoLabel = new QLabel(this);
    infoLabel->setText(QString("🏴‍☠️ Космические пираты атакуют!\n"
                               "Уровень опасности: %1%\n\n"
                               "Быстрее стреляй из бластера!")
                      .arg(QString::number(riskFactor * 100, 'f', 0)));
    infoLabel->setAlignment(Qt::AlignCenter);
    QFont infoFont = infoLabel->font();
    infoFont.setPointSize(12);
    infoLabel->setFont(infoFont);
    mainLayout->addWidget(infoLabel);
    
    //═══ таймер ═══
    timerLabel = new QLabel(this);
    timerLabel->setAlignment(Qt::AlignCenter);
    QFont timerFont = timerLabel->font();
    timerFont.setPointSize(24);
    timerFont.setBold(true);
    timerLabel->setFont(timerFont);
    timerLabel->setStyleSheet("color: red;");
    mainLayout->addWidget(timerLabel);
    
    //═══ счётчик кликов ═══
    clicksLabel = new QLabel(this);
    clicksLabel->setAlignment(Qt::AlignCenter);
    QFont clicksFont = clicksLabel->font();
    clicksFont.setPointSize(16);
    clicksLabel->setFont(clicksFont);
    mainLayout->addWidget(clicksLabel);
    
    //═══ кнопка атаки ═══
    attackButton = new QPushButton("🔫 СТРЕЛЯТЬ!", this);
    QFont buttonFont = attackButton->font();
    buttonFont.setPointSize(18);
    buttonFont.setBold(true);
    attackButton->setFont(buttonFont);
    attackButton->setMinimumHeight(80);
    attackButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #ff4444;"
        "   color: white;"
        "   border: 3px solid #cc0000;"
        "   border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #ff6666;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #cc0000;"
        "}"
    );
    connect(attackButton, &QPushButton::clicked, this, &PirateBattle::onAttackClick);
    mainLayout->addWidget(attackButton);
    
    //обновляем отображение
    updateDisplay();
    
    //═══ запускаем таймер битвы (обновление каждые 0.1 сек) ═══
    battleTimer = new QTimer(this);
    connect(battleTimer, &QTimer::timeout, this, &PirateBattle::onTimerTick);
    battleTimer->start(100);  //100 мс = 0.1 сек
}

void PirateBattle::onAttackClick()
{
    clicksCount++;
    updateDisplay();
    
    //проверяем победу
    if (clicksCount >= REQUIRED_CLICKS) {
        victory = true;
        battleTimer->stop();
        
        attackButton->setEnabled(false);
        attackButton->setText("✅ ПОБЕДА!");
        attackButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #44ff44;"
            "   color: white;"
            "   border: 3px solid #00cc00;"
            "   border-radius: 10px;"
            "}"
        );
        
        infoLabel->setText("🎉 ВЫ ПОБЕДИЛИ!\n\nПираты отступают!");
        
        //закрываем диалог через 1.5 секунды
        endTimer = new QTimer(this);
        connect(endTimer, &QTimer::timeout, this, &PirateBattle::onBattleEnd);
        endTimer->setSingleShot(true);
        endTimer->start(1500);
    }
}

void PirateBattle::onTimerTick()
{
    timeLeft--;
    updateDisplay();
    
    //проверяем поражение
    if (timeLeft <= 0 && clicksCount < REQUIRED_CLICKS) {
        victory = false;
        battleTimer->stop();
        
        attackButton->setEnabled(false);
        attackButton->setText("💀 ПОРАЖЕНИЕ");
        attackButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #444444;"
            "   color: white;"
            "   border: 3px solid #222222;"
            "   border-radius: 10px;"
            "}"
        );
        
        infoLabel->setText("💀 КОРАБЛЬ УНИЧТОЖЕН!\n\nПираты захватили ваш корабль!");
        
        //закрываем диалог через 2 секунды
        endTimer = new QTimer(this);
        connect(endTimer, &QTimer::timeout, this, &PirateBattle::onBattleEnd);
        endTimer->setSingleShot(true);
        endTimer->start(2000);
    }
}

void PirateBattle::onBattleEnd()
{
    accept();  //закрываем диалог
}

void PirateBattle::updateDisplay()
{
    //обновляем таймер
    double seconds = timeLeft / 10.0;
    timerLabel->setText(QString("⏱️ Время: %1 сек").arg(QString::number(seconds, 'f', 1)));
    
    //меняем цвет таймера в зависимости от времени
    if (timeLeft > 30) {
        timerLabel->setStyleSheet("color: green;");
    } else if (timeLeft > 15) {
        timerLabel->setStyleSheet("color: orange;");
    } else {
        timerLabel->setStyleSheet("color: red;");
    }
    
    //обновляем счётчик кликов
    clicksLabel->setText(QString("🎯 Попаданий: %1 / %2")
                        .arg(clicksCount)
                        .arg(REQUIRED_CLICKS));
}
