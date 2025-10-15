#ifndef VECTOR_H
#define VECTOR_H

#include "../number/number.h"
#include <cmath>

class Vector {
private:
    Number x_;
    Number y_;

public:
    // Конструкторы
    Vector();
    Vector(const Number& x, const Number& y);

    Vector(const Vector& other);
    Vector& operator=(const Vector& other);

    // Методы для получения полярных координат
    Number getRho() const;      // Модуль вектора
    Number getTheta() const;    // Угол в радианах

    // Оператор сложения
    Vector operator+(const Vector& other) const;

    // Геттеры для декартовых координат
    Number getX() const;
    Number getY() const;

    // Статические константные векторы
    static const Vector ZERO;
    static const Vector ONE;
};

#endif // VECTOR_H
