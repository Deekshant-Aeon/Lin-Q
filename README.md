# Lin-Q

**English** | [简体中文](README.zh-CN.md)

A simple, header-only, easy-to-use, and high-performance LINQ-like query library for C++ 🚀

This library provides a powerful set of query operations for collections, inspired by .NET's Language Integrated Query (LINQ)

## ✨ Features

- **📑 Header-only**: Just include `LinQ.hpp` to get started
- **⌛ Deferred Execution**: Operations like `Where` and `Select` are deferred until the results are actually needed
- **🎩 Powerful API**:
  - Filtering: `Where`
  - Projection: `Select`
  - Sorting: `OrderBy`, `OrderByDescending`, `ThenBy`
  - Aggregation: `Sum`, `Count`, `Average`, `Min`, `Max`
  - Element: `First`, `FirstOrDefault`, `Last`, `LastOrDefault`, `ElementAt`
  - Quantifiers: `Any`, `All`
  - Set: `Distinct`, `Union`, `Intersect`, `Except`
  - Conversion: `ToVector`, `ToMap`, `ToUnorderedMap`, `ToArray`, `ToCArray`
  - ...

## 🛠️ Building the Project

### Using xmake

```bash
xmake
xmake run example
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
# Run the executable
./Release/example.exe
```

## 🚀 Usage Example

```cpp
#include "LinQ.hpp"
#include <iostream>
#include <string>
#include <vector>

int main() {
  // Test with an integer array
  int intArray[] = {1, 2, 3, 4, 5};
  auto intQuery = LinQ::From(intArray)
                    .Select([](int x) { return x * x; });

  for (auto x : intQuery) {
    std::cout << x << " "; // Output: 1 4 9 16 25
  }
  std::cout << std::endl;

  // Test with a vector of strings
  std::vector<std::string> stringVector = {"apple", "banana", "cherry", "date", "fig"};
  auto stringQuery = LinQ::From(stringVector)
                          .Where([](const std::string& s) { return s.length() > 4; })
                          .Select([](const std::string& s) {
                            std::string upperString = s;
                            std::transform(upperString.begin(), upperString.end(), upperString.begin(), ::toupper);
                            return upperString;
                          })
                          .OrderByDescending([](const std::string& s) { return s; });

  for (const auto& s : stringQuery) {
    std::cout << s << " "; // Output: CHERRY BANANA APPLE
  }
  std::cout << std::endl;

  // Test ToMap with a single argument
  struct Person {
    int id;
    std::string name;
  };
  std::vector<Person> people = {{1, "ELDment"}, {2, "Ambr0se"}, {3, "利世"}};

  auto personMap = LinQ::From(people).ToMap([](const Person& p) { return p.id; });

  for (const auto& pair : personMap) {
    std::cout << "[" << pair.first << ": " << pair.second.name << "] "; // Output: [1: ELDment] [2: Ambr0se] [3: 利世]
  }
  std::cout << std::endl;

  return 0;
}
```

## 🤝 Contributing

Contributions, issues, and feature requests are welcome!