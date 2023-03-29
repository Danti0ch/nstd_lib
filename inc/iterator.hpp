#ifndef NSTD_ITERATOR_H
#define NSTD_ITERATOR_H

#include "move_semantics.hpp"
#include <iostream>

// TODO: operator++ refactor

namespace nstd{

// TODO: add checks

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
    { return direct_iter_.operator==(other.direct_iter_); }

    bool operator !=(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator!=(other.direct_iter_); }

    bool operator >(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator<(other.direct_iter_); }

    bool operator >=(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator<=(other.direct_iter_); }

    bool operator <(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator>(other.direct_iter_); }

    bool operator <=(const reverse_ra_iterator_default& other) const
    { return direct_iter_.operator>=(other.direct_iter_); }

private:

    //! is ok
    TIter direct_iter_;
};

struct bit_reference{

    bit_reference():
        p_data_(NULL),
        mask_offset_(0){}
    
    bit_reference(uint8_t* p_data, uint8_t mask_offset):
        p_data_(p_data),
        mask_offset_(mask_offset){}
    
    bit_reference(const bit_reference& other) = default;
    
    ~bit_reference() {
        p_data_ = NULL; mask_offset_ = 0;
    }

    bit_reference& operator=(const bit_reference& other){
        p_data_        = other.p_data_;
        mask_offset_   = other.mask_offset_;

        return *this;
    }

    bit_reference& operator=(bool val) {
        uint8_t mask = 1 << mask_offset_;

        *p_data_ &= ~mask;
        *p_data_ |= val ? mask : ~mask;

        return *this;
    }

    operator bool(){
        return *p_data_ & (1 << mask_offset_);
    }

    uint8_t* p_data_;
    uint8_t  mask_offset_;
};

struct bit_reference_const{

    bit_reference_const():
        p_data_(NULL),
        mask_offset_(0){}
    
    bit_reference_const(const uint8_t* p_data, uint8_t mask_offset):
        p_data_(p_data),
        mask_offset_(mask_offset){}
    
    bit_reference_const(const bit_reference_const& other) = default;
    
    ~bit_reference_const() {
        p_data_ = NULL; mask_offset_ = 0;
    }

    bit_reference_const& operator=(const bit_reference_const& other){
        p_data_        = other.p_data_;
        mask_offset_   = other.mask_offset_;

        return *this;
    }

    bit_reference_const& operator=(bool val) = delete;

    operator bool(){
        return *p_data_ & (1 << mask_offset_);
    }

    const uint8_t* p_data_;
    uint8_t  mask_offset_;
};

template<class BIT_REF> class bit_iterator_reverse;

// TODO: remove, express by pt_like_iterator
template<class BIT_REF>
class bit_iterator
{
    //! codestyle check
    static_assert(std::is_same_v<BIT_REF, bit_reference> || std::is_same_v<BIT_REF, bit_reference_const>, "Type of the BIT_REF could'nt be custom");
    

    template<class BIT_REFF> friend class bit_iterator_reverse;
public:
    typedef typename std::random_access_iterator_tag iterator_category;
    typedef bool value_type;
    typedef ptrdiff_t difference_type;
    typedef BIT_REF* pointer;
    typedef BIT_REF reference;

private:
    BIT_REF get_shifted_bitref(difference_type offset) const {
        uint8_t new_mask_offset     = (bit_ref_.mask_offset_ + offset) % 8;
        difference_type addr_offset = (bit_ref_.mask_offset_ + offset) / 8;

        return BIT_REF(bit_ref_.p_data_ + addr_offset, new_mask_offset);
    } 

public:
    bit_iterator():
        bit_ref_(){}

    bit_iterator(const BIT_REF& pt):
        bit_ref_(pt){}

    bit_iterator(const bit_iterator& other) = default;
 
    bit_iterator& operator =(const bit_iterator& other) = default;

    ~bit_iterator() = default;

    reference operator *()
    { return bit_ref_; }

    // TODO: check
    pointer operator ->() 
    { return &bit_ref_; }

    reference operator [](difference_type idx) { 
        return get_shifted_bitref(idx);
    }

    bit_iterator& operator ++() {
        if(bit_ref_.mask_offset_ == 7) {
            bit_ref_.mask_offset_  = 0;
            bit_ref_.p_data_      += 1;
        } else {
            bit_ref_.mask_offset_ += 1;
        }
        return *this;
    }

    bit_iterator operator ++(int)
    { return bit_iterator(this->operator++()); }

    bit_iterator& operator --() {
        if(bit_ref_.mask_offset_ == 0) {
            bit_ref_.mask_offset_  = 7;
            bit_ref_.p_data_      -= 1;
        } else {
            bit_ref_.mask_offset_ -= 1;
        }
        return *this;
    }

    bit_iterator operator --(int)
    { return bit_iterator(this->operator--()); }

    bit_iterator  operator +(difference_type offset) const
    { return bit_iterator(get_shifted_bitref(offset)); }

    bit_iterator& operator +=(difference_type offset){
        bit_ref_ = get_shifted_bitref(offset);
        return *this;
    }

    bit_iterator operator -(difference_type offset) const
    { return bit_iterator(get_shifted_bitref(-offset)); }

    bit_iterator& operator -=(difference_type offset) {
        bit_ref_ = get_shifted_bitref(-offset);
        return *this;
    }

