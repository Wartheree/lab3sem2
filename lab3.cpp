//
// Created by danil on 19.03.2026.
//
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <unordered_map>


template<class T>
class MyUnique {
private:
    T* p = nullptr;
    static inline std::unordered_set<T*> v;

public:
    MyUnique(T* ptr) {
        if (v.count(ptr)==0) {
            p=ptr;
            v.insert(ptr);
        }
        else throw std::runtime_error("Not unique");
    }

    ~MyUnique() {
        v.erase(p);
    }

    MyUnique(const MyUnique&) = delete;

    MyUnique (MyUnique && rhs) {
        p=rhs.p;
        rhs.p=nullptr;
    }

    MyUnique& operator= (MyUnique && rhs) {
        if (p != rhs.p) {
            p=rhs.p;
            rhs.p=nullptr;
        }
        return *this;
    }

    MyUnique& operator=(const MyUnique&) = delete;

    T* get() const{
        return p;
    }

    T& operator* () {
        return *p;
    }

    T* operator-> () {
        return p;
    }
};

template<class T>
class MyShared {
private:
    T* p = nullptr;
    static inline std::unordered_map<T*, int> v;

public:
    MyShared(T* ptr) {
        v[ptr]++;
        p=ptr;
    }

    ~MyShared() {
        if (p!=nullptr) {
            v[p]--;
            if (v[p]==0) v.erase(p);
        }
    }

    MyShared (MyShared && rhs) {
        p=rhs.p;
        rhs.p=nullptr;
    }

    MyShared(const MyShared& rhs) {
        p=rhs.p;
        v[p]++;
    }

    MyShared& operator= (MyShared && rhs) {
        if (p != rhs.p) {
            v[p]--;
            if (v[p]==0) v.erase(p);
            p=rhs.p;
            rhs.p=nullptr;
        }
        return *this;
    }

    MyShared& operator= (const MyShared& rhs) {
        if (p != rhs.p) {
            v[p]--;
            if (v[p]==0) v.erase(p);
            p=rhs.p;
            v[p]++;
        }
        return *this;
    }

    T* get() const{
        return p;
    }

    T& operator* () {
        return *p;
    }

    T* operator-> () {
        return p;
    }

    int count() const{
        if (p==nullptr) return 0;
        return v[p];
    }
};



