#include <iostream>
#include <cmath>

const int SUCCESS = 0;
const int ERROR = 1;

int main() {
    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
    std::cout << "Enter coefficients a, b and c for 'ax^2 + bx + c = 0': ";
    if (!(std::cin >> a >> b >> c)) {
        std::cout << "ERROR INPUT" << std::endl;
        return ERROR;
    }
    float Discriminant = std::pow(b, 2) - 4 * a * c;
    if (Discriminant > 0) {
        float x1 = (-b + std::sqrt(Discriminant)) / (2 * a);
        float x2 = (-b - std::sqrt(Discriminant)) / (2 * a);
        std::cout << "Two roots: x1 = " << x1 << std::endl << "x2 = " << x2 << std::endl;
    } else if (Discriminant == 0) {
        float x = (-b / (2 * a));
        std::cout << "Root: x = " << x << std::endl;
    } else {
        std::cout << "No real roots (Discriminant is zero)" << std::endl;
    }
    return SUCCESS;
}