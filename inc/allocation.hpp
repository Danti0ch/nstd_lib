#ifndef ALLOCATION_H
#define ALLOCATION_H

#include <cstdlib>
#include <utility>

// TODO: remake under iterators
template<typename T>
void clear_mem(uint8_t* data, size_t init_pos, size_t n_elems) {
    for(uint i = 0; i < n_elems; i++){
        (*(T*)(data + (init_pos + i) * sizeof(T))).~T();
    }
}

template<typename T>
void set_mem(uint8_t* data, size_t init_pos, size_t n_elems) {
    for(uint i = 0; i < n_elems; i++) {
        new (data + (init_pos + i) * sizeof(T)) T();
    }
}

template<typename T>
void set_mem(uint8_t* data, size_t init_pos, size_t n_elems, const T& val) {
    for(uint i = 0; i < n_elems; i++) {
        new (data + (init_pos + i) * sizeof(T)) T(val);
    }
}

template<typename T>
void set_mem(uint8_t* data, size_t init_pos, uint8_t* from, size_t from_init_pos, size_t n_elems, bool is_move = false) {
    
    if(!is_move) {
        for(uint i = 0; i < n_elems; i++) {
            new (data + (init_pos + i) * sizeof(T)) T(from[from_init_pos + i]);
        }
    }
    else{
        for(uint i = 0; i < n_elems; i++) {
            new (data + (init_pos + i) * sizeof(T)) T(std::move(from[from_init_pos + i]));
        }
    }
}

#endif // ALLOCATION_H
