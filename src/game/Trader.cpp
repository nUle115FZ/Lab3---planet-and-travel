#include "Trader.h"

//═══════════════════════════════════════════════════════════
//Trader implementation
//═══════════════════════════════════════════════════════════

Trader::Trader()
    : name("Торговец"), currentPlanetId(-1), isPlaced(false),
      completedRoutes(0), totalDistance(0) {}

Trader::Trader(const std::string& traderName)
    : name(traderName), currentPlanetId(-1), isPlaced(false),
      completedRoutes(0), totalDistance(0) {}

void Trader::setCurrentPlanet(int planetId) {
    currentPlanetId = planetId;
    if (!isPlaced && planetId >= 0) {
        isPlaced = true;
    }
}

void Trader::placeOnPlanet(int planetId) {
    currentPlanetId = planetId;
    isPlaced = true;
}

void Trader::reset() {
    currentPlanetId = -1;
    isPlaced = false;
    completedRoutes = 0;
    totalDistance = 0;
}

void Trader::completeRoute(int distance) {
    completedRoutes++;
    totalDistance += distance;
}
