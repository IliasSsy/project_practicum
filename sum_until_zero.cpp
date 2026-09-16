#include <iostream>

const int SUCCESS = 0;
const int ERROR = 1;

int main() {
    std::cout << "Введите целые числа (введите 0 для окончания):" << std::endl;
    int number = 0;
    int count = 0;
    long long sum = 0;
    while (std::cin >> number && number != 0) {
        sum += number;
        count++;
    }
    if (!std::cin) {
        std::cout << "INVALID INPUT" << std::endl;
        return ERROR;
    }
    if (count == 0) {
        std::cout << "ERROR: First number cannot be 0!" << std::endl;
        return ERROR;
    }
    std::cout << "Количество чисел: " << count << std::endl;
    std::cout << "Сумма чисел: " << sum << std::endl;
    std::cout << "Гистограмма: ";
    for (int i = 0; i < count; i++) {
        std::cout << "*";
    }
    std::cout << std::endl;
    return SUCCESS;
}
