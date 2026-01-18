#ifndef VISUAL_EFFECTS_H
#define VISUAL_EFFECTS_H

//    Чтобы удалить ВСЕ эффекты:
//    1. Измени ENABLE_VISUAL_EFFECTS на false

//ГЛАВНЫЙ ПЕРЕКЛЮЧАТЕЛЬ (true = включить, false = выключить)
constexpr bool ENABLE_VISUAL_EFFECTS = true;

//отдельные эффекты (работают только если ENABLE_VISUAL_EFFECTS = true)
constexpr bool ENABLE_PATH_ANIMATION = true;      //анимация найденного пути
constexpr bool ENABLE_PLANET_GRADATION = true;    //цветовая градация планет
constexpr bool ENABLE_METEORS = true;             //метеориты на фоне

//настройки анимации пути
constexpr int PATH_ANIMATION_DELAY_MS = 300;      //задержка между планетами (мс)
constexpr int PATH_GLOW_DURATION_MS = 500;        //длительность свечения

//настройки градации планет
constexpr int MIN_CONNECTIONS_BLUE = 0;           //мало связей - синий
constexpr int MAX_CONNECTIONS_RED = 8;            //много связей - красный

//настройки метеоритов
constexpr int METEOR_COUNT = 15;                  //количество метеоритов
constexpr int METEOR_MIN_SPEED = 1;               //минимальная скорость
constexpr int METEOR_MAX_SPEED = 3;               //максимальная скорость
constexpr int METEOR_UPDATE_INTERVAL_MS = 50;     //частота обновления (мс)

#endif //VISUAL_EFFECTS_H

