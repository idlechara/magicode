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

#include "IQS.h"
#include <cstdlib>

/**
 *  Function to play with IQS implementation of random
 * @param lhs left index, inclusive
 * @param rhs left index, inclusive
 * @return a number [lhs, rhs]
 */
template<class Container, class Type>
inline size_t IQS<Container, Type>::random_between(size_t lhs, size_t rhs) {
    return lhs + (rand() % (rhs - lhs));
}

/**
 * Hoare's partition implementation. Guarantees that all element at the left of the pivot
 * are lower or equal to the pivot value and all elements to the right of it are greater
 * @param pivot_value pivot value to move
 * @param lhs left bound inclusive
 * @param rhs right bound inclusive
 * @return correct index of the pivot
 */
template<class Container, class Type>
inline size_t IQS<Container, Type>::partition(Type pivot_value, size_t lhs, size_t rhs) {
    lhs--;
    rhs++;
    while(1){
        do{ // Advance left index
            lhs++;
        } while(this->container[lhs] < pivot_value);

        do{ // Advance right index
            rhs--;
        } while(pivot_value < this->container[rhs]);

        // base case
        if (lhs >= rhs) return rhs;

        // swap elements
        this->swap(this-> container, lhs, rhs);
    }
}

/**
 * Original IQS partition algorithm implementation
 * @param pivot_idx pivot index to select
 * @param lhs left bound inclusive
 * @param rhs right bound inclusive
 * @return correct index of the pivot
 */
template<class Container, class Type>
inline size_t IQS<Container, Type>::partition_redundant(Type pivot_value, size_t lhs, size_t rhs) {
    size_t i = lhs;
    size_t k = rhs;
    i--;                        // Hoare partition
    k++;
    while (1) {
        while (this->container[++i] < pivot_value);
        while (this->container[--k] > pivot_value);
        if (i >= k)
            break;
        this->swap(this->container, i, k);
    }
//    i = k++;
//    while(i > lhs && this->container[i] == pivot_value)
//        i--;
//      do{k++;} while(k < rhs && this->container[k] == pivot_value);

    return k++;
}

/**
 * Swap implementation. Inheritable and inline in order to play with it
 * @param lhs left bound inclusive
 * @param rhs right bound inclusive
 * @param container the container to apply the function
 */
template<class Container, class Type>
inline void IQS<Container, Type>::swap(Container &container, size_t idx_1, size_t idx_2) {
    std::swap(container[idx_1], container[idx_2]);
}

/**
 * Returns the next sorted element
 * @tparam Container
 * @return the next sorted element
 */
template<class Container, class Type>
Type IQS<Container, Type>::next() {
    bool first_iteration = true;
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resise the search window
        size_t top_element = this->stack.top();
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
//        pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);
        // We displace the selection for the top element in order to prevent hanging
        // you can't just use top_element -1 at the first iteration, as you still need to sort the last element. duh!
        pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, top_element);

        // Push and recurse the loop
        this->stack.push(pivot_idx);
    }
}

template<class Container, class Type>
IQS<Container, Type>::IQS(Container &container): container(container) {
    this->extracted_count = 0;
    this->stack = std::stack<size_t>();
    this->stack.push(container.size()-1);
    this->max_stack_size = container.size();
}
