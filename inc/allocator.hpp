#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <string.h>

//? is it a good idea for allocators make the option for reallocating storage in allocator or maybe it's better to implement chunk tactic from https://habr.com/ru/post/505632/

template<class T, size_t N_BLOCKS, unsigned int N_VALS_IN_BLOCK = 1>
class PoolAllocator
{
    static_assert(!std::is_same_v<T, void>, "Type of the allocator can not be void");
    static_assert(N_VALS_IN_BLOCK > 0, "specialize N_VALS_IN_BLOCK > 0");

    // static_assert(BLOCK_SIZE > sizeof(chunk_list), "size of the block is too small, not supporting in current realization");

    constexpr static size_t BLOCK_SIZE = sizeof(T) * N_VALS_IN_BLOCK;

public:
    typedef T value_type;

    PoolAllocator() = delete;

    //? is adding allignment required?
    PoolAllocator(uint n_init_blocks) {
        assert(n_init_blocks > 0 && "allocator doesn't support empty storage");
        
        data_ = new uint8_t[BLOCK_SIZE * n_init_blocks];
        n_blocks_ = n_init_blocks;
        free_chunks_ = new (data_) chunk_list(n_init_blocks);
    }
        
    //? meaning ?
    PoolAllocator(const PoolAllocator& other):
        PoolAllocator(other.n_blocks_)
    { memcpy(data_, other.data_, other.n_blocks_ * BLOCK_SIZE); }

    /// allocate array of value_type, which size in count_objects
    T* allocate(size_t count_objects) {

        if(free_chunks_ == NULL) return NULL;
        
        chunk_list* cur_chunk = free_chunks_;

        while(cur_chunk != NULL && cur_chunk->n_blocks_ < count_objects) {
            T* allocated_data = reinterpret_cast<T*>(cur_chunk);

            if(cur_chunk->n_blocks_ == count_objects) {
                cur_chunk->~chunk_list();
            } else {
                uint8_t* parted_chunk_place = reinterpret_cast<uint8_t*>(cur_chunk) + sizeof(chunk_list) + count_objects * sizeof(T);
                chunk_list* parted_chunk = new (parted_chunk_place) chunk_list(nstd::move(*cur_chunk));

                parted_chunk->n_blocks_ -= count_objects;
                // we dont call cur_chunk->~chunk_list(), cause move constructor of parted chunk already has cleaned data of cur_chunk
            }

            return allocated_data;
        }

        // fragmentation case
        return NULL;
    }

    void deallocate(T* ptr, size_t count_objects) {
        uint8_t* casted_ptr = reinterpret_cast<uint8_t*>(ptr);

        if(data_ < casted_ptr || casted_ptr < data_ + n_blocks * BLOCK_SIZE)
    }

private:
    struct chunk_list {

        chunk_list():
            chunk_list(0){}

        chunk_list(uint n_blocks):
            next_(NULL),
            prev_(NULL),
            n_blocks_(n_blocks){}

        chunk_list(const chunk_list& other) = delete;
        chunk_list(chunk_list&& other):
            chunk_list()
        { 
            std::swap(*this, other); 
            if(this->next_)
                { this->next_->prev_ = this; }
            if(this->prev_)
                { this->prev_->next_ = this; }
        }

        ~chunk_list() {
            if(next_)
                { next_->prev_ = prev_; }

            if(prev_)
                { prev_->next_ = next_; }

            // for safety
            next_ = prev_ = NULL;
            n_blocks_ = 0;
        }

        void append(uint8_t* placement_pos, uint n_blocks) {

            //? is performance ok, maybe just direct cast to chunk_list*
            chunk_list* new_obj = new (placement_pos) chunk_list(n_blocks);

            new_obj->next_ = next_;
            new_obj->prev_ = this;

            if(new_obj->next_) {
                new_obj->next->prev = new_obj;
            }

            this->next = new_obj;
        }

        chunk_list* next_, prev_;
        uint n_blocks_;
    };

private:
    uint8_t*    data_;
    size_t      n_blocks_;
    chunk_list* free_chunks_;
};

template<class T, class U>
bool operator==(const Allocator<T>&, const Allocator<U>&);
template<class T, class U>
bool operator!=(const Allocator<T>&, const Allocator<U>&);

#endif // ALLOCATOR_H
