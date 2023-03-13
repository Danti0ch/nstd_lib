#ifndef NSTD_VECTOR_H
#define NSTD_VECTOR_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include "allocation.hpp"
#include "iterator.hpp"
#include "move_semantics.hpp"

//? Base class to bit reference (for reference at())
//? #if _cplusplus > 2020L

const size_t DEF_CAPACITY    = 1 << 4;
const size_t GROWTH_FACTOR   = 1 << 1;
const size_t DECREASE_FACTOR = 1 << 3;

namespace nstd{
    
template<typename T>
class vector{
public:
    typedef ptlike_iterator<T*>                                     iterator;
    typedef ptlike_iterator<const T*>                               const_iterator;
    typedef reverse_ra_iterator_default<ptlike_iterator<T*>>        reverse_iterator;
    typedef reverse_ra_iterator_default<ptlike_iterator<const T*>>  const_reverse_iterator;

    typedef typename ptlike_iterator<T*>::reference             reference;
    typedef typename ptlike_iterator<const T*>::reference       const_reference;

public:

    constexpr vector():
        data_(new uint8_t[DEF_CAPACITY * sizeof(T)]),
        size_(0),
        capacity_(DEF_CAPACITY)
    {}

    constexpr explicit vector(size_t size, const T& def_val = T()):
        data_(new uint8_t[size * sizeof(T)]),
        size_(size),
        capacity_(size)
    {
        set_mem<T>(data_, 0, size, def_val);
    }

    constexpr vector(const vector<T>& other):
        data_(new uint8_t[other.size_ * sizeof(T)]),
        size_(other.size_),
        capacity_(other.size_)
    {   
        set_mem<T>(data_, 0, other.data_, 0, other.size_);
    }

    constexpr vector(vector<T>&& other):
        data_(other.data_),
        size_(other.size_),
        capacity_(other.capacity_)
    {
        other.data_ = NULL;
        other.size_ = other.capacity_ = 0;
    }


    constexpr ~vector() {
        clear_mem<T>(data_, 0, size_);
        delete[] data_;
        size_ = 0;
        capacity_ = 0;
    }

    constexpr vector& operator=(const vector<T>& other){
        vector<T> tmp = other;
        *this = nstd::move(tmp);
        
        return *this;
    }

    constexpr vector&  operator=(vector<T>&& other){
        //? is ok
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        return *this;
    }


    constexpr void assign(size_t n_elems, const T& val) {
        reserve(n_elems);

        set_mem<T>(data_, 0, n_elems, val);
    }

    // TODO: remove copypaste
    // TODO: static_cast???

    constexpr reference at(size_t n_elem){
        if(n_elem >= size_)
            throw std::out_of_range("out of range");

        return data()[n_elem];
    }

    constexpr const_reference at(size_t n_elem) const{
        if(n_elem >= size_)
            throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);

        return data()[n_elem];
    }


    constexpr reference operator[](size_t n_elem)
    { return at(n_elem); }

    constexpr const_reference operator[](size_t n_elem) const
    { return at(n_elem); }

    constexpr reference front()
    { return at(0); }

    constexpr const_reference front() const
    { return at(0); }
    
    constexpr reference back()
    { return at(size_ - 1); }

    constexpr const_reference back() const
    { return at(size_ - 1); }

    constexpr T* data()
    { return reinterpret_cast<T*>(data_); }

    constexpr const T* data() const
    { return static_cast<const T*>(data_); } //? reinterpret_cast?? }

    constexpr bool empty() const
    { return size_ == 0; }

    constexpr size_t size() const
    { return size_; }

    // constexpr size_t          max_size() const; ? how to implement
    constexpr void reserve(size_t capacity){
        if(capacity <= capacity_) return;

        if(capacity_ == 0) {
            //?
            data_ = new uint8_t[capacity * sizeof(T)];
        }

        increase_capacity(capacity);
        capacity_ = capacity;
    }

    constexpr size_t capacity() const
    { return capacity_; }

    constexpr void shrink_to_fit() {
        if(capacity_ == size_) return;

        // TODO: fix ))
        vector<T> tmp = *this;
        clear_mem<T>(data_, 0, size_);
        delete[] data_;
        *this = nstd::move(tmp);
        tmp.data_ = NULL;
    }

    constexpr void clear() {
        clear_mem<T>(data_, 0, size_);
        size_ = 0;
    }

    //? why in standart const_iterator used and what would be realization of erase with it :|
    constexpr iterator erase(iterator pos)
    { return erase(pos, pos + 1); }

    constexpr iterator erase(iterator start, iterator last){
        auto offset = last - start;
        
        for(iterator it = start; it + offset != end(); it++) {
            *(it) = nstd::move(*(it + offset));
        }

        clear_mem<T>(data_, size_ - offset, offset);
        size_--;

        return start;
    }

    // TODO: remove copypaste

    constexpr iterator insert(iterator pos, const T& val)
    { return insert(pos, 1, val); }

