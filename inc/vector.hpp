#ifndef NSTD_VECTOR_H
#define NSTD_VECTOR_H

#include <cstdlib>
#include <string>
#include <stdexcept>

//? Base class to bit reference (for reference at())
//? #if _cplusplus > 2020L

const size_t DEF_CAPACITY = 16;
const size_t DEF_GROW_FACTOR = 2;

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

    constexpr T*              data(size_t n_elem);
    constexpr const T*        data(size_t n_elem) const;

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

    constexpr void            pop_back();
    constexpr void            resize(size_t n_elems);
    constexpr void            resize(size_t n_elems, const T& val);

    // TODO:
    //constexpr void            swap(vector<T>& other);


private:

    T*     data_;
    size_t size_;
    size_t capacity_;
};

//?? TODO: make allocator class
// TODO: implement exception if it is unable to reserve memory(check sem3 vector)

template<typename T>
constexpr vector<T>::vector():
    data_(new T[DEF_CAPACITY]),
    size_(0),
    capacity_(DEF_CAPACITY)
{}

template<typename T>
constexpr vector<T>::vector(size_t size, const T& def_val):
    data_(new T[size]),
    size_(size),
    capacity_(size)
{
    for(uint i = 0; i < size; i++) {
        data_[i] = def_val;
    }
}

template<typename T>
constexpr vector<T>::vector(const vector<T>& other):
    data_(new T[other.size_]),
    size_(other.size_),
    capacity_(other.size_)
{   
    //? is OK
    memcpy(data_, other.data_, other.size_ * sizeof(T));
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

    for(int i = 0; i < n_elems; i++){
        data_[i] = val;
    }
}

// TODO: remove copypaste
// TODO: static_cast???

template<typename T>
constexpr typename vector<T>::reference vector<T>::at(size_t n_elem) {
    if(n_elem >= size_)
        throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);

    return data_[n_elem];
}

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::at(size_t n_elem) const {
    if(n_elem >= size_)
        throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);

    return data_[n_elem];
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
constexpr T* vector<T>::data(size_t n_elem) {

    if(n_elem >= size_) throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);
    return data_ + n_elem;
}

template<typename T>
constexpr const T* vector<T>::data(size_t n_elem) const {
    if(n_elem >= size_) throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);
    return data_ + n_elem;
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
        data_ = new T[capacity];
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

    data_[size_++] = T(val);
}

template<typename T>
constexpr void vector<T>::push_back(T&& val) {
    if(capacity_ == 0) {
        reserve(1);
    }
    if(capacity_ == size_) {
        increase_capacity(capacity_ + 1);
    }

    data_[size_++] = T(std::move(val));
}
// TODO: emplace_back

template<typename T>
constexpr void vector<T>::pop_back() {
    if(size_ == 0)
        throw std::out_of_range("pop_back on empty vector %p", this);

    data_[--size_].~T();
}

template<typename T>
constexpr void vector<T>::resize(size_t n_elems) {
    if(size_ > n_elems) {
        clear_mem(n_elems, n_elems, size_ - n_elems);
    }
    else if(size_ < n_elems) {
        set_mem(data_, size_, n_elems - size_);
    }
}

template<typename T>
constexpr void vector<T>::resize(size_t n_elems, const T& val) {
    if(size_ > n_elems) {
        clear_mem(n_elems, n_elems, size_ - n_elems);
    }
    else if(size_ < n_elems) {
        set_mem(data_, size_, n_elems - size_, val);
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

};

#endif // NSTD_VECTOR_H
