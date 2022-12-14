#pragma once

#include <memory>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace cpplab
{
    template <class T>
    class vector
    {
    public:
        typedef T value_type;
        vector()
        {
            capacity_ = 0;
            size_ = 0;
        }
        vector(std::initializer_list<T> list)
        {
            capacity_ = list.size();
            size_ = list.size();
            data_ = std::make_unique<T[]>(capacity_);
            std::copy(list.begin(), list.end(), data_.get());
        }
        ~vector()
        {
            data_.release();
        }
        T &operator[](size_t index)
        {
            if (index < 0 || index >= size_)
                throw std::out_of_range("vector index out of bounds");
            return data_[index];
        }
        T operator[](size_t index) const
        {
            if (index < 0 || index >= size_)
                throw std::out_of_range("vector index out of bounds");
            return data_[index];
        }
        friend std::ostream &operator<<(std::ostream &os, const vector<T> &v)
        {
            for (size_t i = 0; i < v.size_; ++i)
                os << v[i] << " ";
            return os;
        }

        void push_back(T item)
        {
            if (size_ >= capacity_)
                reallocate();
            data_[size_] = item;
            ++size_;
        }
        void pop_back()
        {
            if (size_ > 0)
                --size_;
        }
        void resize(size_t new_size, T value)
        {
            if (new_size > size_ && new_size <= capacity_)
                for (size_t i = size_; i < new_size; ++i)
                    data_[i] = value;
            else if (new_size > capacity_)
                for (size_t i = size_; i < new_size; ++i)
                    this->push_back(value);
            size_ = new_size;
        }
        size_t size() const
        {
            return size_;
        }
        T *begin() const
        {
            return &data_[0];
        }
        T *end() const
        {
            return &data_[size_];
        }

    private:
        void reallocate()
        {
            size_t new_capacity = (capacity_ == 0) ? 1 : 2 * capacity_;
            capacity_ = new_capacity;
            T *new_data = new T[new_capacity];
            for (size_t i = 0; i < size_; ++i)
                new_data[i] = data_[i];
            data_.reset(new_data);
        }

        size_t size_;
        size_t capacity_;
        std::unique_ptr<T[]> data_;
    };
}