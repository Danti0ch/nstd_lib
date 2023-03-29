#ifndef NSTD_VECTOR_H
#define NSTD_VECTOR_H

#include <cstdlib>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include "construction.hpp"
#include "iterator.hpp"
#include "move_semantics.hpp"
#include "allocator.hpp"
#include <concepts>

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

    typedef typename ptlike_iterator<T*>::reference                 reference;
    typedef typename ptlike_iterator<const T*>::reference           const_reference;

public:

    constexpr vector();
    constexpr explicit vector(size_t size, const T& def_val = T());
    constexpr vector(const vector& other);
    constexpr vector(vector&& other);

    constexpr ~vector();

    constexpr vector& operator=(const vector& other);
    constexpr vector& operator=(vector&& other);

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
    T*        data_;
    size_t    size_;
    size_t    capacity_;

private:
    void increase_capacity(size_t low_limit);
    void reduce_capacity();
};

template<typename T, template <typename> class Alloc>
constexpr vector<T, Alloc>::vector():
    data_(this->allocate(DEF_CAPACITY)),
    size_(0),
    capacity_(DEF_CAPACITY)
{}

template<typename T, template <typename> class Alloc>
constexpr vector<T, Alloc>::vector(size_t size, const T& def_val):
    data_(this->allocate(size)),
    size_(size),
    capacity_(size)
{
    set_mem<T>(data_, 0, size, def_val);
}

template<typename T, template <typename> class Alloc>
constexpr vector<T, Alloc>::vector(const vector<T, Alloc>& other):
    data_(this->allocate(other.size_)),
    size_(other.size_),
    capacity_(other.size_)
{   
    set_mem<T>(data_, 0, other.data_, 0, other.size_);
}

// TODO: allocator copy issues
template<typename T, template <typename> class Alloc>
constexpr vector<T, Alloc>::vector(vector<T, Alloc>&& other):
    data_(other.data_),
    size_(other.size_),
    capacity_(other.capacity_)
{
    other.data_ = NULL;
    other.size_ = other.capacity_ = 0;
}

template<typename T, template <typename> class Alloc>
constexpr vector<T, Alloc>::~vector() {
    clear_mem<T>(data_, 0, size_);
    this->deallocate(data_, capacity_);
    size_ = 0;
    capacity_ = 0;
}

template<typename T, template <typename> class Alloc>
constexpr vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& other){
    vector<T, Alloc> tmp = other;
    *this = nstd::move(tmp);
    
    return *this;
}

template<typename T, template <typename> class Alloc>
constexpr vector<T, Alloc>&  vector<T, Alloc>::operator=(vector<T, Alloc>&& other){
    //? is ok
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}

