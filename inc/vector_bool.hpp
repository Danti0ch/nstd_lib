#ifndef VECTOR_BOOL_H
#define VECTOR_BOOL_H

#include <vector.hpp>
#include <iostream>

namespace nstd{

template <>
class vector<bool, std::allocator> : public std::allocator<uint8_t>{
public:
    typedef bit_iterator<bit_reference>                iterator;
    typedef bit_iterator<bit_reference_const>          const_iterator;
    typedef bit_iterator_reverse<bit_reference>        reverse_iterator;
    typedef bit_iterator_reverse<bit_reference_const>  const_reverse_iterator;

    typedef bit_reference         reference;
    typedef bit_reference_const   const_reference;
    typedef bit_reference*        pointer;
    typedef bit_reference_const*  const_pointer;

public:

    vector():
        data_(this->allocate(DEF_CAPACITY)),
        size_(0),
        capacity_(DEF_CAPACITY){}
    
    explicit vector(size_t size, const bool& def_val = false):
        data_(this->allocate(convert_size(size))),
        size_(size),
        capacity_(convert_size(size))
    {
        if(def_val)
            memset(data_, 0xFF, capacity_);
        else
            memset(data_, 0, capacity_);
    }

    vector(const vector& other):
        data_(this->allocate(convert_size(other.size_))),
        size_(other.size_),
        capacity_(convert_size(other.size_))
    {
        memcpy(data_, other.data_, convert_size(other.size_));
    }

    vector(vector&& other):
        data_(other.data_),
        size_(other.size_),
        capacity_(other.capacity_)
    {
        other.data_ = NULL;
        other.size_ = other.capacity_ = 0;
    }

    ~vector() {
        this->deallocate(data_, capacity_);
        size_ = 0;
        capacity_ = 0;
    }

    vector& operator=(const vector& other){
        vector tmp = other;
        *this = nstd::move(tmp);
        
        return *this;
    }

    vector& operator=(vector&& other){
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        return *this;
    }

    void assign(size_t n_elems, const bool& val){
        reserve(n_elems);

        // TODO: refactor
        bit_iterator<bit_reference> start = begin();
        set_mem(start, start + n_elems, val);
    }

    // TODO: remove copypaste
    // TODO: static_cast???

    reference at(size_t n_elem){
        if(n_elem >= size_)
            throw std::out_of_range("out of range");

        return *(begin() + n_elem);
    }

    const_reference at(size_t n_elem) const {
        if(n_elem >= size_)
            throw std::out_of_range("out of range");

        return *(cbegin() + n_elem);
    }

    reference       operator[](size_t n_elem)
    { return at(n_elem); }

    const_reference operator[](size_t n_elem) const
    { return at(n_elem); }

    reference front()
    { return at(0); }

    reference back()
    { return at(size_ - 1); }

    const_reference front() const
    { return at(0); }

    const_reference back() const
    { return at(size_ - 1); }

    // TODO:
    pointer data();
    const_pointer data() const;

    bool empty() const
    { return size_ == 0; }

    size_t size() const
    { return size_; }

    // size_t          max_size() const; ? how to implement
    void reserve(size_t capacity){
        if(capacity <= capacity_) return;

        if(capacity_ == 0) {
            data_ = this->allocate(capacity);
        }

        increase_capacity(capacity);
        capacity_ = capacity;
    }

    size_t capacity() const
    { return capacity_; }

    void shrink_to_fit(){
        if(capacity_ == size_) return;

        vector tmp = *this;
        deallocate(data_, capacity_);
        *this = nstd::move(tmp);
        tmp.data_ = NULL;
    }

    void clear()
    { size_ = 0; }

    //? why in standart const_iterator used and what would be realization of erase with it :|
    iterator erase(iterator pos)
    { return erase(pos, pos + 1); }

    iterator erase(iterator start, iterator last){
        auto offset = last - start;
        
        for(iterator it = start; it + offset != end(); it++) {
            *(it) = nstd::move(*(it + offset));
        }

        size_--;

        return start;
    }

    // TODO: remove copypaste

    iterator insert(iterator pos, const bool& val)
    { return insert(pos, 1, val); }

    iterator insert(iterator pos, bool&& val){
        reserve(size_ + 1);
        size_++;

        reverse_iterator last_pos = bit_iterator_reverse<bit_reference>(pos.bit_ref());

        for(reverse_iterator it = rbegin(); it != last_pos; it++) {
            *(it) = nstd::move(*(it + 1));
        }

        *pos = nstd::move(val);

        return pos;
    }

