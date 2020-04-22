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
BareBoneIQS<T>::BareBoneIQS(T *target_ptr, size_t target_size){
    this->target_ptr = target_ptr;
    this->target_size = target_size;

    this->stack = (size_t *) malloc(this->target_size * sizeof(size_t)) ;
    this->stack[0] = target_size - 1; // index of the last element
    for(size_t i=1; i < target_size; i++) this->stack[i] = -1;
    this->stack_length = 1; //starts with a single element, the top

    this->extracted_count = 0; // this way, after adding +1, we can partition as whole
}
template <class T>
BareBoneIQS<T>::~BareBoneIQS() {
    free(this->stack);
}

/**
 * @brief Swaps two elements in the referenced array
 *
 * @param idx_1 index to be swapped
 * @param idx_2 index to be swapped
 */
template <class T>
inline void BareBoneIQS<T>::swap(size_t idx_1, size_t idx_2){
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
inline size_t BareBoneIQS<T>::partition(T pivot_value, size_t left_idx, size_t right_idx){
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
inline size_t BareBoneIQS<T>::stack_pop(){
    return this->stack[--this->stack_length];
}

/**
 * @brief Peeks the last element on the stack
 *
 * @return size_t element at the top of the stack
 */
template<class T>
inline size_t BareBoneIQS<T>::stack_peek(){
    return this->stack[this->stack_length-1];
}

/**
 * @brief Inserts an element on the top of the stack
 *
 * @param value the element to insert
 */
template<class T>
inline void BareBoneIQS<T>::stack_push(size_t value){
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
T BareBoneIQS<T>::next() {
    // This for allows the tail recursion
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resise the search window
        if (extracted_count == this->stack_peek()){
            extracted_count++;
            return target_ptr[this->stack_pop()];
        }

        // Selects a random pivot from the remaining array

        #ifdef FIXED_PIVOT_SELECTION
            size_t pivot_idx = this->extracted_count;
        #else
            size_t rand_range = this->stack_peek() - this->extracted_count;
            size_t pivot_idx = this->extracted_count + (rand() % rand_range);
        #endif
        T pivot_value = this->target_ptr[pivot_idx];

        // pivot partition and indexing
        pivot_idx = this->partition(pivot_value, this->extracted_count, this->stack_peek());

        // Push and recurse the loop
        this->stack_push(pivot_idx);
    }
}
