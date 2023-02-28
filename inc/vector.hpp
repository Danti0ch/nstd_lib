#ifndef NSTD_VECTOR_H
#define NSTD_VECTOR_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include "allocation.hpp"

//? Base class to bit reference (for reference at())
//? #if _cplusplus > 2020L

const size_t DEF_CAPACITY    = 1 << 4;
const size_t GROWTH_FACTOR   = 1 << 1;
const size_t DECREASE_FACTOR = 1 << 3;

namespace nstd{

struct reference{

};

template<typename T>
class vector{

typedef T& reference;
typedef const T& const_reference;

    constexpr          vector();
    constexpr explicit vector(size_t size, const T& def_val = T());
    constexpr          vector(const vector<T>& other);
    constexpr          vector(vector<T>&& other);

    constexpr         ~vector();

    constexpr vector&  operator=(const vector<T>& other);
    constexpr vector&  operator=(vector<T>&& other);

    constexpr void            assign(size_t n_elems, const T& val);

    constexpr reference       at(size_t n_elem);
    constexpr const_reference at(size_t n_elem) const;

    constexpr reference       operator[](size_t n_elem);
    constexpr const_reference operator[](size_t n_elem) const;

    constexpr reference       front();
    constexpr const_reference front() const;
    
    constexpr reference       back();
    constexpr const_reference back() const;

    constexpr T*              data();
    constexpr const T*        data() const;

    constexpr bool            empty() const;
    constexpr size_t          size() const;
    // constexpr size_t          max_size() const; ? how to implement
    constexpr void            reserve(size_t capacity);
    constexpr size_t          capacity() const;
    constexpr void            shrink_to_fit() const;

    constexpr void            clear();
    // TODO: insert, emplace, erase

    constexpr void            push_back(const T& val);
    constexpr void            push_back(T&& val);
    // TODO: emplace_back

    constexpr T               pop_back();
    constexpr void            resize(size_t n_elems);
    constexpr void            resize(size_t n_elems, const T& val);

    // TODO:
    //constexpr void            swap(vector<T>& other);

private:

    uint8_t*  data_;
    size_t    size_;
    size_t    capacity_;

private:
    void increase_capacity(size_t low_limit);
    void reduce_capacity();
};

//?? TODO: make allocator class
// TODO: implement exception if it is unable to reserve memory(check sem3 vector)

template<typename T>
constexpr vector<T>::vector():
    data_(new uint8_t[DEF_CAPACITY * sizeof(T)]),
    size_(0),
    capacity_(DEF_CAPACITY)
{}

template<typename T>
constexpr vector<T>::vector(size_t size, const T& def_val):
    data_(new uint8_t[size * sizeof(T)]),
    size_(size),
    capacity_(size)
{
    set_mem<T>(data_, 0, size);
}

template<typename T>
constexpr vector<T>::vector(const vector<T>& other):
    data_(new uint8_t[other.size_ * sizeof(T)]),
    size_(other.size_),
    capacity_(other.size_)
{   
    set_mem<T>(data_, 0, static_cast<T*>(other.data_), 0, other.size_);
}

template<typename T>
constexpr vector<T>::vector(vector<T>&& other):
    data_(other.data_),
    size_(other.size_),
    capacity_(other.capacity_)
{
    other.data_ = NULL;
    other.size_ = other.capacity_ = 0;
}

template<typename T>
constexpr vector<T>::~vector(){
    clear_mem<T>(data_, 0, size_);
    delete[] data_;
    size_ = 0;
    capacity_ = 0;
}

template<typename T>
constexpr vector<T>& vector<T>::operator=(const vector<T>& other){
    vector<T> tmp = other;
    *this = std::move(tmp);
    
    return *this;
}

template<typename T>
constexpr vector<T>& vector<T>::operator=(vector<T>&& other){

    //? is ok
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
}

template<typename T>
constexpr void vector<T>::assign(size_t n_elems, const T& val) {
    reserve(n_elems);

    set_mem<T>(data_, 0, n_elems, val);
}

// TODO: remove copypaste
// TODO: static_cast???

template<typename T>
constexpr typename vector<T>::reference vector<T>::at(size_t n_elem) {
    if(n_elem >= size_)
        throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);

