#ifndef VECTOR_BOOL_H
#define VECTOR_BOOL_H

#include <vector.hpp>

namespace nstd{

template <>
class vector<bool> {
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

    constexpr vector();
    constexpr explicit vector(size_t size, const bool& def_val = false):
        data_()
    constexpr vector(const vector& other);
    constexpr vector(vector&& other);

    constexpr ~vector();

    constexpr vector& operator=(const vector& other);
    constexpr vector& operator=(vector&& other);

    constexpr void assign(size_t n_elems, const bool& val);

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

    constexpr pointer data();
    constexpr const_pointer data() const;

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

    constexpr iterator insert(iterator pos, const bool& val);
    constexpr iterator insert(iterator pos, bool&& val);
    constexpr iterator insert(iterator pos, size_t count, const bool& val) ;

    // TODO:
    //template<class ItFrom>
    //constexpr iterator        insert(iterator pos, ItFrom start, ItFrom last);
    
    // TODO: emplace

    constexpr void push_back(const bool& val);
    constexpr void push_back(bool&& val);
    // TODO: emplace_back

    constexpr bool pop_back();

    constexpr void resize(size_t n_elems);
    constexpr void resize(size_t n_elems, const bool& val);

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

};

#endif // VECTOR_BOOL_H
