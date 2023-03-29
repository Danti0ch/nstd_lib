#ifndef NSTD_VECTOR_H
#define NSTD_VECTOR_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include "construction.hpp"
#include "iterator.hpp"
#include "move_semantics.hpp"

//? Base class to bit reference (for reference at())
//? #if _cplusplus > 2020L

const size_t DEF_CAPACITY    = 1 << 4;
const size_t GROWTH_FACTOR   = 1 << 1;
const size_t DECREASE_FACTOR = 1 << 3;

namespace nstd{
    
template<typename T, template <typename> class Alloc = std::allocator>
class vector : public Alloc<T>{
public:
    typedef ptlike_iterator<T*>                                     iterator;
    typedef ptlike_iterator<const T*>                               const_iterator;
    typedef reverse_ra_iterator_default<ptlike_iterator<T*>>        reverse_iterator;
    typedef reverse_ra_iterator_default<ptlike_iterator<const T*>>  const_reverse_iterator;

    typedef typename ptlike_iterator<T*>::reference             reference;
    typedef typename ptlike_iterator<const T*>::reference       const_reference;

public:

    constexpr vector();
    constexpr explicit vector(size_t size, const T& def_val = T());
    constexpr vector(const vector<T>& other);
    constexpr vector(vector<T>&& other);

    constexpr ~vector();

    constexpr vector& operator=(const vector<T>& other);
    constexpr vector& operator=(vector<T>&& other);

    constexpr void assign(size_t n_elems, const T& val);

    // TODO: remove copypaste
    // TODO: static_cast???

    constexpr reference       at(size_t n_elem);
    constexpr const_reference at(size_t n_elem) const;

    constexpr reference       operator[](size_t n_elem);
    constexpr const_reference operator[](size_t n_elem) const;

    constexpr reference front();
    constexpr reference back();

    constexpr const_reference front() const;    
    constexpr const_reference back() const;

    constexpr T* data();
    constexpr const T* data() const;

    constexpr bool empty() const;
    constexpr size_t size() const;

    // constexpr size_t          max_size() const; ? how to implement
    constexpr void reserve(size_t capacity);

    constexpr size_t capacity() const;

    constexpr void shrink_to_fit();

    constexpr void clear();

    //? why in standart const_iterator used and what would be realization of erase with it :|
    constexpr iterator erase(iterator pos);
    constexpr iterator erase(iterator start, iterator last);

    // TODO: remove copypaste

    constexpr iterator insert(iterator pos, const T& val);
    constexpr iterator insert(iterator pos, T&& val);
    constexpr iterator insert(iterator pos, size_t count, const T& val) ;

    // TODO:
    //template<class ItFrom>
    //constexpr iterator        insert(iterator pos, ItFrom start, ItFrom last);
    
    // TODO: emplace

    constexpr void push_back(const T& val);
    constexpr void push_back(T&& val);
    // TODO: emplace_back

    constexpr T pop_back();

    constexpr void resize(size_t n_elems);
    constexpr void resize(size_t n_elems, const T& val);

    // TODO: constexpr void            swap(vector<T>& other);

    constexpr iterator begin();
    constexpr const_iterator cbegin() const;
    constexpr reverse_iterator rbegin();
    constexpr const_reverse_iterator crbegin() const;

