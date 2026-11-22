#include <iostream>
#include <map>
#include "MyAllocator.h"
#include "MyList.h"

int factorial(int n) { // факториал числа
    int res = 1;
    for (int i = 2; i <= n; i++) {
        res *= i;
    }
    return res;
}

int main() {
    std::map<int, int> m1; // 1. map со стандартным аллокатором
    for (int i = 0; i < 10; i++) { // 2. ключ от 0 до 9, значение факториал ключа
        m1[i] = factorial(i);
    }

    std::map<int, int, std::less<int>, MyAllocator<std::pair<const int, int>, 10>> m2; // 3. map с моим аллокатором на 10 элементов
    for (int i = 0; i < 10; i++) { // 4. то же заполнение
        m2[i] = factorial(i);
    }

    std::cout << "----map с MyAllocator----" << std::endl; // 5. вывод ключ и значение через пробел
    for (const auto& [key, value] : m2) {
        std::cout << key << " " << value << std::endl;
    }

    MyList<int> l1; // 6. мой контейнер со стандартным аллокатором
    for (int i = 0; i < 10; i++) { // 7. значения от 0 до 9
        l1.push_back(i);
    }

    MyList<int, MyAllocator<int, 10>> l2; // 8. мой контейнер с моим аллокатором на 10 элементов
    for (int i = 0; i < 10; i++) { // 9. значения от 0 до 9
        l2.push_back(i);
    }

    std::cout << "----MyList с MyAllocator----" << std::endl; // 10. вывод всех значений
    for (const auto& value : l2) {
        std::cout << value << std::endl;
    }

    std::cout << "----статистика пулов при уничтожении----" << std::endl;
}
