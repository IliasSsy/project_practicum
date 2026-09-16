#include <iostream>
#include <vector>
#include <limits>

const int SUCCESS = 0;
const int ERROR = 1;

int main () {
    std::cout << "Enter the number of elements N (N > 0): ";
    int n = 0;
    if (!(std::cin >> n) || n <= 0) {
        std::cout << "INVALID INPUT" << std::endl;
        return ERROR;
    }
    std::cout << "Enter " << n << " integers:" << std::endl;
    std::vector<int> numbers;
    numbers.reserve(n);
    for (int i = 0; i < n; i++) {
        int digit = 0;
        if (!(std::cin >> digit)) {
            std::cout << "INVALID INPUT" << std::endl;
            return ERROR;
        }
        numbers.push_back(digit);
    }
    long long sum = 0;
    int max = std::numeric_limits<int>::min();
    int min = std::numeric_limits<int>::max();
    for (int i = 0; i < numbers.size(); i++) {
        if (numbers[i] > 0 && sum > std::numeric_limits<long long>::max() - numbers[i]) {
            std::cout << "Sum too much overflowed long long type" << std::endl;
            return ERROR;
        }
        if (numbers[i] < 0 && sum < std::numeric_limits<long long>::min() - numbers[i]) {
            std::cout << "Sum too much overflowed long long type" << std::endl;
            return ERROR;
        }
        if (numbers[i] < min) {
            min = numbers[i];
        }
        if (numbers[i] > max) {
            max = numbers[i];
        }
        sum += numbers[i];
    }
    float arithmeticMean = static_cast<float>(sum) / numbers.size();
    std::cout << "Sum is: " << sum << std::endl;
    std::cout << "Arifmetic mean is: " << arithmeticMean << std::endl;
    std::cout << "Max integer is: " << max << std::endl;
    std::cout << "Min integer is: " << min << std::endl;
    return SUCCESS;
}
