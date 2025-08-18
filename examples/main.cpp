#include "LinQ.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>

int main() {
    // 整数数组测试
    int intArray[] = {1, 2, 3, 4, 5};
    auto result = LinQ::From(intArray).Select([](int x) { return x * x; });
    for (auto x : result) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // 字符串向量测试
    std::vector<std::string> words = {"apple", "banana", "cherry", "date", "fig"};
    auto stringQuery = LinQ::From(words)
                            .Where([](const std::string& s) { return s.length() > 4; })
                            .Select([](const std::string& s) {
                                std::string upper_s = s;
                                std::transform(upper_s.begin(), upper_s.end(), upper_s.begin(), ::toupper);
                                return upper_s;
                            })
                            .OrderByDescending([](const std::string& s) { return s; });

    for (const auto& s : stringQuery) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    // ToMap 测试
    auto wordMap = LinQ::From(words).ToMap(
        [](const std::string& s) { return s; },
        [](const std::string& s) { return s.length(); }
    );

    for (const auto& pair : wordMap) {
        std::cout << "[" << pair.first << ": " << pair.second << "] ";
    }
    std::cout << std::endl;

    // ToMap 单参数测试
    struct Person {
        int id;
        std::string name;
    };
    std::vector<Person> people = {{1, "Alice"}, {2, "Bob"}, {3, "Charlie"}};

    auto personMap = LinQ::From(people).ToMap([](const Person& p) { return p.id; });

    for (const auto& pair : personMap) {
        std::cout << "[" << pair.first << ": " << pair.second.name << "] ";
    }
    std::cout << std::endl;

    // ToArray 测试
    auto evenNumbers = LinQ::From(intArray)
                            .Where([](int x) { return x % 2 == 0; })
                            .ToArray();

    for (const auto& num : evenNumbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // ToCArray 测试
    auto [cArray, size] = LinQ::From(intArray)
                                .Where([](int x) { return x > 2; })
                                .ToCArray();

    if (cArray) {
        for (size_t i = 0; i < size; ++i) {
            std::cout << cArray[i] << " ";
        }
    }
    std::cout << std::endl;

    return 0;
}