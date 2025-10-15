#include "pch.h"
#include "number.h"
#include <stdexcept>

// Определение статических констант
const Number Number::ZERO = Number(0.0);
const Number Number::ONE  = Number(1.0);

Number::Number() : value_(0.0) {}

Number::Number(double val) : value_(val) {}

Number Number::operator+(const Number& other) const {
    return Number(value_ + other.value_);
}

Number Number::operator-(const Number& other) const {
    return Number(value_ - other.value_);
}

Number Number::operator*(const Number& other) const {
    return Number(value_ * other.value_);
}

Number Number::operator/(const Number& other) const {
    if (other.value_ == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return Number(value_ / other.value_);
}

double Number::getValue() const {
    return value_;
}

Number Number::create(double val) {
    return Number(val);
}
