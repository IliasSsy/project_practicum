#include <iostream>

const int ERROR = 1;
const int SUCCESS = 0;

int main() {
    std::cout << "Введите 3 целочисленных значения" << std::endl;
    int a = 0;
    int b = 0;
    int c = 0;

    if (!(std::cin >> a >> b >> c)) {
        std::cout << "ERROR INPUT" << std::endl;
        return ERROR;
    }
    long long result = a + b + c;
    std::cout << "Сумма чисел: " << result << std::endl;

}