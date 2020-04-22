//
// Created by kuky_nekoi on 19-04-20.
//

#include "IQS.h"
#include <cstdlib>

//heaps work well when you know how many elements you're trying to sort
// but it pitfalls when ordering the whole array is also a posibility

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
        pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);

        // Push and recurse the loop
        this->stack.push(pivot_idx);
    }
}

template<class Container, class Type>
IQS<Container, Type>::IQS(Container &container): container(container) {
    this->extracted_count = 0;
    this->stack = std::stack<size_t>();
    this->stack.push(container.size()-1);
}