    return data()[n_elem];
}

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::at(size_t n_elem) const {
    if(n_elem >= size_)
        throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);

    return data()[n_elem];
}

template<typename T>
constexpr typename vector<T>::reference vector<T>::operator[](size_t n_elem){
    return at(n_elem);
}

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::operator[](size_t n_elem) const{
    return at(n_elem);
}

template<typename T>
constexpr typename vector<T>::reference vector<T>::front(){
    return at(0);
}

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::front() const {
    return at(0);
}

template<typename T>
constexpr  typename vector<T>::reference vector<T>::back() {
    return at(size_ - 1);
}

template<typename T>
constexpr  typename vector<T>::const_reference vector<T>::back() const {
    return at(size_ - 1);
}

// TODO: copypaste
template<typename T>
constexpr T* vector<T>::data() {

    return static_cast<T*>(data_);
}

template<typename T>
constexpr const T* vector<T>::data() const {
    return static_cast<const T*>(data_); //? reinterpret_cast??
}

template<typename T>
constexpr bool vector<T>::empty() const {
    return size_ == 0;
}

template<typename T>
constexpr size_t vector<T>::size() const {
    return size_;
}

template<typename T>
constexpr void vector<T>::reserve(size_t capacity) {

    if(capacity <= capacity_) return;

    if(capacity_ == 0) {
        //?
        data_ = new uint8_t[capacity * sizeof(T)];
    }

    increase_capacity(capacity);
    capacity_ = capacity;
}

template<typename T>
constexpr size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
constexpr void vector<T>::shrink_to_fit() const {
    
    if(capacity_ == size_) return;

    // TODO: fix ))
    vector<T> tmp = *this;
    clear_mem(data_, 0, size_);
    delete[] data_;
    *this = std::move(tmp);
}

template<typename T>
constexpr void vector<T>::clear(){

    clear_mem(data_, 0, size_);
    size_ = 0;
}

// TODO: insert, emplace, erase

template<typename T>
constexpr void vector<T>::push_back(const T& val) {

    if(capacity_ == 0) {
        reserve(1);
    }
    if(capacity_ == size_) {
        increase_capacity(capacity_ + 1);
    }

    *(T*)(data_ + (size_++) * sizeof(T)) = val;
}

template<typename T>
constexpr void vector<T>::push_back(T&& val) {
    if(capacity_ == 0) {
        reserve(1);
    }
    if(capacity_ == size_) {
        increase_capacity(capacity_ + 1);
    }

    *(T*)(data_ + (size_++) * sizeof(T)) = std::move(val)
}
// TODO: emplace_back

template<typename T>
constexpr T vector<T>::pop_back() {
    if(size_ == 0)
        throw std::out_of_range("pop_back on empty vector %p", this);

    T val = data()[size_ - 1];
    data_[--size_].~T();

    if((size_ - 1) * DECREASE_FACTOR >= capacity_ ) {
        reduce_capacity();
    }

    return val;
}

template<typename T>
constexpr void vector<T>::resize(size_t n_elems) {
    if(size_ > n_elems) {
        clear_mem<T>(data_, n_elems, size_ - n_elems);
    }
    else if(size_ < n_elems) {
        set_mem<T>(data_, size_, n_elems - size_);
    }
}

template<typename T>
constexpr void vector<T>::resize(size_t n_elems, const T& val) {
    if(size_ > n_elems) {
        clear_mem<T>(data_, n_elems, size_ - n_elems);
    }
    else if(size_ < n_elems) {
        set_mem<T>(data_, size_, n_elems - size_, val);
    }
}

/*
template<typename T>
constexpr void vector<T>::swap(vector<T>& other) {

}
*/

template<typename T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs){

    if(lhs.size()!= rhs.size()) return false;

    size_t size = lhs.size();
    for(uint i = 0; i < size; i++) {
        if(lhs[i] != rhs[i]) return false;
    }

    return true;
}

// TODO:
// TODO:
// TODO:
// TODO:
// TODO: optimize
template<typename T>
void vector<T>::increase_capacity(size_t low_limit) {

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

template<typename T>
void vector<T>::reduce_capacity(){

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

#endif // NSTD_VECTOR_H
