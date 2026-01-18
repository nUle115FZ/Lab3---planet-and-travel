#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "DynamicArray.h"
#include <functional>
#include <utility>

/**
 * Очередь с приоритетом, реализованная как min-heap
 * T - тип элемента, K - тип приоритета
 * Элементы с меньшим приоритетом извлекаются первыми
 */
template <typename T, typename K>
class PriorityQueue : private DynamicArray<std::pair<T, K>> {
private:
    using Base = DynamicArray<std::pair<T, K>>;
    std::function<bool(K, K)> comparator; //компаратор для приоритетов
    
    //получить индекс родителя
    int parent(int i) const {
        return (i - 1) / 2;
    }
    
    //получить индекс левого потомка
    int leftChild(int i) const {
        return 2 * i + 1;
    }
    
    //получить индекс правого потомка
    int rightChild(int i) const {
        return 2 * i + 2;
    }
    
    //восстановление свойства кучи снизу вверх
    void heapifyUp(int index) {
        while (index > 0 && comparator(Base::Get(index).second, Base::Get(parent(index)).second)) {
            std::pair<T, K> temp = Base::Get(index);
            Base::Set(index, Base::Get(parent(index)));
            Base::Set(parent(index), temp);
            index = parent(index);
        }
    }
    
    //восстановление свойства кучи сверху вниз
    void heapifyDown(int index) {
        int minIndex = index;
        int left = leftChild(index);
        int right = rightChild(index);
        
        if (left < Base::GetSize() && comparator(Base::Get(left).second, Base::Get(minIndex).second)) {
            minIndex = left;
        }
        
        if (right < Base::GetSize() && comparator(Base::Get(right).second, Base::Get(minIndex).second)) {
            minIndex = right;
        }
        
        if (index != minIndex) {
            std::pair<T, K> temp = Base::Get(index);
            Base::Set(index, Base::Get(minIndex));
            Base::Set(minIndex, temp);
            heapifyDown(minIndex);
        }
    }

public:
    //конструктор с компаратором (по умолчанию - min-heap)
    explicit PriorityQueue(std::function<bool(K, K)> comp = std::less<K>()) 
        : Base(), comparator(comp) {}
    
    //добавление элемента с приоритетом
    void Enqueue(const T& item, const K& priority) {
        Base::Append(std::make_pair(item, priority));
        heapifyUp(Base::GetSize() - 1);
    }
    
    //извлечение элемента с наивысшим приоритетом
    T Dequeue() {
        if (IsEmpty()) {
            throw std::out_of_range("Priority queue is empty");
        }
        
        T result = Base::Get(0).first;
        Base::Set(0, Base::Get(Base::GetSize() - 1));
        Base::RemoveAt(Base::GetSize() - 1);
        
        if (!IsEmpty()) {
            heapifyDown(0);
        }
        
        return result;
    }
    
    //просмотр элемента с наивысшим приоритетом без извлечения
    T Peek() const {
        if (IsEmpty()) {
            throw std::out_of_range("Priority queue is empty");
        }
        return Base::Get(0).first;
    }
    
    //просмотр приоритета верхнего элемента
    K PeekPriority() const {
        if (IsEmpty()) {
            throw std::out_of_range("Priority queue is empty");
        }
        return Base::Get(0).second;
    }
    
    //проверка на пустоту
    bool IsEmpty() const {
        return Base::GetSize() == 0;
    }
    
    //получение размера
    int GetSize() const {
        return Base::GetSize();
    }
    
    //очистка очереди
    void Clear() {
        Base::Clear();
    }
    
    //скрытие методов базового класса Sequence
private:
    using Base::Append;
    using Base::Prepend;
    using Base::InsertAt;
    using Base::RemoveAt;
    using Base::Get;
    using Base::GetFirst;
    using Base::GetLast;
    using Base::Set;
};

#endif //pRIORITY_QUEUE_H

