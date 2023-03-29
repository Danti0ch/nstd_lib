#ifndef ALLOCATION_H
#define ALLOCATION_H

#include <cstdlib>
#include <utility>
#include <stdint.h>

// TODO: remake under iterators
template<typename T>
void clear_mem(T* data, size_t init_pos, size_t n_elems) {
    for(uint i = 0; i < n_elems; i++){
        (*(data + (init_pos + i))).~T();
    }
}

template<typename T>
void set_mem(T* data, size_t init_pos, size_t n_elems) {
    for(uint i = 0; i < n_elems; i++) {
        new (data + (init_pos + i)) T();
    }
}

template<typename T>
void set_mem(T* data, size_t init_pos, size_t n_elems, const T& val) {
    for(uint i = 0; i < n_elems; i++) {
        new (data + (init_pos + i)) T(val);
    }
}

template<typename T>
void set_mem(T* data, size_t init_pos, T* from, size_t from_init_pos, size_t n_elems, bool is_move = false) {
    
    if(!is_move) {
        for(uint i = 0; i < n_elems; i++) {
            new (data + (init_pos + i)) T(from[from_init_pos + i]);
        }
    }
    else{
        for(uint i = 0; i < n_elems; i++) {
            new (data + (init_pos + i)) T(std::move(from[from_init_pos + i]));
        }
    }
}

#endif // ALLOCATION_H
