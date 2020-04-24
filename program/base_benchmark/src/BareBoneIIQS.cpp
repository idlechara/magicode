// Copyright (C) 2020 Erik Regla
// 
// This file is part of Magicode - (I)IQS benchmark.
// 
// Magicode - (I)IQS benchmark is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Magicode - (I)IQS benchmark is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Magicode - (I)IQS benchmark.  If not, see <http://www.gnu.org/licenses/>.

#include <cmath>
#include <cstdio>
#include <algorithm>
#include "BareBoneIIQS.h"
/* This constructor allows in-place ordering */
template <class T>
BareBoneIIQS<T>::BareBoneIIQS(T *target_ptr, size_t target_size): BareBoneIQS<T>(target_ptr, target_size){
}
template <class T>
BareBoneIIQS<T>::~BareBoneIIQS() {
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
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resise the search window
        size_t top_element = this->stack_peek();
        size_t range = top_element - this->extracted_count;
        /*size_t p30_idx = range * 0.3; // actually, if we don't care about balancing the stack, you can ignore the p30 condition*/
        size_t p70_idx = range * 0.7;


        if (this->extracted_count == top_element ){
            this->extracted_count++;
            return this->target_ptr[this->stack_pop()];
        }

        #ifdef FIXED_PIVOT_SELECTION
            size_t pivot_idx = this->extracted_count;
        #else
            size_t rand_range = this->stack_peek() - this->extracted_count;
            size_t pivot_idx = this->extracted_count + (rand() % rand_range);
        #endif

        T pivot_value = this->target_ptr[pivot_idx];

        // pivot partition and indexing
        #ifdef USE_FAT_PARTITION
            pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, top_element);
        #else
            pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);
        #endif

        // IIQS changes start! only check if range is less than the square root of the total size
        // First, we need to check if this pointer belongs P70 \union P30
        size_t pivot_p_idx = pivot_idx - this->extracted_count;
        if (/*p30_idx > pivot_idx ||*/ pivot_idx > p70_idx){
            // if we enter here, then it's because the index needs to be recomputed.
            // So, we ditch the index and get a nice approximate median median and reuse previous computation
            size_t previous_pivot_idx = pivot_idx;
            pivot_idx = this->bfprt(this->extracted_count, pivot_idx, 5);
            pivot_value = this->target_ptr[pivot_idx];
            // then we re-partition, assuming that this median is better
            #ifdef USE_FAT_PARTITION
                pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, previous_pivot_idx);
            #else
                pivot_idx = this->partition(pivot_value, this->extracted_count, previous_pivot_idx);
            #endif
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

/**
 * Median selection via quickselect. We can asume that this process is constant, as it is being always executed
 * with 5 elements (by default, you can change this later)
 * @tparam T
 * @param lhs
 * @param rhs
 * @return
 */
template<class T>
inline size_t BareBoneIIQS<T>::median(size_t lhs, size_t rhs) {
    std::sort(this->target_ptr, this->target_ptr + (rhs-lhs));
    return (lhs + rhs) / 2;
    /* implement heapsort later as it is more cache-friendly for small arrays, I'm too drunk now */
}