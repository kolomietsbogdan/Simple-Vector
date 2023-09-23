#pragma once 
#include <cassert> 
#include <initializer_list> 
#include <algorithm> 
#include <stdexcept> 
#include <string> 
#include "array_ptr.h" 
#include <utility> 
#include <iterator> 


class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity)
        : capacity_(capacity)
    {
    }

    size_t GetCapacity() {
        return capacity_;
    }
private:
    size_t capacity_;
};


template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : SimpleVector(size, Type())
    {
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value): arr_(size),
            size_(size), 
			capacity_(size)
			{ 
				ArrayPtr<Type> new_arr_(size); 
				Iterator beginn = arr_.Get(); 
				std::fill(beginn, beginn + size, value); 
			} 

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : arr_(init.size()), size_(init.size()), capacity_(init.size())
    {
        std::copy(init.begin(), init.end(), arr_.Get());
    }
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this == &rhs) 
			{ 
				return * this; 
			}
			SimpleVector temp(rhs); 
			swap(temp); 
			return * this; 
    }

    explicit SimpleVector(ReserveProxyObj obj)
    {
        Reserve(obj.GetCapacity());
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
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) throw std::out_of_range("Index is Out of Range");
        return arr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) throw std::out_of_range("Index is Out of Range");
        return arr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type  
    void Resize(size_t new_size) {
        if (new_size <= capacity_) {
            std::generate(arr_.Get() + size_, arr_.Get() + size_ + new_size, [] () {return Type();});
        }
        if (new_size > capacity_) {
            size_t new_capacity = std::max(new_size, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::generate(temp.Get(), temp.Get() + new_size, [] () {return Type();});// Initialize new elements with default value
            std::move(arr_.Get(), arr_.Get() + capacity_, temp.Get());
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
        size_ = new_size;
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
    
    SimpleVector(const SimpleVector& other): arr_(other.GetSize()),
            size_(other.GetSize()), 
			capacity_(other.GetCapacity())		 
			{ 
				std::copy(other.begin(), other.end(), begin()); 
			} 
 
    SimpleVector(SimpleVector&& other) : arr_(std::move(other.arr_))  
        { 
            size_ = std::exchange(other.size_, 0); 
            capacity_ = std::exchange(other.capacity_, 0); 
        }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ + 1 > capacity_) {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::copy(arr_.Get(), arr_.Get() + size_, temp.Get());
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
        arr_[size_] = item;
        ++size_;
    }

    void PushBack(Type&& item) {
        if (size_ + 1 > capacity_) {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::move(arr_.Get(), arr_.Get() + size_, temp.Get());
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
        arr_[size_] = std::move(item);
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t index = pos - arr_.Get();
        if (capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[index] = value;
            arr_.swap(temp);
            ++capacity_;
        }
        else if (size_ < capacity_) {
            std::copy_backward(arr_.Get() + index, arr_.Get() + size_,
                arr_.Get() + size_ + 1);
            arr_[index] = value;
        }
        else {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(capacity_);
            std::copy(arr_.Get(), arr_.Get() + size_,
                temp.Get());
            std::copy_backward(arr_.Get() + index, arr_.Get() + size_,
                temp.Get() + size_ + 1);
            temp[index] = value;
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
        ++size_;
        return &arr_[index];
    }

    Iterator Insert(Iterator pos, Type&& value) {
        assert (pos >= begin() && pos <= end());
        size_t index = pos - arr_.Get();
        if (capacity_ == 0) {
            ArrayPtr<Type> temp(1);
            temp[index] = std::move(value);
            arr_.swap(temp);
            ++capacity_;
        }
        else if (size_ < capacity_) {
            std::move_backward(arr_.Get() + index, arr_.Get() + size_,
                arr_.Get() + size_ + 1);
            arr_[index] = std::move(value);
        }
        else {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(capacity_);
            std::move(arr_.Get(), arr_.Get() + size_,
                temp.Get());
            std::move_backward(arr_.Get() + index, arr_.Get() + size_,
                temp.Get() + size_ + 1);
            temp[index] = std::move(value);
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
        ++size_;
        return &arr_[index];
    }



    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert (size_ != 0);
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert (pos >= begin() && pos <= end());
        assert (size_ != 0);
        int count = pos - arr_.Get();
        std::move(arr_.Get() + count + 1, arr_.Get() + size_, arr_.Get() + count);
        --size_;
        return &arr_[count];
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        arr_.swap(other.arr_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> temp(new_capacity);
            std::copy(arr_.Get(), arr_.Get() + size_, temp.Get());
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr<Type> arr_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template < typename Type> 
	bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
	{ 
		if (lhs.GetSize() != rhs.GetSize()) 
		{ 
			return false; 
		} 
		else 
		{ 
			return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); 
		} 
	} 
 
template < typename Type> 
	bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
	{ 
		return !(lhs == rhs); 
	} 
 
template < typename Type> 
	bool operator < (const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
	{ 
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); 
	} 
 
template < typename Type> 
	bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
	{ 
		return !(rhs < lhs); 
	} 
 
template < typename Type> 
	bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
	{ 
		return rhs < lhs; 
	} 
 
template < typename Type> 
	bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
	{ 
		return !(lhs < rhs); 
	} 
 
ReserveProxyObj Reserve(size_t capacity_to_reserve) 
{ 
	return ReserveProxyObj(capacity_to_reserve); 
}
