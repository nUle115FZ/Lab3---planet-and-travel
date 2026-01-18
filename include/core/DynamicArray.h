#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "Sequence.h"
#include <algorithm>
#include <cstring>

/**
 * Динамический массив - реализация Sequence на основе массива
 * Поддерживает автоматическое увеличение размера
 */
template <typename T>
class DynamicArray : public Sequence<T> {
private:
    T* data;
    int size;
    int capacity;
    
    //увеличение емкости массива
    void resize(int newCapacity) {
        T* newData = new T[newCapacity];
        for (int i = 0; i < size; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
    
    //проверка индекса на валидность
    void checkIndex(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
    }

public:
    //конструктор по умолчанию
    DynamicArray() : data(nullptr), size(0), capacity(0) {}
    
    //конструктор с начальной емкостью
    explicit DynamicArray(int initialCapacity) 
        : data(new T[initialCapacity]), size(0), capacity(initialCapacity) {}
    
    //конструктор копирования
    DynamicArray(const DynamicArray& other) 
        : data(new T[other.capacity]), size(other.size), capacity(other.capacity) {
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }
    
    //оператор присваивания
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            capacity = other.capacity;
            data = new T[capacity];
            for (int i = 0; i < size; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }
    
    //деструктор
    ~DynamicArray() override {
        delete[] data;
    }
    
    //получение элемента по индексу
    T Get(int index) const override {
        checkIndex(index);
        return data[index];
    }
    
    //получение первого элемента
    T GetFirst() const override {
        if (size == 0) {
            throw std::out_of_range("Array is empty");
        }
        return data[0];
    }
    
    //получение последнего элемента
    T GetLast() const override {
        if (size == 0) {
            throw std::out_of_range("Array is empty");
        }
        return data[size - 1];
    }
    
    //получение размера
    int GetSize() const override {
        return size;
    }
    
    //добавление в конец
    void Append(const T& item) override {
        if (size == capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        data[size++] = item;
    }
    
    //добавление в начало
    void Prepend(const T& item) override {
        InsertAt(item, 0);
    }
    
    //вставка по индексу
    void InsertAt(const T& item, int index) override {
        if (index < 0 || index > size) {
            throw std::out_of_range("Index out of range");
        }
        if (size == capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        for (int i = size; i > index; i--) {
            data[i] = data[i - 1];
        }
        data[index] = item;
        size++;
    }
    
    //удаление по индексу
    void RemoveAt(int index) override {
        checkIndex(index);
        for (int i = index; i < size - 1; i++) {
            data[i] = data[i + 1];
        }
        size--;
    }
    
    //установка значения
    void Set(int index, const T& value) override {
        checkIndex(index);
        data[index] = value;
    }
    
    //очистка массива
    void Clear() override {
        size = 0;
    }
    
    //оператор индексации
    T& operator[](int index) {
        checkIndex(index);
        return data[index];
    }
    
    const T& operator[](int index) const {
        checkIndex(index);
        return data[index];
    }
};

#endif //dYNAMIC_ARRAY_H