    constexpr iterator insert(iterator pos, T&& val) {
        reserve(size_ + 1);

        set_mem<T>(data_, size_, 1);
        size_++;

        reverse_iterator last_pos = make_default_ra_reverse_iterator(pos);

        for(reverse_iterator it = rbegin(); it != pos; it++) {
            *(it) = nstd::move(*(it + 1));
        }

        *pos = nstd::move(val);

        return pos;
    }

    constexpr iterator insert(iterator pos, size_t count, const T& val) {
        if(count == 0) return pos;

        reserve(size_ + count);

        //!
        // TODO: extra default ctrs calls
        set_mem<T>(data_, size_, count);
        size_ += count;

        reverse_iterator last_pos = make_default_ra_reverse_iterator(pos);

        for(reverse_iterator it = rbegin(); it + count != pos; it++) {
            *(it) = nstd::move(*(it + count));
        }

        for(iterator it = pos; it != pos + count; it++){
            *(it) = val;
        }

        return pos;
    }

    // TODO:
    //template<class ItFrom>
    //constexpr iterator        insert(iterator pos, ItFrom start, ItFrom last);
    
    // TODO: emplace

    constexpr void push_back(const T& val) {
        if(capacity_ == 0) {
            reserve(1);
        }
        if(capacity_ == size_) {
            increase_capacity(capacity_ + 1);
        }

        *(T*)(data_ + (size_++) * sizeof(T)) = val;
    }

    constexpr void push_back(T&& val) {
        if(capacity_ == 0) {
            reserve(1);
        }
        if(capacity_ == size_) {
            increase_capacity(capacity_ + 1);
        }

        *(T*)(data_ + (size_++) * sizeof(T)) = nstd::move(val);
    }
    // TODO: emplace_back

    constexpr T pop_back(){
        if(size_ == 0)
            throw std::out_of_range("pop_back on empty vector %p", this);

        T val = data()[size_ - 1];
        data_[--size_].~T();

        if((size_ - 1) * DECREASE_FACTOR >= capacity_ ) {
            reduce_capacity();
        }

        return val;
    }

    constexpr void resize(size_t n_elems) {
        if(size_ > n_elems) {
            clear_mem<T>(data_, n_elems, size_ - n_elems);
        }
        else if(size_ < n_elems) {
            set_mem<T>(data_, size_, n_elems - size_);
        }
    }

    constexpr void resize(size_t n_elems, const T& val) {
        if(size_ > n_elems) {
            clear_mem<T>(data_, n_elems, size_ - n_elems);
        }
        else if(size_ < n_elems) {
            set_mem<T>(data_, size_, n_elems - size_, val);
        }
    }

    // TODO: constexpr void            swap(vector<T>& other);

    constexpr iterator begin()
    { return iterator(reinterpret_cast<T*>(data_)); }

    constexpr const_iterator cbegin() const
    { return const_iterator(reinterpret_cast<T*>(data_)); }

    constexpr reverse_iterator rbegin()
    { return reverse_iterator(reinterpret_cast<T*>(data_) + size_ - 1); }

    constexpr const_reverse_iterator crbegin() const
    { return const_reverse_iterator(reinterpret_cast<T*>(data_)+ size_ - 1); }

    constexpr iterator end()
    { return iterator(reinterpret_cast<T*>(data_)+ size_); }

    constexpr const_iterator  cend() const
    { return const_iterator(reinterpret_cast<T*>(data_) + size_); }

    constexpr reverse_iterator rend()
    { return reverse_iterator(reinterpret_cast<T*>(data_)  - 1); }

    constexpr const_reverse_iterator crend() const
    { return const_reverse_iterator(reinterpret_cast<T*>(data_) - 1); }

private:

    uint8_t*  data_;
    size_t    size_;
    size_t    capacity_;

private:
    // TODO:
    // TODO:
    // TODO:
    // TODO:
    // TODO: optimize
    void increase_capacity(size_t low_limit) {

        size_t new_capacity = capacity_;
        while(new_capacity < low_limit) {
            new_capacity *= GROWTH_FACTOR;
        }

        // TODO: remove copypaste
        uint8_t* new_data = new uint8_t[new_capacity * sizeof(T)];
        set_mem<T>(new_data, 0, data_, 0, size_, true);
        //????? clear_mem(data_, 0, size);
        data_ = new_data;

        capacity_ = new_capacity;
    }

    void reduce_capacity(){
        size_t new_capacity = capacity_;
        while(size_ * DECREASE_FACTOR > new_capacity) { // TODO: optimize
            new_capacity /= DECREASE_FACTOR;
        }

        uint8_t* new_data = new uint8_t[new_capacity * sizeof(T)];
        set_mem<T>(new_data, 0, data_, 0, size_, true);
        //????? clear_mem(data_, 0, size);
        data_ = new_data;

        capacity_ = new_capacity;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const vector<T>& v) {

    for(typename vector<T>::const_iterator iter = v.cbegin(); iter != v.cend(); iter++) {
        stream << *iter << " ";
    }

    return stream;
}

template <>
class vector<bool> {

public:

private:
    
};
};

#endif // NSTD_VECTOR_H