    iterator insert(iterator pos, size_t count, const bool& val){
        if(count == 0) return pos;

        reserve(size_ + count);

        size_ += count;

        reverse_iterator last_pos = bit_iterator_reverse<bit_reference>(pos.bit_ref());

        for(reverse_iterator it = rbegin(); it + count != last_pos; it++) {
            *(it) = nstd::move(*(it + count));
        }

        for(iterator it = pos; it != pos + count; it++){
            *(it) = val;
        }

        return pos;
    }

    // TODO:
    //template<class ItFrom>
    //iterator        insert(iterator pos, ItFrom start, ItFrom last);
    
    // TODO: emplace

    void push_back(const bool& val){
        if(capacity_ == 0) {
            reserve(1);
        }
        if(capacity_ * 8 == size_) {
            increase_capacity(capacity_ + 1);
        }

        *(begin() + size_++) = val;
    }

    void push_back(bool&& val){
        if(capacity_ == 0) {
            reserve(1);
        }
        if(capacity_ * 8 == size_) {
            increase_capacity(capacity_ + 1);
        }

        *(begin() + size_++) = val;
    }

    // TODO: emplace_back

    bool pop_back(){
        if(size_ == 0)
            throw std::out_of_range("pop_back on empty vector");

        bool val = at(size_ - 1);

        if((size_ - 1) * DECREASE_FACTOR >= capacity_ * 8) {
            reduce_capacity();
        }
        return val;
    }

    void resize(size_t n_elems){
        reserve(n_elems);
        size_ = n_elems;
    }

    void resize(size_t n_elems, const bool& val){
        reserve(n_elems);
        size_ = n_elems;

        if(size_ < n_elems) { 
            if(val)
                memset(data_, 0xFF, convert_size(n_elems));
            else
                memset(data_, 0, convert_size(n_elems));
        }
    }

    // TODO: void            swap(vector<T>& other);

    iterator begin()
    { return bit_iterator<bit_reference>(bit_reference(data_, 0));}

    const_iterator cbegin() const
    { return bit_iterator<bit_reference_const>(bit_reference_const(data_, 0));}

    reverse_iterator rbegin() { 
        bit_iterator<bit_reference> tmp = begin() + size_ - 1;
        return bit_iterator_reverse<bit_reference>(tmp.bit_ref());
    }

    const_reverse_iterator crbegin() const{
        bit_iterator<bit_reference_const> tmp = cbegin() + size_ - 1;
        return bit_iterator_reverse<bit_reference_const>(tmp.bit_ref());
    }

    iterator end()
    { return begin() + size_; }

    const_iterator  cend() const
    { return cbegin() + size_; }

    reverse_iterator rend()
    { return rbegin() + size_; }

    const_reverse_iterator crend() const
    { return crbegin() + size_; }

private:
    uint8_t*  data_;
    size_t    size_;
    size_t    capacity_;

private:
    void increase_capacity(size_t low_limit){

        size_t new_capacity = capacity_;
        while(new_capacity < low_limit) {
            new_capacity *= GROWTH_FACTOR;
        }

        // TODO: remove copypaste
        uint8_t* new_data = this->allocate(new_capacity);

        memcpy(new_data, data_, capacity_);

        this->deallocate(data_, capacity_);
        data_ = new_data;

        capacity_ = new_capacity;
    }
    void reduce_capacity(){
        size_t new_capacity = capacity_;
        while(size_ * DECREASE_FACTOR > new_capacity) { // TODO: optimize
            new_capacity /= DECREASE_FACTOR;
        }

        uint8_t* new_data = this->allocate(new_capacity);
        memcpy(new_data, data_, capacity_);
        this->deallocate(data_, capacity_);
        data_ = new_data;

        capacity_ = new_capacity;
    }

    // TODO: memset optimization
    void set_mem(bit_iterator<bit_reference> start, bit_iterator<bit_reference> finish, bool val) {
        for(bit_iterator<bit_reference> it = start; it != finish; it++) {
            *it = val;
        }
    }

    void set_mem(bit_iterator<bit_reference> start, bit_iterator<bit_reference> finish, bit_iterator<bit_reference_const> other_start) {
        for(bit_iterator<bit_reference> it = start; it != finish; it++, other_start++) {
            *it = *other_start;
        }
    }

    size_t convert_size(size_t size)
        { return ((size + 7) / 8); }

};

};

#endif // VECTOR_BOOL_H
