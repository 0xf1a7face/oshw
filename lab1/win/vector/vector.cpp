#include "pch.h"
#include "vector.h"
#include <cmath>

// Определение статических констант
const Vector Vector::ZERO = Vector(Number::ZERO, Number::ZERO);
const Vector Vector::ONE = Vector(Number::ONE, Number::ZERO);

Vector::Vector() : x_(Number::ZERO), y_(Number::ZERO) {}

Vector::Vector(const Number &x, const Number &y) : x_(x), y_(y) {}

Vector::Vector(const Vector &other) : x_(other.x_), y_(other.y_) {}

Vector &Vector::operator=(const Vector &other) {
  if (this != &other) {
    x_ = other.x_;
    y_ = other.y_;
  }
  return *this;
}

Number Vector::getRho() const {
  double x_val = x_.getValue();
  double y_val = y_.getValue();
  return Number(std::sqrt(x_val * x_val + y_val * y_val));
}

Number Vector::getTheta() const {
  double x_val = x_.getValue();
  double y_val = y_.getValue();
  return Number(std::atan2(y_val, x_val));
}

Vector Vector::operator+(const Vector &other) const {
  return Vector(x_ + other.x_, y_ + other.y_);
}

Number Vector::getX() const { return x_; }

Number Vector::getY() const { return y_; }
