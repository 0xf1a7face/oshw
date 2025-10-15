#include <iostream>
#include "number.h"
#include "vector.h"

int main() {
    // Примеры работы с Number
    Number a = Number::create(3.0);
    Number b = Number::create(4.0);
    Number sum = a + b;
    Number prod = a * b;
    Number diff = a - b;
    Number quot = a / b;

    std::cout << "Number examples:\n";
    std::cout << "a = " << a.getValue() << "\n";
    std::cout << "b = " << b.getValue() << "\n";
    std::cout << "a + b = " << sum.getValue() << "\n";
    std::cout << "a * b = " << prod.getValue() << "\n";
    std::cout << "a - b = " << diff.getValue() << "\n";
    std::cout << "a / b = " << quot.getValue() << "\n\n";

    // Примеры работы с Vector
    Vector v1(Number::create(3.0), Number::create(4.0));
    Vector v2 = Vector::ONE;

    Vector v_sum = v1 + v2;

    std::cout << "Vector examples:\n";
    std::cout << "v1 = (" << v1.getX().getValue() << ", " << v1.getY().getValue() << ")\n";
    std::cout << "v2 = (" << v2.getX().getValue() << ", " << v2.getY().getValue() << ")\n";
    std::cout << "v1 + v2 = (" << v_sum.getX().getValue() << ", " << v_sum.getY().getValue() << ")\n";

    Number rho = v1.getRho();
    Number theta = v1.getTheta();
    std::cout << "Polar coordinates of v1:\n";
    std::cout << "rho = " << rho.getValue() << "\n";
    std::cout << "theta = " << theta.getValue() << " rad\n";

    // Проверка констант
    std::cout << "\nConstants:\n";
    std::cout << "Number::ZERO = " << Number::ZERO.getValue() << "\n";
    std::cout << "Number::ONE = " << Number::ONE.getValue() << "\n";
    std::cout << "Vector::ZERO = (" << Vector::ZERO.getX().getValue() << ", " << Vector::ZERO.getY().getValue() << ")\n";
    std::cout << "Vector::ONE = (" << Vector::ONE.getX().getValue() << ", " << Vector::ONE.getY().getValue() << ")\n";

    return 0;
}