    constexpr iterator end();
    constexpr const_iterator  cend() const;
    constexpr reverse_iterator rend();
    constexpr const_reverse_iterator crend() const;

private:
    uint8_t*  data_;
    size_t    size_;
    size_t    capacity_;

private:
    void increase_capacity(size_t low_limit);
    void reduce_capacity();
};

template<typename T>
constexpr vector<T>::vector():
    data_(this->allocate(DEF_CAPACITY)),
    size_(0),
    capacity_(DEF_CAPACITY)
{}

template<typename T>
constexpr vector<T>::vector(size_t size, const T& def_val = T()):
    data_(this->allocate(size)),
    size_(size),
    capacity_(size)
{
    set_mem<T>(data_, 0, size, def_val);
}

template<typename T>
constexpr vector<T>::vector(const vector<T>& other):
    data_(this->allocate(size)),
    size_(other.size_),
    capacity_(other.size_)
{   
    set_mem<T>(data_, 0, other.data_, 0, other.size_);
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
constexpr vector<T>::~vector() {
    clear_mem<T>(data_, 0, size_);
    this->deallocate(data_, capacity_);
    size_ = 0;
    capacity_ = 0;
}

template<typename T>
constexpr vector<T>& vector<T>::operator=(const vector<T>& other){
    vector<T> tmp = other;
    *this = nstd::move(tmp);
    
    return *this;
}

template<typename T>
constexpr vector<T>&  vector<T>::operator=(vector<T>&& other){
    //? is ok
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}

template<typename T>
constexpr void vector<T>::assign(size_t n_elems, const T& val) {
    reserve(n_elems);

    set_mem<T>(data_, 0, n_elems, val);
}

template<typename T>
constexpr typename vector<T>::reference vector<T>::at(size_t n_elem){
    if(n_elem >= size_)
        throw std::out_of_range("out of range");

    return data()[n_elem];
}

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::at(size_t n_elem) const {
    if(n_elem >= size_)
        throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);

    return data()[n_elem];
}

template<typename T>
constexpr typename vector<T>::reference vector<T>::operator[](size_t n_elem)
{ return at(n_elem); }

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::operator[](size_t n_elem) const
{ return at(n_elem); }

template<typename T>
constexpr typename vector<T>::reference vector<T>::front()
{ return at(0); }

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::front() const
{ return at(0); }

template<typename T>
constexpr typename vector<T>::reference vector<T>::back()
{ return at(size_ - 1); }

template<typename T>
constexpr typename vector<T>::const_reference vector<T>::back() const
{ return at(size_ - 1); }

template<typename T>
constexpr T* vector<T>::data()
{ return reinterpret_cast<T*>(data_); }

template<typename T>
constexpr const T* vector<T>::data() const
{ return static_cast<const T*>(data_); } //? reinterpret_cast?? }

template<typename T>
constexpr bool vector<T>::empty() const
{ return size_ == 0; }

template<typename T>
constexpr size_t vector<T>::size() const
{ return size_; }

// constexpr size_t          max_size() const; ? how to implement
template<typename T>
constexpr void vector<T>::reserve(size_t capacity) {
    if(capacity <= capacity_) return;

    if(capacity_ == 0) {
        //?
        data_ = this->allocate(capacity);
    }

    increase_capacity(capacity);
    capacity_ = capacity;
}

template<typename T>
constexpr size_t vector<T>::capacity() const
{ return capacity_; }

template<typename T>
constexpr void vector<T>::shrink_to_fit() {
    if(capacity_ == size_) return;

    // TODO: fix ))
    vector<T> tmp = *this;
    clear_mem<T>(data_, 0, size_);
    deallocate(data_, capacity_);
    *this = nstd::move(tmp);
    tmp.data_ = NULL;
}

template<typename T>
constexpr void vector<T>::clear() {
    clear_mem<T>(data_, 0, size_);
    size_ = 0;
}

//? why in standart const_iterator used and what would be realization of erase with it :|
template<typename T>
constexpr typename vector<T>::iterator vector<T>::erase(typename vector<T>::iterator pos)
{ return erase(pos, pos + 1); }

template<typename T>
constexpr typename vector<T>::iterator erase(typename vector<T>::iterator start, typename vector<T>::iterator last){
    auto offset = last - start;
    
    for(iterator it = start; it + offset != end(); it++) {
        *(it) = nstd::move(*(it + offset));
    }

    clear_mem<T>(data_, size_ - offset, offset);
    size_--;

    return start;
}

// TODO: remove copypaste

template<typename T>
constexpr typename vector<T>::iterator vector<T>::insert(typename vector<T>::iterator pos, const T& val)
{ return insert(pos, 1, val); }

template<typename T>
constexpr typename vector<T>::iterator vector<T>::insert(typename vector<T>::iterator pos, T&& val) {
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

template<typename T>
constexpr typename vector<T>::iterator vector<T>::insert(typename vector<T>::iterator pos, size_t count, const T& val) {
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

template<typename T>
std::ostream& operator<<(std::ostream& stream, const vector<T>& v) {

    for(typename vector<T>::const_iterator iter = v.cbegin(); iter != v.cend(); iter++) {
        stream << *iter << " ";
    }

    return stream;
}

template<typename T>
void vector<T>::increase_capacity(size_t low_limit) {

    size_t new_capacity = capacity_;
    while(new_capacity < low_limit) {
        new_capacity *= GROWTH_FACTOR;
    }

    // TODO: remove copypaste
    T* new_data = this->allocate(new_capacity);
    set_mem<T>(new_data, 0, data_, 0, size_, true);
    //????? clear_mem(data_, 0, size);

    this->deallocate(data_, capacity_);
    data_ = new_data;

    capacity_ = new_capacity;
}

template<typename T>
void vector<T>::reduce_capacity(){
    size_t new_capacity = capacity_;
    while(size_ * DECREASE_FACTOR > new_capacity) { // TODO: optimize
        new_capacity /= DECREASE_FACTOR;
    }

    uint8_t* new_data = this->allocate(new_capacity);
    set_mem<T>(new_data, 0, data_, 0, size_, true);
    //????? clear_mem(data_, 0, size);

    this->deallocate(data_, capacity_);
    data_ = new_data;

    capacity_ = new_capacity;
}
};

#endif // NSTD_VECTOR_H
