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
    
    function(nullptr_t) :
        function(){}
    
    template<class T_FUNCTOR>
    function(T_FUNCTOR functor) :
        storage_(new functor_storage<T_FUNCTOR>(functor)) {}
    
    template<class T_OBJECT, class T_FUNCTOR>
    function(T_FUNCTOR T_OBJECT::*functor) :
        storage_(new member_functor_storage<T_FUNCTOR, ArgTs...>(functor)) {}
    
    function(const function& other) :
        storage_(other.storage_){}
    
    function(function&& other) :
        storage_(std::move(other.storage_)){}
    
    function& operator=(const function& other) {
        function(other).swap(*this);
        return *this;
    }

    function& operator=(function&& other) {
        function(std::move(other)).swap(*this);
        return *this;
    }

    template<class T_FUNCTOR>
    function& operator=(T_FUNCTOR functor) {
        function(functor).swap(*this);
        return *this;
    }

    template<class T_OBJECT, class T_FUNCTOR>
    function& operator=(T_FUNCTOR T_OBJECT::*functor){ 
        function(functor).swap(*this);
        return *this;
    }
    
    function& operator=(nullptr_t) {
        storage_.reset();
        return *this;
    }

    RetT operator()(ArgTs... args) const {
        if(!storage_) throw std::runtime_error("function is not initialized");

        return storage_->operator()(args...);
    }
    
    void swap(function& other)
    { storage_.swap(other.storage_); }
    
    operator bool() const
    { return storage_.operator bool(); }

    const std::type_info& target_type() const {
        return storage_->type();
    }

    template<class T>
    T* target() {
        const_cast<T*>(target());
    }

    template<class T>
    const T* target() {
        if(target_type() == typeid(T)) return storage_.get();
        return nullptr;
    }
    
private:

class storage_base{
public:
    virtual ~storage_base() {}
    virtual RetT operator()(ArgTs...) const = 0;
    virtual const std::type_info& type() const = 0;
};

template<class T_FUNCTOR>
class functor_storage : public storage_base{
public:
    functor_storage(T_FUNCTOR functor):
        functor_(functor) {}

    functor_storage(const functor_storage& other) = delete;
    functor_storage& operator =(const functor_storage& other) = delete;

    RetT operator()(ArgTs... args) const override
     { return functor_(args...); }

    const std::type_info& type() const override
    { return typeid(T_FUNCTOR); }
    
    
private:
    T_FUNCTOR functor_;
};

template<class T_FUNCTOR, class T_OBJECT, typename ... RestArgumentTypes>
class member_functor_storage : public storage_base{
    typedef T_FUNCTOR T_OBJECT::* T_MEMBER_FUNC;
public:
    member_functor_storage(T_MEMBER_FUNC member_functor) :
        member_functor_(member_functor){}
  
    // RetT operator()(T_OBJECT obj, RestArgumentTypes... args) override
    //  { return (obj.*member_functor_)(args...); }

    RetT operator()(T_OBJECT obj, RestArgumentTypes... args) const override
     { return (obj.*member_functor_)(args...); }

    const std::type_info& type() const
    { return typeid(T_MEMBER_FUNC); }

private:
    T_MEMBER_FUNC member_functor_;
};

private:
    std::shared_ptr<storage_base> storage_;
};

template<typename RetT, typename...  ArgTs>
void swap(function<RetT(ArgTs...)> &f1, function<RetT(ArgTs...)> &f2) {
    f1.swap(f2);
}

template<typename RetT, typename...  ArgTs>
bool operator ==(const function<RetT(ArgTs...)> &f, nullptr_t)
{ return !static_cast<bool>(f); }

template<typename RetT, typename...  ArgTs>
bool operator ==(nullptr_t, const function<RetT(ArgTs...)> &f)
{ return !static_cast<bool>(f); }


template<typename RetT, typename...  ArgTs>
bool operator !=(const function<RetT(ArgTs...)> &f, nullptr_t)
{ return static_cast<bool>(f); }

template<typename RetT, typename...  ArgTs>
bool operator !=(nullptr_t, const function<RetT(ArgTs...)> &f)
{ return static_cast<bool>(f); }

};

#endif // NSTD_FUNCTION_H
