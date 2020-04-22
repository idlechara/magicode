//
// Created by kuky_nekoi on 21-04-20.
//

#ifndef IQS_TEST_BAREBONEIIQS_H
#define IQS_TEST_BAREBONEIIQS_H
#include "cstdlib"
#include "../flags.h"

template <class T>
class BareBoneIIQS {
public:
    BareBoneIIQS(T *target_ptr, size_t target_size);
    ~BareBoneIIQS();
    inline void swap(size_t idx_1, size_t idx_2);
    inline size_t partition(T pivot_value, size_t left_idx, size_t right_idx);
    inline size_t stack_pop();
    inline size_t stack_peek();
    inline void stack_push(size_t value);
    T next();
    T bfprt(size_t lhs, size_t rhs, size_t median_length);
    size_t median(size_t lhs, size_t rhs);

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
    size_t stack_max_length;
    T *target_ptr;
};


#endif //IQS_TEST_BAREBONEIIQS_H
