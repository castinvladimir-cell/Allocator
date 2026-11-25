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
    std::map<int, int> m1;          // 1. map со стандартным аллокатором
    for (int i = 0; i < 10; i++) { // 2. ключ от 0 до 9, значение факториал ключа
        m1[i] = factorial(i);
    }

    std::map<int, int, std::less<int>, MyAllocator<std::pair<const int, int>, 10>> m2; // 3. map с моим аллокатором на 10 элементов
    for (int i = 0; i < 10; i++) {                                                    // 4. то же заполнение
        m2[i] = factorial(i);
    }

    std::cout << "----map с MyAllocator----" << std::endl; // 5. вывод ключ и значение через пробел
    for (const auto& [key, value] : m2) {
        std::cout << key << " " << value << std::endl;
    }

    MyList<int> l1;                 // 6. мой контейнер со стандартным аллокатором
    for (int i = 0; i < 10; i++) { // 7. значения от 0 до 9
        l1.push_back(i);
    }

    MyList<int, MyAllocator<int, 10>> l2; // 8. мой контейнер с моим аллокатором на 10 элементов
    for (int i = 0; i < 10; i++) {       // 9. значения от 0 до 9
        l2.push_back(i);
    }

    std::cout << "----MyList с MyAllocator----" << std::endl; // 10. вывод всех значений
    for (auto it = l2.cbegin(); it != l2.cend(); ++it) {     // обход только на чтение
        std::cout << *it << std::endl;
    }

    decltype(m2) m3;      // доп. проверка флагов: map с моим аллокатором и своим пулом
    m3[100] = 1;
    m3 = std::move(m2); // перемещение: m3 забирает узлы m2 вместе с его аллокатором

    decltype(m2) m4;  // ещё один map со своим пулом
    for (int i = 0; i < 5; i++) {
        m4[i] = i * i;
    }
    m3.swap(m4); // swap: узлы и аллокаторы меняются местами

    std::cout << "----после перемещения и swap----" << std::endl;
    std::cout << "m3:";
    for (const auto& [key, value] : m3) {
        std::cout << " " << key << "-" << value;
    }
    std::cout << std::endl << "m4:";
    for (const auto& [key, value] : m4) {
        std::cout << " " << key << "-" << value;
    }
    std::cout << std::endl;

    std::cout << "----статистика пулов при уничтожении----" << std::endl;
}
