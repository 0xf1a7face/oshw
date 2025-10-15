#ifndef NUMBER_H
#define NUMBER_H

class Number {
private:
    double value_;

public:
    // Конструкторы
    Number();
    explicit Number(double val);

    // Операторы
    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;

    // Получение значения
    double getValue() const;

    // Статические константы
    static const Number ZERO;
    static const Number ONE;

    // Функция для создания числа
    static Number create(double val);
};

#endif // NUMBER_H
