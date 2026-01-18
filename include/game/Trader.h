#ifndef TRADER_H
#define TRADER_H

#include <string>

//класс торговца (персонажа игрока)
class Trader {
private:
    std::string name;              //имя торговца
    int currentPlanetId;           //ID планеты где находится
    bool isPlaced;                 //размещён ли на карте
    
    //статистика
    int completedRoutes;           //выполненных маршрутов
    int totalDistance;             //пройденное расстояние

public:
    Trader();
    Trader(const std::string& traderName);
    
    //═══ геттеры ═══
    std::string getName() const { return name; }
    int getCurrentPlanetId() const { return currentPlanetId; }
    bool getIsPlaced() const { return isPlaced; }
    
    //статистика
    int getCompletedRoutes() const { return completedRoutes; }
    int getTotalDistance() const { return totalDistance; }
    
    //═══ сеттеры ═══
    void setCurrentPlanet(int planetId);
    void placeOnPlanet(int planetId);
    void reset();  //сброс при очистке графа
    
    //═══ путешествия ═══
    void completeRoute(int distance);
};

#endif //tRADER_H
