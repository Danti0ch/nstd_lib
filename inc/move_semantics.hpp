#ifndef MOVE_SEMANTICS_H
#define MOVE_SEMANTICS_H

namespace nstd {

template<class T> 
struct remove_reference {
    typedef T type;
};

template<class T> 
struct remove_reference<T&> {
    typedef T type;
};

template<class T> 
struct remove_reference<T&&> {
    typedef T type;
};

template<class T>
typename remove_reference<T>::type&& move(T&& obj) {
    return static_cast<typename remove_reference<T>::type&&>(obj);
}

template<class T>
T&& forward(T& a)
{
    return static_cast<T&&>(a);
}

}; // NSTD namespace

#endif // MOVE_SEMANTICS_H
