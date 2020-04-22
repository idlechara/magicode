//
// Created by kuky_nekoi on 21-04-20.
//

#include <cmath>
#include <cstdio>
#include "BareBoneIIQS.h"
/* This constructor allows in-place ordering */
template <class T>
BareBoneIIQS<T>::BareBoneIIQS(T *target_ptr, size_t target_size){
    this->target_ptr = target_ptr;
    this->target_size = target_size;
    this->stack_max_length = std::sqrt(target_size) + 1;
    this->stack = (size_t *) malloc(this->stack_max_length * sizeof(size_t)) ;
    this->stack[0] = target_size - 1; // index of the last element
    this->stack_length = 1; //starts with a single element, the top
    this->extracted_count = 0; // this way, after adding +1, we can partition as whole
}
template <class T>
BareBoneIIQS<T>::~BareBoneIIQS() {
    free(this->stack);
}

/**
 * @brief Swaps two elements in the referenced array
 *
 * @param idx_1 index to be swapped
 * @param idx_2 index to be swapped
 */
template <class T>
inline void BareBoneIIQS<T>::swap(size_t idx_1, size_t idx_2){
    T _temp_val = this->target_ptr[idx_1];
    this->target_ptr[idx_1] = this->target_ptr[idx_2];
    this->target_ptr[idx_2] = _temp_val;
}

/**
 * @brief Implementation of Hoare's partition algorithm. Can be found on
 * Cormen's "Introduction to algorithms - 2nd edition" p146
 *
 * @param pivot_value
 * @param left_idx
 * @param right_idx
 * @return size_t
 */
template <class T>
inline size_t BareBoneIIQS<T>::partition(T pivot_value, size_t left_idx, size_t right_idx){
    if (left_idx == right_idx) return left_idx;

    left_idx--;
    right_idx++;

    while(1){
        do{
            left_idx++;
        } while(this->target_ptr[left_idx] < pivot_value);
        do{
            right_idx--;
        } while(pivot_value < this->target_ptr[right_idx]);
        if (left_idx >= right_idx) return right_idx;
        this->swap(left_idx, right_idx);
    }
}

/**
 * @brief Pops the last element on the stack
 *
 * @return size_t element at the top of the stack
 */
template<class T>
inline size_t BareBoneIIQS<T>::stack_pop(){
    return this->stack[--this->stack_length];
}

/**
 * @brief Peeks the last element on the stack
 *
 * @return size_t element at the top of the stack
 */
template<class T>
inline size_t BareBoneIIQS<T>::stack_peek(){
    return this->stack[this->stack_length-1];
}

/**
 * @brief Inserts an element on the top of the stack
 *
 * @param value the element to insert
 */
template<class T>
inline void BareBoneIIQS<T>::stack_push(size_t value){
    this->stack[this->stack_length] = value;
    this->stack_length++;
}

/**
 * @brief Retrieves the next sorted element. The basic idea is to
 * use quick select to find the smallest element, but store the pivots along the
 * way in order to shortent future calculations.
 *
 * @return T the next sorted element
 */
template<class T>
T BareBoneIIQS<T>::next() {
//    #ifndef BFPRT_LIMITS_PER_ITERATION
//        size_t p70_idx = this->target_size * 0.7;
//    #endif
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resise the search window
        size_t top_element = this->stack_peek();
        size_t range = top_element - this->extracted_count;
        /*size_t p30_idx = range * 0.3; // actually, if we don't care about balancing the stack, you can ignore the p30 condition*/
//        #ifdef BFPRT_LIMITS_PER_ITERATION
            size_t p70_idx = range * 0.7;
//        #endif

        if (this->extracted_count == this->stack_peek() ){
            this->extracted_count++;
            this->stack_pop();
            return this->target_ptr[top_element];
        }

        #ifdef FIXED_PIVOT_SELECTION
            size_t pivot_idx = this->extracted_count;
        #else
            size_t rand_range = this->stack_peek() - this->extracted_count;
            size_t pivot_idx = this->extracted_count + (rand() % rand_range);
        #endif

        T pivot_value = this->target_ptr[pivot_idx];

        // pivot partition and indexing
        pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);

        // IIQS changes start! only check if range is less than the square root of the total size
        // First, we need to check if this pointer belongs P70 \union P30
        size_t pivot_p_idx = pivot_idx - this->extracted_count;
        if (/*p30_idx > pivot_idx ||*/ range > this->stack_max_length && pivot_idx > p70_idx){
            // if we enter here, then it's because the index needs to be recomputed.
            // So, we ditch the index and get a nice approximate median median and reuse previous computation
            size_t previous_pivot_idx = pivot_idx;
            pivot_idx = this->bfprt(this->extracted_count, pivot_idx, 5);
            pivot_value = this->target_ptr[pivot_idx];
            // then we re-partition, assuming that this median is better
            pivot_idx = this->partition(pivot_value, this->extracted_count, previous_pivot_idx);
        }

        // Push and recurse the loop
        this->stack_push(pivot_idx);
    }
}


/**
 * In-place implementation of bfprt. Instead of the classical implementation when auxiliary structures are used
 * this implementation forces two phenomena on the array which both are benefitial to IQS. First, given that we force
 * the selection of the first index, elements near the beggining have a high chance of being good pivots. Second, we
 * don't use extra memory to allocate those median results.
 * @tparam Container
 * @tparam Type
 * @param container
 * @param lhs
 * @param rhs
 * @param median_length
 * @return
 */
template<class T>
inline T BareBoneIIQS<T>::bfprt(size_t lhs, size_t rhs, size_t median_length) {
    size_t base_lhs = lhs;
    size_t medians_extracted = 0;

    while(1){
        // reset base conditions
        lhs = base_lhs;

        // check base case
        if( rhs <= base_lhs + median_length) {
            return this->median(base_lhs, rhs);
        }

        // tail recursion step for bfprt
        while(lhs + median_length <= rhs){
            size_t median_index = this->median(lhs, lhs + median_length);
            //move median to the start of the array
            this->swap(median_index, base_lhs + medians_extracted);
            // search for next stride
            lhs += median_length;
            medians_extracted++;
        }
        rhs = medians_extracted + base_lhs;
        medians_extracted = 0;
    }
}

template<class T>
inline size_t BareBoneIIQS<T>::median(size_t lhs, size_t rhs) {
    size_t k = (lhs + rhs) / 2;
    size_t pivot_idx = k;
    while(1) {
        if (rhs == lhs) return lhs;
        // forced insert_sort
        pivot_idx = this->partition(this->target_ptr[(lhs + rhs) / 2], lhs, rhs);

        if (pivot_idx == k) return k;
        else {
            if (k < pivot_idx) rhs = pivot_idx - 1;
            else lhs = pivot_idx + 1;
        }
    }
}