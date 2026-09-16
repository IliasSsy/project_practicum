#include <iostream>

const int SUCCESS = 0;

int main() {
    float a = 0.0f;
    float b = 0.0f;

    std::cout << "Enter coefficients a and b for 'ax + b = 0': ";
    std::cin >> a >> b;

    if (a != 0.0f) {
        float x = -b / a;
        std::cout << "Solution x = " << x << std::endl;
    }
    else {
        std::cout << "Error: coefficient 'a' cannot be zero." << std::endl;
    }
    return SUCCESS;
}