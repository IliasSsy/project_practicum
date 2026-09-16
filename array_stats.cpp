#include <iostream>
#include <vector>
#include <limits>

const int SUCCESS = 0;
const int ERROR = 1;

int main () {
    std::cout << "Enter integers" << std::endl;
    int digit = 0;
    std::vector<int> numbers;
    while (std::cin >> digit) {
        numbers.push_back(digit);
    }
    if (!(std::cin.eof())) {
        std::cout << "INVALID INPUT" << std::endl;
        return ERROR;
    }
    long long result = 0;
    int max = std::numeric_limits<int>::min();
    int min = std::numeric_limits<int>::max();
    for (int i = 0; i < numbers.size(); i++) {
        if (numbers[i] > 0 && result > std::numeric_limits<long long>::max() - numbers[i]) {
            std::cout << "Sum too mach overflowed long long type" << std::endl;
            return ERROR;
        }
        if (numbers[i] < min) {
            min = numbers[i];
        }
        if (numbers[i] > max) {
            max = numbers[i];
        }
        result += numbers[i];
    }
    double arifmeticMean = 0.0f;
    if (!(numbers.empty())) {
        arifmeticMean = static_cast<double>(result) / numbers.size();
    }
    std::cout << "Sum is: " << result << std::endl;
    std::cout << "Arifmetic mean is: " << arifmeticMean << std::endl;
    if (!(numbers.empty())) {
        std::cout << "Max integer is: " << max << std::endl;
        std::cout << "Min integer is: " << min << std::endl;
    } else {
        std::cout << "Max integer: none"  << std::endl;
        std::cout << "Min integer: none"  << std::endl;
    }

    return SUCCESS;
}