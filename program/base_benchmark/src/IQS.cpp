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
#include <random>

/**
 *  Function to play with IQS implementation of random
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param lhs left index, inclusive
 * @param rhs left index, inclusive
 * @return a random number [lhs, rhs]
 */
template<class Container, class Type>
inline std::size_t IQS<Container, Type>::random_between(std::size_t lhs, std::size_t rhs) {
    return lhs + std::rand() % (rhs - lhs);
}

/**
 * Hoare's partition implementation. Guarantees that all element at the left of the pivot
 * are lower or equal to the pivot value and all elements to the right of it are greater
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param pivot_value pivot value to move
 * @param lhs the left boundary for partition algorithm (inclusive)
 * @param rhs the right boundary for partition algorithm (inclusive)
 * @return std::size_t the index on which the partition value belongs
 */
template<class Container, class Type>
inline std::size_t IQS<Container, Type>::partition(Type pivot_value, std::size_t lhs, std::size_t rhs) {
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
 * Ducth-flag problem algorithm.
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param pivot_value the pivot value to use
 * @param lhs the left boundary for partition algorithm (inclusive)
 * @param rhs the right boundary for partition algorithm (inclusive)
 * @return std::size_t the index on which the partition value belongs
 */
template<class Container, class Type>
inline std::size_t IQS<Container, Type>::partition_redundant(Type pivot_value, std::size_t lhs, std::size_t rhs) {
    std::size_t i = lhs;
    std::size_t j = lhs;
    std::size_t k = rhs;

    while (j < k) {
        if(this->container[j] < pivot_value){
            this->swap(this->container, i++,j++);
        }
        else if (this->container[j] > pivot_value){
            this->swap(this->container, j,--k);
        }
        else {
            j++;
        }
    }
    #ifdef FORCE_PIVOT_SELECTION_LEFT
        return i; // return left pivot
    #elif FORCE_PIVOT_SELECTION_RIGHT
        return k; // return left pivot
    #else
            return (i + k) / 2; // if there is a group, then return the middle element to guarantee a position
    #endif
}


/**
 * Swap implementation. Inheritable and inline in order to play with it
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param lhs left index to be swapped
 * @param rhs right index to be swapped
 * @param container the container to apply the function
 */
template<class Container, class Type>
inline void IQS<Container, Type>::swap(Container &container, std::size_t lhs, std::size_t rhs) {
    std::swap(container[lhs], container[rhs]);
}

/**
 * Returns the next sorted element
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @return the next sorted element
 */
template<class Container, class Type>
Type IQS<Container, Type>::next() {
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resize the search window
        std::size_t top_element = this->stack.top();
        if (this->extracted_count == this->stack.top() ){
            this->extracted_count++;
            this->stack.pop();
            return this->container[top_element];
        }
        #ifdef FIXED_PIVOT_SELECTION
            std::size_t pivot_idx = this->extracted_count;
        #else
            std::size_t pivot_idx = this->random_between(this->extracted_count, top_element);
        #endif

        Type pivot_value = this->container[pivot_idx];

        // pivot partition and indexing
//        pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);
        // We displace the selection for the top element in order to prevent hanging
        // you can't just use top_element -1 at the first iteration, as you still need to sort the last element. duh!
        // pivot partition and indexing
        #ifdef USE_FAT_PARTITION
            pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, top_element);
        #else
            pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);
        #endif


        // Push and recurse the loop
        this->stack.push(pivot_idx);
    }
}

template<class Container, class Type>
IQS<Container, Type>::IQS(Container &container): container(container) {
    this->extracted_count = 0;
    this->stack = std::stack<std::size_t>();
    this->stack.push(container.size()-1);
}
