# Lin-Q

[English](README.md) | **简体中文**

一个为 C++ 设计的，简单、纯头文件、易于使用的高性能类 LINQ 查询库。🚀

本库为处理集合提供了一套强大的查询操作，其设计灵感来源于 .NET 的语言集成查询 (LINQ)。

## ✨ 项目亮点

- **📑 纯头文件**: 只需引入 `LinQ.hpp` 即可开始使用。
- **⌛ 延迟执行**: 像 `Where` 和 `Select` 这样的操作，会被推迟到真正需要结果时执行。
- **🎩 强大的接口**:
  - 筛选: `Where`
  - 投影: `Select`
  - 排序: `OrderBy`, `OrderByDescending`, `ThenBy`
  - 聚合: `Sum`, `Count`, `Average`, `Min`, `Max`
  - 元素: `First`, `FirstOrDefault`, `Last`, `LastOrDefault`, `ElementAt`
  - 量词: `Any`, `All`
  - 集合: `Distinct`, `Union`, `Intersect`, `Except`
  - 转换: `ToVector`, `ToMap`, `ToUnorderedMap`, `ToArray`, `ToCArray`
  - ...

## 🛠️ 构建项目

### 使用 xmake

```bash
xmake
xmake run example
```

### 使用 CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
# 运行可执行文件
./Release/example.exe
```

## 🚀 使用示例

```cpp
#include "LinQ.hpp"
#include <iostream>
#include <string>
#include <vector>

int main() {
  // 整数数组测试
  int intArray[] = {1, 2, 3, 4, 5};
  auto intQuery = LinQ::From(intArray)
                    .Select([](int x) { return x * x; });

  for (auto x : intQuery) {
    std::cout << x << " "; // 输出：1 4 9 16 25
  }
  std::cout << std::endl;

  // 字符串向量测试
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
    std::cout << s << " "; // 输出：CHERRY BANANA APPLE
  }
  std::cout << std::endl;

  // ToMap 单参数测试
  struct Person {
    int id;
    std::string name;
  };
  std::vector<Person> people = {{1, "ELDment"}, {2, "Ambr0se"}, {3, "利世"}};

  auto personMap = LinQ::From(people).ToMap([](const Person& p) { return p.id; });

  for (const auto& pair : personMap) {
    std::cout << "[" << pair.first << ": " << pair.second.name << "] "; // 输出：[1: ELDment] [2: Ambr0se] [3: 利世]
  }
  std::cout << std::endl;

  return 0;
}
```

## 🤝 贡献

欢迎提交贡献、问题和功能请求！