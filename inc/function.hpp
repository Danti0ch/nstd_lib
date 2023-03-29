#ifndef NSTD_FUNCTION_H
#define NSTD_FUNCTION_H

#include <cassert>
#include <stdexcept>
#include <memory>

// TODO: implement static checks

namespace nstd
{

template<typename T>
class function;

template<typename RetT, typename...  ArgTs>
class function<RetT (ArgTs...)> {
    typedef RetT (* TFuncPt)(ArgTs...);
public:
    function() :
        storage_() {}
    
    template<class T_FUNCTOR>
    function(T_FUNCTOR functor) :
        storage_(new functor_storage<T_FUNCTOR>(functor)) {}
    
    template<class T_OBJECT, class T_FUNCTOR>
    function(T_FUNCTOR T_OBJECT::*functor) :
        storage_(new member_functor_storage<T_FUNCTOR, ArgTs...>(functor)) {}
    
    function(const function& other) :
        storage_(other.storage_){}
    
    // TODO: move constructor

    function& operator=(const function& other) {
        storage_ = other.storage_;

        return *this;
    }

    RetT operator()(ArgTs... args) {
        if(!storage_) throw std::runtime_error("function is not initialized");

        return storage_->operator()(args...);
    }
    
private:

class storage_base{
public:
    virtual ~storage_base() {}
    virtual RetT operator()(ArgTs...) = 0;
};

template<class T_FUNCTOR>
class functor_storage : public storage_base{
public:
    functor_storage(T_FUNCTOR functor):
        functor_(functor) {}

    functor_storage(const functor_storage& other) = delete;
    functor_storage& operator =(const functor_storage& other) = delete;

    RetT operator()(ArgTs... args) override
     { return functor_(args...); }

private:
    T_FUNCTOR functor_;
};

template<class T_FUNCTOR, class T_OBJECT, typename ... RestArgumentTypes>
class member_functor_storage : public storage_base{
    typedef T_FUNCTOR T_OBJECT::* T_MEMBER_FUNC;
public:
    member_functor_storage(T_MEMBER_FUNC member_functor) :
        member_functor_(member_functor){}
    
    RetT operator()(T_OBJECT obj, RestArgumentTypes... args) override
     { return (obj.*member_functor_)(args...);}

private:
    T_MEMBER_FUNC member_functor_;
};

private:
    std::shared_ptr<storage_base> storage_;
};

};

#endif // NSTD_FUNCTION_H