int main() {
    std::cout << "========== ТЕСТИРОВАНИЕ MyUnique ==========\n";


    std::cout << "\n--- Тест 1: Создание и базовые операции ---\n";
    MyUnique<int> u1(new int(42));
    std::cout << "u1 значение: " << *u1 << std::endl;
    std::cout << "u1 get(): " << u1.get() << std::endl;


    std::cout << "\n--- Тест 2: Проверка уникальности ---\n";
    int* raw_ptr = new int(100);
    MyUnique<int> u2(raw_ptr);
    std::cout << "u2 создан с raw_ptr: " << *u2 << std::endl;

    try {
        MyUnique<int> u3(raw_ptr);
        std::cout << "ОШИБКА: Исключение не выброшено!\n";
    } catch (const std::runtime_error& e) {
        std::cout << "ОК: Поймано исключение: " << e.what() << std::endl;
    }


    std::cout << "\n--- Тест 3: Перемещение ---\n";
    MyUnique<int> u4(new int(777));
    std::cout << "u4 значение: " << *u4 << std::endl;

    MyUnique<int> u5(std::move(u4));
    std::cout << "После перемещения в u5:\n";
    std::cout << "u5 значение: " << *u5 << std::endl;
    std::cout << "u4 get(): " << u4.get() << " (должен быть nullptr)\n";


    std::cout << "\n--- Тест 4: Оператор присваивания перемещением ---\n";
    MyUnique<int> u6(new int(888));
    MyUnique<int> u7(new int(999));

    std::cout << "До перемещения:\n";
    std::cout << "u6: " << *u6 << std::endl;
    std::cout << "u7: " << *u7 << std::endl;

    u7 = std::move(u6);

    std::cout << "После перемещения u7 = std::move(u6):\n";
    std::cout << "u7: " << *u7 << std::endl;
    std::cout << "u6 get(): " << u6.get() << std::endl;


    std::cout << "\n--- Тест 5: Работа с пользовательским типом ---\n";
    struct TestStruct {
        int a;
        double b;
        std::string s;
        void print() const {
            std::cout << "TestStruct{" << a << ", " << b << ", " << s << "}\n";
        }
    };

    MyUnique<TestStruct> u8(new TestStruct{10, 3.14, "привет"});
    std::cout << "Через оператор ->: ";
    u8->print();
    std::cout << "Через оператор *: ";
    (*u8).print();



    std::cout << "\n========== ТЕСТИРОВАНИЕ MyShared ==========\n";

    std::cout << "\n--- Тест 6: Создание и базовые операции ---\n";
    MyShared<int> s1(new int(42));
    std::cout << "s1 значение: " << *s1 << std::endl;
    std::cout << "s1 count(): " << s1.count() << " (должен быть 1)\n";


    std::cout << "\n--- Тест 7: Копирование ---\n";
    MyShared<int> s2(s1); // копирование
    std::cout << "s2 значение: " << *s2 << std::endl;
    std::cout << "s1 значение: " << *s1 << std::endl;
    std::cout << "s1 count(): " << s1.count() << " (должен быть 2)\n";
    std::cout << "s2 count(): " << s2.count() << " (должен быть 2)\n";


    std::cout << "\n--- Тест 8: Присваивание копированием ---\n";
    MyShared<int> s3(new int(999));
    std::cout << "До присваивания s3 = s1:\n";
    std::cout << "s3 значение: " << *s3 << std::endl;
    std::cout << "s3 count(): " << s3.count() << " (должен быть 1)\n";
    std::cout << "s1 count(): " << s1.count() << " (должен быть 2)\n";

    s3 = s1;

    std::cout << "После присваивания:\n";
    std::cout << "s3 значение: " << *s3 << std::endl;
    std::cout << "s1 count(): " << s1.count() << " (должен быть 3)\n";
    std::cout << "s2 count(): " << s2.count() << " (должен быть 3)\n";
    std::cout << "s3 count(): " << s3.count() << " (должен быть 3)\n";


    std::cout << "\n--- Тест 9: Присваивание самому себе ---\n";
    MyShared<int> s4(new int(777));
    std::cout << "s4 count() до присваивания: " << s4.count() << " (должен быть 1)\n";

    s4 = s4;

    std::cout << "s4 count() после присваивания самому себе: " << s4.count() << " (должен остаться 1)\n";


    std::cout << "\n--- Тест 10: Перемещение ---\n";
    MyShared<int> s5(new int(555));
    std::cout << "s5 count(): " << s5.count() << " (должен быть 1)\n";

    MyShared<int> s6(std::move(s5));

    std::cout << "После перемещения:\n";
    std::cout << "s6 count(): " << s6.count() << " (должен быть 1)\n";
    std::cout << "s5 count(): " << s5.count() << " (должен быть 0)\n";


    std::cout << "\n--- Тест 11: Присваивание перемещением ---\n";
    MyShared<int> s7(new int(111));
    MyShared<int> s8(new int(222));

    std::cout << "До присваивания s8 = std::move(s7):\n";
    std::cout << "s7 значение: " << *s7 << std::endl;
    std::cout << "s8 значение: " << *s8 << std::endl;
    std::cout << "s7 count(): " << s7.count() << " (должен быть 1)\n";
    std::cout << "s8 count(): " << s8.count() << " (должен быть 1)\n";

    s8 = std::move(s7);

    std::cout << "После присваивания:\n";
    std::cout << "s8 значение: " << *s8 << std::endl;
    std::cout << "s8 count(): " << s8.count() << " (должен быть 1)\n";
    std::cout << "s7 get(): " << (s7.get() == nullptr ? "nullptr" : "not null") << " (должен быть nullptr)\n";


    std::cout << "\n--- Тест 12: Сложный сценарий с несколькими копиями ---\n";
    MyShared<std::string> ss1(new std::string("Общий ресурс"));
    std::cout << "ss1 count() после создания: " << ss1.count() << " (должен быть 1)\n";

    {
        MyShared<std::string> ss2(ss1);
        MyShared<std::string> ss3(ss2);
        std::cout << "Внутри блока:\n";
        std::cout << "ss1: " << *ss1 << std::endl;
        std::cout << "ss2: " << *ss2 << std::endl;
        std::cout << "ss3: " << *ss3 << std::endl;
        std::cout << "ss1 count(): " << ss1.count() << " (должен быть 3)\n";
        std::cout << "ss2 count(): " << ss2.count() << " (должен быть 3)\n";
        std::cout << "ss3 count(): " << ss3.count() << " (должен быть 3)\n";
    }

    std::cout << "После выхода из блока (ss2 и ss3 уничтожены):\n";
    std::cout << "ss1: " << *ss1 << std::endl;
    std::cout << "ss1 count(): " << ss1.count() << " (должен быть 1)\n";


    std::cout << "\n--- Тест 13: Работа с nullptr ---\n";
    MyShared<int> s9(nullptr);
    std::cout << "s9 count(): " << s9.count() << " (0)\n";


    std::cout << "\n--- Тест 14: Цепочка присваиваний ---\n";
    MyShared<int> s10(new int(1000));
    MyShared<int> s11(new int(2000));
    MyShared<int> s12(new int(3000));

    std::cout << "До цепочки:\n";
    std::cout << "s10 значение: " << *s10 << ", count(): " << s10.count() << std::endl;
    std::cout << "s11 значение: " << *s11 << ", count(): " << s11.count() << std::endl;
    std::cout << "s12 значение: " << *s12 << ", count(): " << s12.count() << std::endl;

    s12 = s11 = s10;

    std::cout << "После s12 = s11 = s10:\n";
    std::cout << "s10 значение: " << *s10 << ", count(): " << s10.count() << " (должен быть 3)\n";
    std::cout << "s11 значение: " << *s11 << ", count(): " << s11.count() << " (должен быть 3)\n";
    std::cout << "s12 значение: " << *s12 << ", count(): " << s12.count() << " (должен быть 3)\n";

    std::cout << "\n========== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ==========\n";

    return 0;
}