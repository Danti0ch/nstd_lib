#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <string.h>

//? is it a good idea for allocators make the option for reallocating storage in allocator or maybe it's better to implement chunk tactic from https://habr.com/ru/post/505632/

//? maybe there could be implementation with multiple values in one block
template<class T, size_t N_BLOCKS>
class PoolAllocator
{
    static_assert(!std::is_same_v<T, void>, "Type of the allocator can not be void");
    static_assert(N_BLOCKS > 0, "specialize N_BLOCKS > 0");

    // static_assert(sizeof(T) > sizeof(chunk_list), "size of the block is too small, not supporting in current realization");

public:
    typedef T value_type;

    PoolAllocator() = delete;

    //? is adding allignment required
    PoolAllocator(size_t n_init_blocks) {
        assert(n_init_blocks > 0 && "allocator doesn't support empty storage");
        
        data_ = new uint8_t[sizeof(T) * n_init_blocks];
        n_blocks_ = n_init_blocks;
        free_chunks_ = new (data_) chunk_list(n_init_blocks);
    }
        
    //? meaning
    PoolAllocator(const PoolAllocator& other):
        PoolAllocator(other.n_blocks_)
    { memcpy(data_, other.data_, other.n_blocks_ * sizeof(T)); }

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

    //? is linear search of needed chunk_list object is too bad for performance
    void deallocate(T* ptr, size_t count_objects) {
        uint8_t* chunk_to_deallocate_begin = reinterpret_cast<uint8_t*>(ptr);
        uint8_t* chunk_to_deallocate_end   = chunk_to_deallocate_begin + count_objects * sizeof(T);

        if(data_ > chunk_to_deallocate_begin || chunk_to_deallocate_end >= data_ + N_BLOCKS * sizeof(T))
            return;            // TODO: throw except
        
        size_t diff = chunk_to_deallocate_begin - data_;
        if(diff % sizeof(T) != 0)
            return          // TODO: throw except
        
        if(free_chunks_ == NULL) {
            free_chunks_ = new (chunk_to_deallocate_begin) chunk_list(count_objects);
            return;
        }

        chunk_list* cur_chunk = free_chunks_;
        
        if(reinterpret_cast<uint8_t*>(cur_chunk) > chunk_to_deallocate_end) {
            free_chunks_ = cur_chunk->insert_before(chunk_to_deallocate_begin, count_objects);
            return;
        }

        while(cur_chunk != NULL) {
            uint8_t* cur_chunk_begin = reinterpret_cast<uint8_t*>(cur_chunk);
            uint8_t* cur_chunk_end   = cur_chunk_begin + cur_chunk->n_blocks_ * sizeof(T);

            if(cur_chunk_begin < chunk_to_deallocate_end || cur_chunk_end > chunk_to_deallocate_begin)
                return;         // TODO: throw except

            if(cur_chunk_end == chunk_to_deallocate_begin) {
                // has found left free chunk neighbour

                if(cur_chunk->next_ && reinterpret_cast<uint8_t*>(cur_chunk->next_) == chunk_to_deallocate_end) {
                    // has found right free chunk neighbour

                    size_t next_chunk_n_blocks = cur_chunk->next->n_blocks_;
                    cur_chunk->next_->~chunk_list();

                    cur_chunk->n_blocks_ += count_objects + next_chunk_n_blocks;
                } else {
                    // right free chunk neigbour hasn't been found
                    cur_chunk->n_blocks_ += count_objects;
                }
                return;
            } else if(cur_chunk_begin == chunk_to_deallocate_end){
                // right free chunk neigbour hasn't been found

                chunk_list* new_chunk = new (chunk_to_deallocate_begin) chunk_list(nstd::move(cur_chunk));
                new_chunk->n_blocks_ += count_objects;

                return;
            }
            else if(cur_chunk_begin > chunk_to_deallocate_end){
                // first chunk after chunk_to_deallocate(not neigbour)

                cur_chunk->insert_before(chunk_to_deallocate_begin, count_objects);

                return;
            }
        }

        // we couldn't reach this place
        assert(0);
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

        chunk_list* insert_after(uint8_t* placement_pos, uint n_blocks) {

            //? is performance ok, maybe just direct cast to chunk_list*
            chunk_list* new_obj = new (placement_pos) chunk_list(n_blocks);

            new_obj->next_ = next_;
            new_obj->prev_ = this;

            if(new_obj->next_) {
                new_obj->next->prev = new_obj;
            }

            this->next = new_obj;
            return new_obj;
        }

        chunk_list* insert_after(uint8_t* placement_pos, uint n_blocks) {

            //? is performance ok, maybe just direct cast to chunk_list*
            chunk_list* new_obj = new (placement_pos) chunk_list(n_blocks);

            new_obj->next_ = next_;
            new_obj->prev_ = this;

            if(new_obj->next_) {
                new_obj->next_->prev_ = new_obj;
            }

            this->next_ = new_obj;
            return new_obj;
        }

        void insert_before(uint8_t* placement_pos, uint n_blocks) {

            //? is performance ok, maybe just direct cast to chunk_list*
            chunk_list* new_obj = new (placement_pos) chunk_list(n_blocks);

            new_obj->next_ = this;
            new_obj->prev_ = prev_;

            if(new_obj->prev_) {
                new_obj->prev_->next_ = new_obj;
            }

            this->prev_ = new_obj;
        }

        //            FIELDS             //
        chunk_list* next_, prev_;
        uint n_blocks_;
    };

private:
    uint8_t*    data_;
    chunk_list* free_chunks_;
};

//? is it a good idea for allocators make the option for reallocating storage in allocator or maybe it's better to implement chunk tactic from https://habr.com/ru/post/505632/

template<class T>
class StackAllocator
{
    static_assert(!std::is_same_v<T, void>, "Type of the allocator can not be void");

public:
    typedef T value_type;

    StackAllocator() = delete;

    StackAllocator(size_t size) {
        assert(size > 0 && "allocator doesn't support empty storage");

        data_ = new uint8_t[size * sizeof(T)];
        free_block_ = data_;
        *free_block = ???;
    }

    // TODO: StackAllocator(const StackAllocator& other){}

    T* allocate(size_t count_objects) {
        
    }
private:
    uint8_t*    data_;
    uint8_t*    free_block_;
};

//? meaning
/*
    template<class T, class U>
    bool operator==(const Allocator<T>&, const Allocator<U>&);
    template<class T, class U>
    bool operator!=(const Allocator<T>&, const Allocator<U>&);
*/

#endif // ALLOCATOR_H
