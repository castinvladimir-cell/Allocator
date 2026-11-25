#pragma once

#include <cstddef> 
#include <new>
#include <memory> 
#include <type_traits>
#include <iostream>

template <typename T, size_t N> 
struct mempool {
    struct Nodik {
        Nodik* next_ = nullptr; 
    };
    static constexpr size_t size = sizeof(T) > sizeof(Nodik) ? sizeof(T) : sizeof(Nodik);
    Nodik* blocks = nullptr; 
    Nodik* returned = nullptr;                                      // вернувшиеся после деаллокейт
    size_t allocated = 0, released = 0, givenA = 0, fromheap = 0;  // выделено, возвращено, отдано уже

    void printccounted() {
        if (allocated > 0 ) {
            std::cout << "Количество вернувшихся байт - "             << released  << "\n"; 
            std::cout << "Количество байт попрошенных контейнером - " << allocated << "\n";
            std::cout << "Количество байт взятых у кучи - "           << fromheap  << "\n";
        } else {
            std::cout << "Количество байт попрошенных контейнером - " << allocated << "\n";
        }
    }

    mempool() = default; 

    mempool(const mempool&) = delete;             // Запрет конст. копирования 
    mempool& operator=(const mempool&) = delete;

    ~mempool() {
        Nodik* curr = blocks;
        Nodik* ptratnext = nullptr;
        while (curr != nullptr)
        {
            ptratnext = curr -> next_;
            ::operator delete(curr);
            curr = ptratnext;
        }    
        printccounted();
    }
}; 

template <typename T, size_t N> 
class MyAllocator {
private: 
    std::shared_ptr<mempool<T, N>> pool_; // Указатель на пул
    using Nodik = typename mempool<T, N>::Nodik;

public:
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap            = std::true_type;
    using value_type                             = T;

    template <typename U>
    struct rebind { // Поменять тип 
        using other = MyAllocator<U, N>;
    };
    //1
    bool operator==(const MyAllocator<T, N>& other) const { return other.pool_ == pool_; }
    bool operator!=(const MyAllocator<T, N>& other) const { return other.pool_ != pool_; }
    //2
    template <typename U>
    bool operator==(const MyAllocator<U, N>&) const { return false; }
    template <typename U>
    bool operator!=(const MyAllocator<U, N>&) const { return true; }

    MyAllocator() : pool_(std::make_shared<mempool<T, N>>()) { }          // создаём новый пул
    MyAllocator(const MyAllocator&) = default;                           // Копия делит тот же пул
    MyAllocator& operator=(const MyAllocator&) = default;               // присваивание тоже делит пул
    MyAllocator(MyAllocator&& other) noexcept : pool_(other.pool_) { } // Перемещение копирует указатель. у other пул остаётся
    template <typename U>
    MyAllocator(const MyAllocator<U, N>&) noexcept : pool_(std::make_shared<mempool<T, N>>()) {  } // из аллокатора другого типа. Аллокатор для узла

    T* allocate(size_t amount) {                  // память под amount объектов, без создания объектов
        const size_t bytes = amount * sizeof(T); // сколько байт просят
        pool_ -> allocated += bytes;
        if (amount == 1) {
            if (pool_ -> returned != nullptr) {// вернувшиеся есть 
                auto curr = pool_ -> returned;
                pool_ -> returned = pool_ -> returned -> next_; 
                return reinterpret_cast<T*>(curr);
            } else if (pool_ -> blocks == nullptr || pool_ -> givenA == N) {   // Выданы все чо были или буферов немаэ
                pool_ -> fromheap += (N + 1) * mempool<T, N>::size;
                auto curr = static_cast<Nodik*>(::operator new((N + 1) * mempool<T, N>::size));
                curr -> next_ = pool_ -> blocks; 
                pool_-> blocks = curr;
                pool_ -> givenA = 0;
            } 
            auto start = reinterpret_cast<std::byte*>(pool_ -> blocks); 
            auto res = start + (pool_ -> givenA + 1) * mempool<T, N>::size; // считаем адрес
            pool_ -> givenA++;
            return reinterpret_cast<T*>(res);
        } else {
            pool_ -> fromheap += bytes;
            return static_cast<T*>(::operator new(bytes));
        }
    }

    void deallocate(T* ptrfromalloc, size_t amount) { // Возвращается в пул
        const size_t bytes = amount * sizeof(T);     // сколько байт возвращают
        pool_ -> released += bytes;
        if (amount == 1) {
            auto curr = reinterpret_cast<Nodik*>(ptrfromalloc);
            curr -> next_ = pool_ -> returned;    
            pool_-> returned = curr;  
        } else {
            ::operator delete(ptrfromalloc);
        }       
    }
};