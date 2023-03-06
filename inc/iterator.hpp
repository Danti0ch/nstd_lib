#ifndef NSTD_ITERATOR_H
#define NSTD_ITERATOR_H

#include "move_semantics.hpp"
#include <iostream>

namespace nstd{

//? Could it work on non pointer classes
template<typename Pt>
class ptlike_iterator : public std::iterator_traits<Pt>
{
public:
    typedef typename std::iterator_traits<Pt>::iterator_category iterator_category;
    typedef typename std::iterator_traits<Pt>::value_type value_type;
    typedef typename std::iterator_traits<Pt>::difference_type difference_type;
    typedef typename std::iterator_traits<Pt>::pointer pointer;
    typedef typename std::iterator_traits<Pt>::reference reference;
public:
    ptlike_iterator():
        p_data_(NULL){}

    ptlike_iterator(Pt pt):
        p_data_(pt){}

    ptlike_iterator(const ptlike_iterator& other) = default;
 
    ptlike_iterator& operator =(const ptlike_iterator& other) = default;

    ~ptlike_iterator()
    { p_data_ = NULL; }

    reference operator *()
    { return *p_data_; }

    pointer operator ->() 
    { return p_data_; }

    reference operator [](difference_type idx) 
    { return *(p_data_ + idx); }

    ptlike_iterator& operator ++() {
        p_data_++;
        return *this;
    }

    ptlike_iterator operator ++(int)
    { return ptlike_iterator(p_data_++); }

    ptlike_iterator& operator --() {
        p_data_--;
        return *this;
    }

    ptlike_iterator operator --(int)
    { return ptlike_iterator(p_data_--); }

    ptlike_iterator  operator +(difference_type offset) const
    { return ptlike_iterator(p_data_ + offset); }

    ptlike_iterator& operator +=(difference_type offset){
        p_data_ += offset;
        return *this;
    }

    ptlike_iterator operator -(difference_type offset) const
    { return ptlike_iterator(p_data_ - offset); }

    ptlike_iterator& operator -=(difference_type offset) {
        p_data_ -= offset;
        return *this;
    }

    difference_type operator -(const ptlike_iterator& other) const
    { return p_data_ - other.p_data_; }

    bool operator ==(const ptlike_iterator& other) const
    { return p_data_ == other.p_data_; }

    bool operator !=(const ptlike_iterator& other) const
    { return p_data_ != other.p_data_; }

    bool operator >(const ptlike_iterator& other) const
    { return p_data_ > other.p_data_; }

    bool operator >=(const ptlike_iterator& other) const
    { return p_data_ >= other.p_data_; }

    bool operator <(const ptlike_iterator& other) const
    { return p_data_ < other.p_data_; }

    bool operator <=(const ptlike_iterator& other) const
    { return p_data_ <= other.p_data_; }

private:
   Pt p_data_;
};


template<typename TIter>
class reverse_ra_iterator_default;

template<class TIter>
reverse_ra_iterator_default<TIter>  make_default_ra_reverse_iterator(const TIter& iter){
    return reverse_ra_iterator_default<TIter>(iter);
}

template<class TIter>
reverse_ra_iterator_default<TIter>  make_default_ra_reverse_iterator(TIter&& iter){
    return reverse_ra_iterator_default<TIter>(iter);
}

template<typename TIter>
class reverse_ra_iterator_default : public std::iterator_traits<TIter>
{
    typedef typename std::iterator_traits<TIter>::iterator_category iterator_category;
    typedef typename std::iterator_traits<TIter>::value_type value_type;
    typedef typename std::iterator_traits<TIter>::difference_type difference_type;
    typedef typename std::iterator_traits<TIter>::pointer pointer;
    typedef typename std::iterator_traits<TIter>::reference reference;
public:
    reverse_ra_iterator_default() = default;

    //?
    reverse_ra_iterator_default(pointer pt):
        direct_iter_(pt){}

    reverse_ra_iterator_default(const reverse_ra_iterator_default& other) = default;
    reverse_ra_iterator_default(reverse_ra_iterator_default&& other) = default;
//?    { other.direct_iter_ = NULL; }

    reverse_ra_iterator_default(const TIter& other):
        direct_iter_(other){}

    reverse_ra_iterator_default(TIter&& other):
        direct_iter_(nstd::move(other)){}

    reverse_ra_iterator_default& operator =(const reverse_ra_iterator_default& other) = default;
    //? operator=(const TIter ..)

    ~reverse_ra_iterator_default() = default;

    reference operator *()
    { return direct_iter_.operator*(); }

    pointer operator ->() 
    { return direct_iter_.operator->(); }

    reference operator [](difference_type idx) 
    { return direct_iter_.operator[](idx); }

    reverse_ra_iterator_default& operator ++(){ 
        direct_iter_.operator--(); //! extra return *this in fw_iter.operator--()
        return *this;
    }

    //? is performance ok
    reverse_ra_iterator_default operator ++(int)
    { return reverse_ra_iterator_default(nstd::move(direct_iter_.operator--(0))); }

    reverse_ra_iterator_default& operator --(){
        direct_iter_.operator++();
        return *this;
    }

    reverse_ra_iterator_default operator --(int)
    { return reverse_ra_iterator_default(nstd::move(direct_iter_.operator++(0))); }

    reverse_ra_iterator_default operator +(difference_type offset) const
    { return reverse_ra_iterator_default(nstd::move(direct_iter_.operator-(offset))); }

    reverse_ra_iterator_default& operator +=(difference_type offset){ 
        direct_iter_.operator-=(offset); 
        return *this;
    }

    reverse_ra_iterator_default operator -(difference_type offset) const
    { return reverse_ra_iterator_default(nstd::move(direct_iter_.operator+(offset))); }

    reverse_ra_iterator_default& operator -=(difference_type offset){
        direct_iter_.operator+=(offset); 
        return *this;
    }

    //? for ex std::normal_iterator doesnt have this operator
    difference_type operator -(const reverse_ra_iterator_default& other) const
    { return other.direct_iter_.operator-(direct_iter_); }

    bool operator ==(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator==(other); }

    bool operator !=(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator!=(other); }

    bool operator >(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator<(other); }

    bool operator >=(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator<=(other); }

    bool operator <(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator>(other); }

    bool operator <=(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator>=(other); }

private:
    TIter direct_iter_;
};

class bit_reference{

public:
    bit_reference():
        p_data_(NULL),
        mask_(0){}
    
    bit_reference(uint8_t* p_data, uint8_t mask):
        p_data_(p_data),
        mask_(mask){}
    
    bit_reference(const bit_reference& other) = default;
    
    ~bit_reference() {
        p_data_ = NULL; mask_ = 0;
    }

    bit_reference& operator=(const bit_reference& other){
        p_data_ = other.p_data_;
        mask_   = other.mask_;

        return *this;
    }

    bit_reference& operator=(bool val) {
        *p_data_ &= ~mask_;
        *p_data_ |= val ? mask_ : ~mask_;

        return *this;
    }

    operator bool(){
        return *p_data_ & mask_;
    }
private:
    uint8_t* p_data_;
    uint8_t  mask_;
};

class bit_pointer {

public:
    bit_pointer() = default;
    bit_pointer(uint8_t* p_data, uint8_t mask):
        ref_(p_data, mask){}
    

private:
    bit_reference ref_;
};

}; // namespace nstd

#endif // NSTD_ITERATOR_H