    difference_type operator -(const bit_iterator& other) const{
        if(bit_ref_.p_data_ == other.bit_ref_.p_data_)
            { return static_cast<difference_type>(bit_ref_.mask_offset_ - other.bit_ref_.mask_offset_); }

        return static_cast<difference_type>(bit_ref_.p_data_ - other.bit_ref_.p_data_) * 8 + bit_ref_.mask_offset_ + 7 - other.bit_ref_.mask_offset_;
    }

    bool operator ==(const bit_iterator& other) const
    { return bit_ref_.p_data_ == other.bit_ref_.p_data_ && bit_ref_.mask_offset_ == other.bit_ref_.mask_offset_; }

    bool operator !=(const bit_iterator& other) const
    { return !(bit_ref_.p_data_ == other.bit_ref_.p_data_ && bit_ref_.mask_offset_ == other.bit_ref_.mask_offset_); }

    bool operator >(const bit_iterator& other) const
    { return bit_ref_.p_data_ > other.bit_ref_.p_data_ || (bit_ref_.p_data_ == other.bit_ref_.p_data_ && bit_ref_.mask_offset_ > other.bit_ref_.mask_offset_); }

    bool operator >=(const bit_iterator& other) const
    { return bit_ref_.p_data_ >= other.bit_ref_.p_data_ || (bit_ref_.p_data_ == other.bit_ref_.p_data_ && bit_ref_.mask_offset_ >= other.bit_ref_.mask_offset_); }

    bool operator <(const bit_iterator& other) const
    { return bit_ref_.p_data_ < other.bit_ref_.p_data_ || (bit_ref_.p_data_ == other.bit_ref_.p_data_ && bit_ref_.mask_offset_ < other.bit_ref_.mask_offset_); }

    bool operator <=(const bit_iterator& other) const
    { return bit_ref_.p_data_ <= other.bit_ref_.p_data_ || (bit_ref_.p_data_ == other.bit_ref_.p_data_ && bit_ref_.mask_offset_ <= other.bit_ref_.mask_offset_); }

    BIT_REF bit_ref()
    { return bit_ref_; }
private:
   BIT_REF bit_ref_;
};

template<class BIT_REF>
class bit_iterator_reverse
{
    //! codestyle check
    static_assert(std::is_same_v<BIT_REF, bit_reference> || std::is_same_v<BIT_REF, bit_reference_const>, "Type of the BIT_REF could'nt be custom");

public:
    typedef typename std::random_access_iterator_tag iterator_category;
    typedef bool value_type;
    typedef ptrdiff_t difference_type;
    typedef BIT_REF* pointer;
    typedef BIT_REF reference;

private:
    BIT_REF get_shifted_bitref(difference_type offset) const {
        uint8_t new_mask_offset     = (direct_iter_.bit_ref_.mask_offset_ + offset) % 8;
        difference_type addr_offset = (direct_iter_.bit_ref_.mask_offset_ + offset) / 8;

        return BIT_REF(direct_iter_.bit_ref_.p_data_ + addr_offset, new_mask_offset);
    } 

public:
    bit_iterator_reverse():
        direct_iter_(){}

    bit_iterator_reverse(const BIT_REF& pt):
        direct_iter_(pt){}

    bit_iterator_reverse(const bit_iterator_reverse& other) = default;
 
    bit_iterator_reverse& operator =(const bit_iterator_reverse& other) = default;

    ~bit_iterator_reverse() = default;

    reference operator *()
    { return direct_iter_.bit_ref_; }

    // TODO: check
    pointer operator ->() 
    { return &(direct_iter_.bit_ref_); }

    reference operator [](difference_type idx) { 
        return get_shifted_bitref(-idx);
    }

    bit_iterator_reverse& operator ++() {
        direct_iter_.operator--();
        return *this;
    }

    bit_iterator_reverse operator ++(int)
    { return bit_iterator_reverse(this->operator++()); }

    bit_iterator_reverse& operator --() {
        direct_iter_.operator--();
        return *this;
    }

    bit_iterator_reverse operator --(int)
    { return bit_iterator_reverse(this->operator--()); }

    bit_iterator_reverse  operator +(difference_type offset) const
    { return bit_iterator_reverse(get_shifted_bitref(-offset)); }

    bit_iterator_reverse& operator +=(difference_type offset){
        direct_iter_.bit_ref_ = get_shifted_bitref(-offset);
        return *this;
    }

    bit_iterator_reverse operator -(difference_type offset) const
    { return bit_iterator_reverse(get_shifted_bitref(offset)); }

    bit_iterator_reverse& operator -=(difference_type offset) {
        direct_iter_.bit_ref_ = get_shifted_bitref(offset);
        return *this;
    }

    difference_type operator -(const bit_iterator_reverse& other) const{
        return other.direct_iter_ - direct_iter_;
    }

    bool operator ==(const bit_iterator_reverse& other) const
    { return direct_iter_.operator==(other.direct_iter_); }

    bool operator !=(const bit_iterator_reverse& other) const
    { return direct_iter_.operator!=(other.direct_iter_); }

    bool operator >(const bit_iterator_reverse& other) const
    { return direct_iter_.operator<(other.direct_iter_); }

    bool operator >=(const bit_iterator_reverse& other) const
    { return direct_iter_.operator<=(other.direct_iter_); }

    bool operator <(const bit_iterator_reverse& other) const
    { return direct_iter_.operator>(other.direct_iter_); }

    bool operator <=(const bit_iterator_reverse& other) const
    { return direct_iter_.operator>=(other.direct_iter_); }

    // TODO: operator <=>
private:
   bit_iterator<BIT_REF> direct_iter_;
};

}; // namespace nstd

#endif // NSTD_ITERATOR_H
