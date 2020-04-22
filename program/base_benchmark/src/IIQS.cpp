//
// Created by kuky_nekoi on 20-04-20.
//

#include "IIQS.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

/**
 * Returns the next sorted element
 * @tparam Container
 * @return the next sorted element
 */
template<class Container, class Type>
Type IIQS<Container, Type>::next() {
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resise the search window
        size_t top_element = this->stack.top();
        size_t range = top_element - this->extracted_count;
        /*size_t p30_idx = range * 0.3; // actually, if we don't care about balancing the stack, you can ignore the p30 condition*/
        size_t p70_idx = range * 0.7;

        if (this->extracted_count == this->stack.top() ){
            this->extracted_count++;
            this->stack.pop();
            return this->container[top_element];
        }

        #ifdef FIXED_PIVOT_SELECTION
            size_t pivot_idx = this->extracted_count;
        #else
            size_t pivot_idx = this->random_between(this->extracted_count, top_element);
        #endif

        Type pivot_value = this->container[pivot_idx];

        // pivot partition and indexing
        pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);

        // IIQS changes start! only check if range is less than the square root of the total size
        // First, we need to check if this pointer belongs P70 \union P30
        size_t pivot_p_idx = pivot_idx - this->extracted_count;
        if (/*p30_idx > pivot_idx ||*/ range > this->max_stack_size && pivot_idx > p70_idx){
            // if we enter here, then it's because the index needs to be recomputed.
            // So, we ditch the index and get a nice approximate median median and reuse previous computation
            size_t previous_pivot_idx = pivot_idx;
            pivot_idx = this->bfprt(this->container, this->extracted_count, pivot_idx, 5);
            pivot_value = this->container[pivot_idx];
            // then we re-partition, assuming that this median is better
            pivot_idx = this->partition(pivot_value, this->extracted_count, previous_pivot_idx);
        }

        // Push and recurse the loop
        this->stack.push(pivot_idx);
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

template<class Container, class Type>
inline size_t IIQS<Container, Type>::median(Container &container, size_t lhs, size_t rhs) {
    // in practice, this invokes heapsort each time
    std::sort(container.begin() + lhs, container.begin() + rhs);
    return (lhs + rhs) / 2;
}

template<class Container, class Type>
IIQS<Container, Type>::IIQS(Container &container): IQS<Container, Type>(container) {
    this->extracted_count = 0;
    this->stack = std::stack<size_t>();
    this->stack.push(container.size()-1);
    this->max_stack_size = std::sqrt(container.size()) + 1;
}