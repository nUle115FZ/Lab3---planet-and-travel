#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdexcept>

/**
 * Абстрактный класс-интерфейс последовательности элементов
 * Предоставляет базовые операции для работы с коллекциями
 */
template <typename T>
class Sequence {
public:
    virtual ~Sequence() = default;

    //получение элемента по индексу
    virtual T Get(int index) const = 0;
    
    //получение первого элемента
    virtual T GetFirst() const = 0;
    
    //получение последнего элемента
    virtual T GetLast() const = 0;
    
    //получение размера последовательности
    virtual int GetSize() const = 0;
    
    //проверка на пустоту
    virtual bool IsEmpty() const {
        return GetSize() == 0;
    }
    
    //добавление элемента в конец
    virtual void Append(const T& item) = 0;
    
    //добавление элемента в начало
    virtual void Prepend(const T& item) = 0;
    
    //вставка элемента по индексу
    virtual void InsertAt(const T& item, int index) = 0;
    
    //удаление элемента по индексу
    virtual void RemoveAt(int index) = 0;
    
    //установка значения элемента по индексу
    virtual void Set(int index, const T& value) = 0;
    
    //очистка последовательности
    virtual void Clear() = 0;
};

#endif //sEQUENCE_H

