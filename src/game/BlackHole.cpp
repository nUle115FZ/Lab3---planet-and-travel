#include "BlackHole.h"
#include <QtMath>

BlackHole::BlackHole()
    : position(0, 0), velocity(0, 0), radius(50.0), 
      isActive(false), lifetime(300.0), age(0.0)
{
}

BlackHole::BlackHole(QPointF pos, QPointF vel, double rad)
    : position(pos), velocity(vel), radius(rad), 
      isActive(true), lifetime(300.0), age(0.0)
{
}

void BlackHole::update(double deltaTime)
{
    if (!isActive) {
        return;
    }
    
    //двигаем черную дыру
    position += velocity * deltaTime;
    
    //увеличиваем возраст
    age += deltaTime;
    
    //если время вышло - деактивируем
    if (age >= lifetime) {
        isActive = false;
    }
}

bool BlackHole::collidesWithPoint(QPointF point, double margin) const
{
    if (!isActive) {
        return false;
    }
    
    //вычисляем расстояние
    double dx = position.x() - point.x();
    double dy = position.y() - point.y();
    double distance = qSqrt(dx * dx + dy * dy);
    
    //проверяем столкновение
    return distance <= (radius + margin);
}
