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
#include <iostream>
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
 *  Function to play with IQS implementation of random, gives a biased element
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param lhs left index, inclusive
 * @param rhs left index, inclusive
 * @param bias bias to put on the pivot
 * @return a random number [lhs, rhs]
 */
template<class Container, class Type>
inline std::size_t IQS<Container, Type>::biased_between(std::size_t lhs, std::size_t rhs, double bias) {
    double range = (double)(rhs - lhs);
    std::size_t index_bias = (int)(bias * range);
    return lhs + index_bias;
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
inline std::size_t IQS<Container, Type>::partition_redundant(Type pivot_value, std::size_t lhs, std::size_t rhs, bool alternate_implementation) {
    std::size_t i = lhs;
    std::size_t j = lhs;
    std::size_t k = rhs;

    while (j < k) {
        if(this->container[j] < pivot_value){
            // Alternate implementation set as false as there are no two alternate implementations available for testing
            this->swap(this->container, i++,j++, alternate_implementation);
        }
        else if (this->container[j] > pivot_value){
            this->swap(this->container, j,--k, alternate_implementation);
        }
        else {
            j++;
        }
    }
    
    return this->biased_between(i, k, configuration.redundant_bias);
    // #ifdef FORCE_PIVOT_SELECTION_LEFT
    //     return i; // return left pivot
    // #elif FORCE_PIVOT_SELECTION_RIGHT
    //     return k; // return left pivot
    // #else
    //         return (i + k) / 2; // if there is a group, then return the middle element to guarantee a position
    // #endif
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
inline void IQS<Container, Type>::swap(Container &container, std::size_t lhs, std::size_t rhs, bool alternate_implementation) {
    OPTIONAL_REGISTRY(
        this->configuration.log_swaps, 
        {
            std::swap(container[lhs], container[rhs]);
        },
        {
            if(alternate_implementation) {
                this->snapshot.current_iteration_bfprt_partition_swaps += 1;
                this->snapshot.total_executed_bfprt_partition_swaps += 1;
                size_t swap_lenght = rhs - lhs;
                if ( swap_lenght > this->snapshot.current_iteration_longest_bfprt_partition_swap ) {
                    this->snapshot.current_iteration_longest_bfprt_partition_swap = swap_lenght;
                }
            }
            else {
                this->snapshot.current_iteration_partition_swaps += 1;
                this->snapshot.total_executed_partition_swaps += 1;
                size_t swap_lenght = rhs - lhs;
                if ( swap_lenght > this->snapshot.current_iteration_longest_partition_swap ) {
                    this->snapshot.current_iteration_longest_partition_swap = swap_lenght;
                }
            }
        }
    );
}

/**
 * Returns the next sorted element
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @return the next sorted element
 */
template<class Container, class Type>
Type IQS<Container, Type>::next() {
    this->snapshot.current_extraction_executed_partitions = 0;
    while(1){
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resize the search window

        // reset counters
        this->snapshot.current_iteration_partition_swaps = 0;
        this->snapshot.current_iteration_longest_partition_swap = 0;
        this->snapshot.current_iteration_executed_bfprt_partitions = 0;
        this->snapshot.current_iteration_bfprt_partition_swaps = 0;
        this->snapshot.current_iteration_longest_bfprt_partition_swap = 0;
        this->snapshot.current_iteration_pushed_pivots = 0;
        this->snapshot.current_iteration_pulled_pivots = 0;

        LOCAL_CLOCK_START(this->configuration.log_iteration_time, iteration_time_clock, ITERATION_STAGE_BEGIN)

        std::size_t top_element = this->stack.top();
        if (this->extracted_count == this->stack.top() ){
            this->extracted_count++;
            this->stack.pop();

            LOCAL_CLOCK_END(this->configuration.log_iteration_time, iteration_time_clock, ITERATION_STAGE_END,
            this->snapshot, this->snapshots,
            {
                this->snapshot.current_iteration_pulled_pivots += 1;
                this->snapshot.total_pulled_pivots += 1;
            })
            return this->container[top_element];
        }

        std::size_t pivot_idx;
        if (this->configuration.use_random_pivot)
            pivot_idx = this->random_between(this->extracted_count, top_element-1);
        else
            pivot_idx = this->biased_between(this->extracted_count, top_element-1, this->configuration.pivot_bias);


        Type pivot_value = this->container[pivot_idx];

        // pivot partition and indexing
//        pivot_idx = this->partition(pivot_value, this->extracted_count, top_element);
        // We displace the selection for the top element in order to prevent hanging
        // you can't just use top_element -1 at the first iteration, as you still need to sort the last element. duh!
        // pivot partition and indexing

        // As background we'll be using the three-way implementation on those tests in order to level-ground all implementations

        CLOCK_ROUTINE(
            this->configuration.log_pivot_time,
            {pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, top_element-1, this->configuration.use_bfprt);},
            PARTITION_STAGE_END,
            this->snapshot, this->snapshots,
            partition_time, total_partition_time,
            {
                this->snapshot.current_extracted_pivot = pivot_idx / this->container.size();
                this->snapshot.total_executed_partitions += 1;
                this->snapshot.current_extraction_executed_partitions += 1;
                this->snapshot.current_stack_size += this->stack.size();
            }
        )
        // Push and recurse the loop
        this->stack.push(pivot_idx);

        LOCAL_CLOCK_END(this->configuration.log_iteration_time, iteration_time_clock, ITERATION_STAGE_LOOP,
        this->snapshot, this->snapshots,
        {
            this->snapshot.current_iteration_pushed_pivots += 1;
            this->snapshot.total_pushed_pivots += 1;
        })
    }
}

template<class Container, class Type>
IQS<Container, Type>::IQS(Container &container, configuration_t &configuration, std::vector<snapshot_t> &snapshots, snapshot_t &snapshot): 
container(container), configuration(configuration), snapshot(snapshot), snapshots(snapshots) {
    this->extracted_count = 0;
    this->stack = std::stack<std::size_t>();
    this->stack.push(container.size()-1);
    std::cout << "Initializing IQS \n";
}

template<class Container, class Type>
IQS<Container, Type>::IQS() {
}
