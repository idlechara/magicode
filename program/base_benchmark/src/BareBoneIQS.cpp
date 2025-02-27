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

#include "BareBoneIQS.h"
/* This constructor allows in-place ordering */
template <class T>
BareBoneIQS<T>::BareBoneIQS(T *target_ptr, std::size_t target_size){
    this->target_ptr = target_ptr;
    this->target_size = target_size;

    this->stack = (std::size_t *) std::malloc(this->target_size * sizeof(std::size_t)) ;
    this->stack[0] = target_size - 1; // index of the last element
    this->stack_length = 1; //starts with a single element, the top

    this->extracted_count = 0; // this way, after adding +1, we can partition as whole
}

template <class T>
BareBoneIQS<T>::~BareBoneIQS() {
    std::free(this->stack);
}

/**
 * @brief Swaps two elements in the referenced array
 *
 * @param lhs left index to be swapped
 * @param rhs right index to be swapped
 */
template <class T>
inline void BareBoneIQS<T>::swap(std::size_t lhs, std::size_t rhs){
    T _temp_val = this->target_ptr[lhs];
    this->target_ptr[lhs] = this->target_ptr[rhs];
    this->target_ptr[rhs] = _temp_val;
}

/**
 * @brief Implementation of Hoare's partition algorithm. Can be found on
 * Cormen's "Introduction to algorithms - 2nd edition" p146
 * This implementation is not resistant to the case on which the elements are repeated.
 *
 * @param pivot_value the pivot value to use
 * @param lhs the left boundary for partition algorithm (inclusive)
 * @param rhs the right boundary for partition algorithm (inclusive)
 * @return std::size_t the index on which the partition value belongs
 */
template <class T>
inline std::size_t BareBoneIQS<T>::partition(T pivot_value, std::size_t lhs, std::size_t rhs){
    if(lhs == rhs) return lhs;
    lhs--;
    rhs++;

    while(1){
        do{
            lhs++;
        } while(this->target_ptr[lhs] < pivot_value);
        do{
            rhs--;
        } while(pivot_value < this->target_ptr[rhs]);
        if (lhs >= rhs) return rhs;
        this->swap(lhs, rhs);
    }
}

/**
 * Modified version of hoare's algorithm intended to be resistant to redundant elements along the
 * partition. This scheme is also known as three-way partitioning. Make sure to select the forcing pivot
 * scheme that matches your problem accordingly
 * @param pivot_value the pivot value to use
 * @param lhs the left boundary for partition algorithm (inclusive)
 * @param rhs the right boundary for partition algorithm (inclusive)
 * @return std::size_t the index on which the partition value belongs
 */
template<class T>
inline std::size_t BareBoneIQS<T>::partition_redundant(T pivot_value, std::size_t lhs, std::size_t rhs) {
    std::size_t i = lhs - 1;
    std::size_t k = rhs + 1;
    while (1) {
        while (this->target_ptr[++i] < pivot_value);
        while (this->target_ptr[--k] > pivot_value);
        if (i >= k) break;
        this->swap(i, k);
    }
    i = k++;
    while(i > lhs && this->target_ptr[i] == pivot_value) i--;
    while(k < rhs && this->target_ptr[k] == pivot_value) k++;

    #ifdef FORCE_PIVOT_SELECTION_LEFT
        return i; // return left pivot
    #elif FORCE_PIVOT_SELECTION_RIGHT
        return k; // return left pivot
    #else
        return (i + k) / 2; // if there is a group, then return the middle element to guarantee a position
    #endif
}

/**
 * @brief Pops the last element on the stack
 * @return std::size_t element at the top of the stack
 */
template<class T>
inline std::size_t BareBoneIQS<T>::stack_pop(){
    return this->stack[--this->stack_length];
}

/**
 * @brief Peeks the last element on the stack
 * @return std::size_t element at the top of the stack
 */
template<class T>
inline std::size_t BareBoneIQS<T>::stack_peek(){
    return this->stack[this->stack_length-1];
}

/**
 * @brief Inserts an element on the top of the stack
 * @param value the element to insert
 */
template<class T>
inline void BareBoneIQS<T>::stack_push(std::size_t value){
    this->stack[this->stack_length] = value;
    this->stack_length++;
}

/**
 * @brief Retrieves the next sorted element. The basic idea is to
 * use quick select to find the smallest element, but store the pivots along the
 * way in order to shorten future calculations.
 *
 * @return T the next sorted element
 */
template<class T>
T BareBoneIQS<T>::next() {
    // This for allows the tail recursion
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resize the search window
        if (this->extracted_count == this->stack_peek()){
            this->extracted_count++;
            return this->target_ptr[this->stack_pop()];
        }

        // Selects a random pivot from the remaining array

        #ifdef FIXED_PIVOT_SELECTION
            std::size_t pivot_idx = this->extracted_count;
        #else
            std::size_t rand_range = this->stack_peek() - this->extracted_count;
            std::size_t pivot_idx = this->extracted_count + (rand() % rand_range);
        #endif
        T pivot_value = this->target_ptr[pivot_idx];

        // pivot partition and indexing
        #ifdef USE_FAT_PARTITION
                pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, this->stack_peek());
        #else
                pivot_idx = this->partition(pivot_value, this->extracted_count, this->stack_peek());
        #endif

        // Push and recurse the loop
        this->stack_push(pivot_idx);
    }
}
