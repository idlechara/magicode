//
// Created by kuky_nekoi on 19-04-20.
//

#ifndef UNTITLED1_BAREBONEIQS_H
#define UNTITLED1_BAREBONEIQS_H
#include "cstdlib"
#include "../flags.h"

template<class T>
class BareBoneIQS {
public:
    BareBoneIQS(T *target_ptr, size_t target_size);
    ~BareBoneIQS();
    inline void swap(size_t idx_1, size_t idx_2);
    inline size_t partition(T pivot_value, size_t left_idx, size_t right_idx);
    inline size_t stack_pop();
    inline size_t stack_peek();
    inline void stack_push(size_t value);
    T next();

private:
    /**
     * In this example we used a stack which is the same lenght of the array. This is only for
     * testing purposes and can be changed into a proper stack later on if desired
     *
     */
    size_t *stack;
    size_t stack_length;

    size_t target_size;
    size_t extracted_count;
    T *target_ptr;
};


#endif //UNTITLED1_BAREBONEIQS_H
