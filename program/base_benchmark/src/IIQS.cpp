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
#include "IIQS.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

 /**
  * Returns the next sorted element
  * @tparam Container container type to handle on the class
  * @tparam Type type used for comparison
  * @return the next sorted element
  */
template<class Container, class Type>
Type IIQS<Container, Type>::next() {
    bool already_partitioned = false;
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resise the search window
        size_t top_element = this->stack.top();
        size_t range = top_element - this->extracted_count;
        /*size_t p30_idx = range * 0.3; // actually, if we don't care about balancing the stack, you can ignore the p30 condition*/
        size_t p70_idx = range * 0.7;

        if (this->extracted_count == top_element){
            this->extracted_count++;
            this->stack.pop();
            return this->container[top_element];
        }
//
//        //to improve performance on redundant arrays WIP
//        if (this->stack.size() > 1 && this->container[top_element] == this->container[this->extracted_count]){
//            Type result = this->container[this->extracted_count];
//            this->extracted_count ++;
//            return result;
//        }


        #ifdef FIXED_PIVOT_SELECTION
            size_t pivot_idx = this->extracted_count;
        #else
            size_t pivot_idx = this->random_between(this->extracted_count, top_element);
        #endif

        Type pivot_value = this->container[pivot_idx];

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
            pivot_idx = this->bfprt(this->container, this->extracted_count, pivot_idx, 5);
            pivot_value = this->container[pivot_idx];
            // then we re-partition, assuming that this median is better


            #ifdef USE_FAT_PARTITION
                pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, previous_pivot_idx);
            #else
                pivot_idx = this->partition(pivot_value, this->extracted_count, previous_pivot_idx);
            #endif

        }

        // Push and recurse the loop
        this->stack.push(pivot_idx);
        already_partitioned = true;
    }
}

/**
 * In-place implementation of bfprt. Instead of the classical implementation when auxiliary structures are used
 * this implementation forces two phenomena on the array which both are benefitial to IQS. First, given that we force
 * the selection of the first index, elements near the beggining have a high chance of being good pivots. Second, we
 * don't use extra memory to allocate those median results.
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param container reference to the container element to calculate it's median
 * @param lhs the left index to sort (inclusive)
 * @param rhs the right index to sort (inclusive)
 * @param median_length size of the median to use on bfprt, 5 is commonly used
 * @return
 */
template<class Container, class Type>
inline Type IIQS<Container, Type>::bfprt(Container &container, size_t lhs, size_t rhs, size_t median_length) {
    size_t base_lhs = lhs;
    size_t medians_extracted = 0;

    while(1){
        // reset base conditions
        lhs = base_lhs;

        // check base case
        if( rhs <= base_lhs + median_length) {
            return this->median(container, base_lhs, rhs);
        }

        // tail recursion step for bfprt
        // we ignore median tails as they provide little or no value
        while(lhs + median_length <= rhs){
            size_t median_index = this->median(container, lhs, lhs + median_length);
            //move median to the start of the array
            this->swap(container, median_index, base_lhs + medians_extracted);
            // search for next stride
            lhs += median_length;
            medians_extracted++;
        }
        rhs = medians_extracted + base_lhs;
        medians_extracted = 0;
    }
}

/**
 *
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param container reference to the container element to calculate it's median
 * @param lhs the left boundary for median algorithm (inclusive)
 * @param rhs the right boundary for median algorithm (inclusive)
 * @return
 */
template<class Container, class Type>
inline size_t IIQS<Container, Type>::median(Container &container, size_t lhs, size_t rhs) {
    // in practice, this invokes heapsort each time
    std::sort(container.begin() + lhs, container.begin() + rhs);
    return (lhs + rhs) / 2;
}

/**
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param container reference to the container element to apply IQS on
 */
template<class Container, class Type>
IIQS<Container, Type>::IIQS(Container &container): IQS<Container, Type>(container) {
    this->extracted_count = 0;
    this->stack = std::stack<size_t>();
    this->stack.push(container.size()-1);
}