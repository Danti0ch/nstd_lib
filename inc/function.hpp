#ifndef NSTD_FUNCTION_H
#define NSTD_FUNCTION_H

#include <cassert>
#include <stdexcept>
#include <memory>

// TODO: implement shared_ptr

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
    
    function(TFuncPt func_pt) :
        storage_(new function_storage(func_pt)) {}

    template<class T_FUNCTOR>
    function(T_FUNCTOR functor) :
        storage_(new functor_object_storage<T_FUNCTOR>(functor)) {}
    
    function(function& other) :
        storage_(other.storage_){}
    
    function(function&& other) :
        storage_()
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

class function_storage : public storage_base {
public:
    function_storage(TFuncPt func_pt) :
        func_pt_(func_pt) {}

    RetT operator()(ArgTs... args) override {
        assert(func_pt_); 

        return func_pt_(args...); 
    }
    
private:
    TFuncPt func_pt_;
};

template<class T_FUNCTOR>
class functor_object_storage : public storage_base{
public:
    functor_object_storage(T_FUNCTOR functor):
        functor_(functor) {}

    RetT operator()(ArgTs... args) override
     { return functor_(args...); }
    
private:
    T_FUNCTOR functor_;
};

private:
    std::shared_ptr<storage_base> storage_;
};

};

#endif // NSTD_FUNCTION_H
