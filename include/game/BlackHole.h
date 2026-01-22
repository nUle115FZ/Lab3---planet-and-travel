#ifndef BLACKHOLE_H
#define BLACKHOLE_H

#include <QPointF>

//черная дыра - опасность для планет
class BlackHole {
private:
    QPointF position;      //текущая позиция
    QPointF velocity;      //скорость и направление движения
    double radius;         //радиус поражения
    bool isActive;         //активна ли черная дыра
    double lifetime;       //время жизни в секундах
    double age;            //текущий возраст в секундах

public:
    BlackHole();
    BlackHole(QPointF pos, QPointF vel, double rad);
    
    //геттеры
    QPointF getPosition() const { return position; }
    QPointF getVelocity() const { return velocity; }
    double getRadius() const { return radius; }
    bool getIsActive() const { return isActive; }
    double getAge() const { return age; }
    double getLifetime() const { return lifetime; }
    
    //обновление позиции
    void update(double deltaTime);
    
    //проверка коллизии с точкой
    bool collidesWithPoint(QPointF point, double margin = 0.0) const;
    
    //деактивация
    void deactivate() { isActive = false; }
};

#endif // BLACKHOLE_H
