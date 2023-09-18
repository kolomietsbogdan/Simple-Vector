#pragma once
#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <string>
#include "array_ptr.h"
#include <utility>
#include <iterator>

struct ReserveProxyObj
{
    
    ReserveProxyObj(size_t cap) : capacity(cap) {}
   
    size_t capacity;
};
 
 
template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
 
    SimpleVector() noexcept : size_(0), capacity_(0), arr_(nullptr) {}
 
   
 
    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : size_(size), capacity_(size) {
        ArrayPtr<Type> new_arr_(size);
        arr_.swap(new_arr_);
        Iterator beginn = arr_.Get();
        std::fill(beginn, beginn + size, Type{});
    }
 
    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value): size_(size), capacity_(size) {
        ArrayPtr<Type> new_arr_(size);
        arr_.swap(new_arr_);
        Iterator beginn = arr_.Get();
 
        std::fill(beginn, beginn + size, value);
 
    }
 
    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        if (init.size() == 0) {
            size_ = 0;
            capacity_ = 0;
 
            ArrayPtr<Type> new_arr_(size_); // инициализация поля arr_ значением nullptr
            arr_.swap(new_arr_);
        }
        else {
        size_ = init.size();
        ArrayPtr<Type> new_arr_(size_); // инициализация поля arr_ значением nullptr
        arr_.swap(new_arr_);
        size_t clock = 0;
        for (auto memb: init) {
            arr_[clock] = memb;
            clock++;
        }
        capacity_  = size_;
        }
    }
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this == &rhs) {
            return *this;
        }
        SimpleVector temp(rhs);
        swap(temp);
        return *this;
    }
 
    SimpleVector(ReserveProxyObj obj) {
        capacity_ = obj.capacity;
        size_ = 0;
        ArrayPtr<Type> new_arr(static_cast<size_t>(0));
        arr_.swap(new_arr);
 
    }
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
        
    }
 
    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
       
    }
 
    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        if (size_ == 0) {
            return true;
        }
        else {
            return false;
        }
        
    }
 
    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
       return arr_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
       return arr_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range ("blah");;
        }
        else {
            return arr_[index];
        }
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range ("blah");;
        }
        else {
            return arr_[index];
        }
    
    }
 
    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
       size_ = 0;
    }
 
    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_arr_(new_capacity);
            std::move(arr_.Get(), arr_.Get()+size_, new_arr_.Get());
            for (size_t i = size_; i < new_capacity; ++i) {
                new_arr_[i] = Type();  // Initialize new elements with default value
            }
            arr_.swap(new_arr_);
            capacity_ = new_capacity;
        }
        if (new_capacity < size_) std::fill(arr_.Get() + new_capacity, arr_.Get() + size_, 0);
        size_ = new_capacity;
    }
 
    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return arr_.Get();
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        auto result = arr_.Get() + size_;
        return result;
    }
 
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
       return arr_.Get();
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        auto result = arr_.Get() + size_;
        return result;
    }
 
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return arr_.Get();
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        auto result = arr_.Get() + size_;
        return result;
    }
 
    SimpleVector(const SimpleVector& other) {
       ArrayPtr<Type> new_arr_(other.size_);
       arr_.swap(new_arr_);
       size_ = other.size_;
       capacity_ = other.capacity_;
       std::copy(other.arr_.Get(), other.arr_.Get() + other.size_, arr_.Get());
       
    }
 
 
    SimpleVector(SimpleVector&& other) {
 
       std::swap(size_, other.size_);
       std::swap(capacity_, other.capacity_);
       arr_.swap(other.arr_);
       other.Clear();      
    }
 
    SimpleVector& operator=(SimpleVector&& rhs) {
        if (*this == rhs) {
            return *this;
        }
            if (*this == rhs) {
            return *this;
        }
        SimpleVector temp(std::move(rhs));
        swap(temp);
        return *this;
 
    }
 
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
       if (size_ < capacity_) {
        arr_[size_]  = item;
        size_++;
       }
       else {
            if (capacity_ > 0) {
            capacity_ *= 2;
            }
            else {
                capacity_ = size_ * 2;
            }
            Type* new_arr_ = new Type[std::move(capacity_)];
            std::move(arr_.Get(), arr_.Get() + size_, new_arr_);
            new_arr_[size_] =item;
            ArrayPtr<Type> new_arr2_(new_arr_);
            arr_.swap(new_arr2_);
            size_ += 1;
       }
    }
 
    void PushBack(Type&& item) {
       if (size_ < capacity_) {
        arr_[size_]  = std::move(item);
        size_++;
       }
       else {
            capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
            Type* new_arr_ = new Type[std::move(capacity_)];
            std::move(arr_.Get(), arr_.Get() + size_, new_arr_);
            new_arr_[size_] = std::move(item);
            ArrayPtr<Type> new_arr2_(std::move(new_arr_));
            arr_.swap(new_arr2_);
            size_ += 1;
       }
    }   
 
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
     Iterator Insert(ConstIterator pos, const Type& value) {
        size_t index = pos - arr_.Get();
        if (size_ < capacity_) {
            std::copy_backward(arr_.Get() + index, arr_.Get() + size_, arr_.Get() + size_ + 1);
            arr_[index] = value;
            size_++;
            return arr_.Get() + index; 
        }
        else {
            Type* new_arr_ = new Type [size_ + 1];
            std::copy(arr_.Get(), arr_.Get() + index, new_arr_);
            new_arr_[index] = value;
            std::copy(arr_.Get() + index, arr_.Get() + size_, new_arr_ + index + 1);
            size_++;
            
            if (capacity_ > 0) {
            capacity_ *= 2;
            }
            else {
                capacity_ = size_ * 2;
            }
            ArrayPtr<Type> new_arr2_(new_arr_);
            arr_.swap(new_arr2_);
            return arr_.Get() + index; 
        }
    }
 
 
 
 
    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t index = pos - arr_.Get();
        if (size_ < capacity_) {
            std::move_backward(arr_.Get() + index, arr_.Get() + size_, arr_.Get() + size_ + 1);
            arr_[index] = std::move(value);
            size_++;
            return arr_.Get() + index; 
        }
        else {
            Type* new_arr_ = new Type [size_ + 1];
            std::move(arr_.Get(), arr_.Get() + index, new_arr_);
            new_arr_[index] = std::move(value);
            std::move(arr_.Get() + index, arr_.Get() + size_, new_arr_ + index + 1);
            size_++;
            
            if (capacity_ > 0) {
            capacity_ *= 2;
            }
            else {
                capacity_ = size_ * 2;
            }
            ArrayPtr<Type> new_arr2_(std::move(new_arr_));
            arr_.swap(new_arr2_);
            return arr_.Get() + index; 
        }
    }
 
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        size_--;
    }
 
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
      auto index = pos - arr_.Get();
      std::move(arr_.Get() + index + 1, arr_.Get() + size_, arr_.Get() + index);
      size_--;
      return arr_.Get() + index;
    }
 
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        arr_.swap(other.arr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
 
    void Reserve(size_t new_capacity) {
       if (capacity_ > new_capacity) {
            return;
       }
       if (capacity_ == 0) {
        capacity_ = new_capacity;
       }
       else {
            ArrayPtr<Type> new_arr(new_capacity);
            capacity_ = new_capacity;
            std::copy(arr_.Get(), arr_.Get() + size_, new_arr.Get());
            arr_.swap(new_arr);
 
        }
        
    }
 
private:
 
    size_t size_;
    size_t capacity_;
    ArrayPtr<Type> arr_;
};
 
 
template <typename Type>
bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    else {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
}
 
template <typename Type>
bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}
 
template <typename Type>
bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}
 
template <typename Type>
bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return  rhs < lhs;
}
 
template <typename Type>
bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
 
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