template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::assign(size_t n_elems, const T& val) {
    reserve(n_elems);

    set_mem<T>(data_, 0, n_elems, val);
}

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::reference vector<T, Alloc>::at(size_t n_elem){
    if(n_elem >= size_)
        throw std::out_of_range("out of range");

    return data()[n_elem];
}

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_reference vector<T, Alloc>::at(size_t n_elem) const {
    if(n_elem >= size_)
        throw std::out_of_range("index %lu for vector %p out of range", n_elem, this);

    return data()[n_elem];
}

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::reference vector<T, Alloc>::operator[](size_t n_elem)
{ return at(n_elem); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_reference vector<T, Alloc>::operator[](size_t n_elem) const
{ return at(n_elem); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::reference vector<T, Alloc>::front()
{ return at(0); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_reference vector<T, Alloc>::front() const
{ return at(0); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::reference vector<T, Alloc>::back()
{ return at(size_ - 1); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_reference vector<T, Alloc>::back() const
{ return at(size_ - 1); }

template<typename T, template <typename> class Alloc>
constexpr T* vector<T, Alloc>::data()
{ return (data_); }

template<typename T, template <typename> class Alloc>
constexpr const T* vector<T, Alloc>::data() const
{ return (data_); } //? reinterpret_cast?? }

template<typename T, template <typename> class Alloc>
constexpr bool vector<T, Alloc>::empty() const
{ return size_ == 0; }

template<typename T, template <typename> class Alloc>
constexpr size_t vector<T, Alloc>::size() const
{ return size_; }

// constexpr size_t          max_size() const; ? how to implement
template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::reserve(size_t capacity) {
    if(capacity <= capacity_) return;

    if(capacity_ == 0) {
        //?
        data_ = this->allocate(capacity);
    }

    increase_capacity(capacity);
    capacity_ = capacity;
}

template<typename T, template <typename> class Alloc>
constexpr size_t vector<T, Alloc>::capacity() const
{ return capacity_; }

template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::shrink_to_fit() {
    if(capacity_ == size_) return;

    // TODO: fix ))
    // not cringe
    // modify by adding allocator shrink to fit method for searching free block
    vector<T, Alloc> tmp = *this;
    clear_mem<T>(data_, 0, size_);
    deallocate(data_, capacity_);
    *this = nstd::move(tmp);
    tmp.data_ = NULL;
}

template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::clear() {
    clear_mem<T>(data_, 0, size_);
    size_ = 0;
}

template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::push_back(const T& val) {
    if(capacity_ == 0) {
        reserve(1);
    }
    if(capacity_ == size_) {
        increase_capacity(capacity_ + 1);
    }

    *(data_ + (size_++)) = val;
}

template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::push_back(T&& val) {
    if(capacity_ == 0) {
        reserve(1);
    }
    if(capacity_ == size_) {
        increase_capacity(capacity_ + 1);
    }

    *(data_ + (size_++)) = nstd::move(val);
}
// TODO: emplace_back

template<typename T, template <typename> class Alloc>
constexpr T vector<T, Alloc>::pop_back(){
    if(size_ == 0)
        throw std::out_of_range("pop_back on empty vector %p", this);

    T val = data()[size_ - 1];
    data_[--size_].~T();

    if((size_ - 1) * DECREASE_FACTOR >= capacity_ ) {
        reduce_capacity();
    }

    return val;
}

template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::resize(size_t n_elems) {
    reserve(n_elems);

    if(size_ > n_elems) {
        clear_mem<T>(data_, n_elems, size_ - n_elems);
    }
    else if(size_ < n_elems) {
        set_mem<T>(data_, size_, n_elems - size_);
    }
    size_ = n_elems;
}

template<typename T, template <typename> class Alloc>
constexpr void vector<T, Alloc>::resize(size_t n_elems, const T& val) {
    reserve(n_elems);

    if(size_ > n_elems) {
        clear_mem<T>(data_, n_elems, size_ - n_elems);
    }
    else if(size_ < n_elems) {
        set_mem<T>(data_, size_, n_elems - size_, val);
    }

    size_ = n_elems;
}

//? why in standart const_iterator used and what would be realization of erase with it :|
template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(typename vector<T, Alloc>::iterator pos)
{ return erase(pos, pos + 1); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(typename vector<T, Alloc>::iterator start, typename vector<T, Alloc>::iterator last){
    auto offset = last - start;
    
    for(typename vector<T, Alloc>::iterator it = start; it + offset != end(); it++) {
        *(it) = nstd::move(*(it + offset));
    }

    clear_mem<T>(data_, size_ - offset, offset);
    size_--;

    return start;
}

// TODO: remove copypaste

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(typename vector<T, Alloc>::iterator pos, const T& val)
{ return insert(pos, 1, val); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(typename vector<T, Alloc>::iterator pos, T&& val) {
    reserve(size_ + 1);

    set_mem<T>(data_, size_, 1);
    size_++;

    reverse_iterator last_pos = make_default_ra_reverse_iterator(pos);

    for(reverse_iterator it = rbegin(); it != last_pos; it++) {
        *(it) = nstd::move(*(it + 1));
    }

    *pos = nstd::move(val);

    return pos;
}

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(typename vector<T, Alloc>::iterator pos, size_t count, const T& val) {
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

template<typename T, template <typename> class Alloc>
std::ostream& operator<<(std::ostream& stream, const vector<T, Alloc>& v) {

    for(typename vector<T, Alloc>::const_iterator iter = v.cbegin(); iter != v.cend(); iter++) {
        stream << *iter << " ";
    }

    return stream;
}

template<typename T, template <typename> class Alloc>
void vector<T, Alloc>::increase_capacity(size_t low_limit) {

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

template<typename T, template <typename> class Alloc>
void vector<T, Alloc>::reduce_capacity(){
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

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::iterator vector<T, Alloc>::begin()
{ return iterator((data_)); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_iterator vector<T, Alloc>::cbegin() const
{ return const_iterator((data_)); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rbegin()
{ return reverse_iterator((data_) + size_ - 1); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::crbegin() const
{ return const_reverse_iterator((data_)+ size_ - 1); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::iterator vector<T, Alloc>::end()
{ return iterator((data_)+ size_); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_iterator  vector<T, Alloc>::cend() const
{ return const_iterator((data_) + size_); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rend()
{ return reverse_iterator((data_)  - 1); }

template<typename T, template <typename> class Alloc>
constexpr typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::crend() const
{ return const_reverse_iterator((data_) - 1); }



// __________________________________________________________________________________________________________________________________ //

//                                     Vectors methods specialization for allocators

// __________________________________________________________________________________________________________________________________ //

/*
template<typename T>
concept IsStackAllocator = requires
{
  typename T::StackAllocator;
};

template<typename T>
void vector<T, Alloc>::increase_capacity(size_t low_limit) requires({

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

template<typename T, template <typename> class Alloc>
void vector<T, Alloc>::reduce_capacity(){
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
*/

};

#endif // NSTD_VECTOR_H
