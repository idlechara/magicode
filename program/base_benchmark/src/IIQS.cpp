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
#include <cstdio>
#include <cmath>

/**
 * Returns the next sorted element
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @return the next sorted element
 */
template<class Container, class Type>
Type IIQS<Container, Type>::next() {
    this->snapshot.current_extraction_executed_partitions = 0;
    this->snapshot.current_iteration_executed_bfprt_partitions = 0;
 
    while(1){
        //reset counters
        this->snapshot.current_iteration_partition_swaps = 0;
        this->snapshot.current_iteration_longest_partition_swap = 0;
        this->snapshot.current_iteration_executed_bfprt_partitions = 0;
        this->snapshot.current_iteration_bfprt_partition_swaps = 0;
        this->snapshot.current_iteration_longest_bfprt_partition_swap = 0;
        this->snapshot.current_iteration_pushed_pivots = 0;
        this->snapshot.current_iteration_pulled_pivots = 0;
        
        // Base condition. If the element referenced by the top of the stack
        // is the element that we're actually searching, then retrieve it and
        // resize the search window
        LOCAL_CLOCK_START(this->configuration.log_iteration_time, iteration_time_clock, ITERATION_STAGE_BEGIN)

        std::size_t top_element = this->stack.top();
        //std::size_t range = top_element - this->extracted_count;
        

        if (this->extracted_count == top_element){
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

        CLOCK_ROUTINE(
            this->configuration.log_pivot_time,
            {
                if(this->configuration.use_dutch_flag){
                    pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, top_element-1, this->configuration.use_bfprt);
                }
                else{
                    pivot_idx = this->partition(pivot_value, this->extracted_count, top_element-1, this->configuration.use_bfprt);
                }
            },
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
        
        std::size_t previous_pivot_idx = pivot_idx;

        // IIQS changes start! only check if range is less than the square root of the total size
        // First, we need to check if this pointer belongs P70 \union P30
        std::size_t p70_idx = this->biased_between(this->extracted_count, top_element-1, this->configuration.beta_value); //this->extracted_count + (std::size_t)std::ceil(range * this->configuration.beta_value);
        std::size_t p30_idx = this->biased_between(this->extracted_count, top_element-1, this->configuration.alpha_value); //this->extracted_count + (std::size_t)std::floor(range * this->configuration.alpha_value); // actually, if we don't care about balancing the stack, you can ignore the p30 condition

        //apply introspection rule
        if ( (p70_idx - p30_idx > 3) && (pivot_idx < p30_idx || pivot_idx > p70_idx)){


            CLOCK_ROUTINE(
                this->configuration.log_pivot_time,
                
                    // if we enter here, then it's because the index needs to be recomputed.
                    // So, we ditch the index and get a nice approximate median median and reuse previous computation
                    pivot_idx = this->bfprt(this->container, this->extracted_count, top_element-1, 5);
                    pivot_value = this->container[pivot_idx];
                    // then we re-partition, assuming that this median is better

                    if(this->configuration.use_dutch_flag){
                        pivot_idx = this->partition_redundant(pivot_value, this->extracted_count, top_element-1, this->configuration.use_bfprt);
                    }
                    else{
                        pivot_idx = this->partition(pivot_value, this->extracted_count, top_element-1, this->configuration.use_bfprt);
                    }
                ,
                ITERATION_STAGE_INTROSPECT,
                this->snapshot, this->snapshots,
                partition_time, total_partition_time,
                
                    this->snapshot.current_extracted_pivot = pivot_idx / this->container.size();
                    this->snapshot.total_executed_bfprt_partitions += 1;
                    this->snapshot.current_iteration_executed_bfprt_partitions += 1;
                    this->snapshot.current_stack_size += this->stack.size();
                
            )


        }


        // I need to see later how it does affect the stack this segment.
        if(this->configuration.enable_reuse){
            if(previous_pivot_idx < pivot_idx){
                this->stack.push(pivot_idx);
                this->stack.push(previous_pivot_idx);
                continue;
            }
            else if(previous_pivot_idx > pivot_idx){
                this->stack.push(previous_pivot_idx);
                this->stack.push(pivot_idx);
                continue;
            }
        }
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

/**
 * In-place implementation of bfprt. Instead of the classical implementation when auxiliary structures are used
 * this implementation forces two phenomena on the array which both are beneficial to IQS. First, given that we force
 * the selection of the first index, elements near the beginning have a high chance of being good pivots. Second, we
 * don't use extra memory to allocate those median results.
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param container reference to the container element to calculate it's median
 * @param lhs the left index to sort (inclusive)
 * @param rhs the right index to sort (inclusive)
 * @param median_length size of the median to use on bfprt, 5 is commonly used
 * @return the median value
 */
template<class Container, class Type>
inline std::size_t IIQS<Container, Type>::bfprt(Container &container, std::size_t lhs, std::size_t rhs, std::size_t median_length) {
    std::size_t base_lhs = lhs;
    std::size_t medians_extracted = 0;

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
            std::size_t median_index = this->median(container, lhs, lhs + median_length);
            //move median to the start of the array
            this->swap(container, median_index, base_lhs + medians_extracted, true);
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
 * @return the median index
 */
template<class Container, class Type>
inline std::size_t IIQS<Container, Type>::median(Container &container, std::size_t lhs, std::size_t rhs) {
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
IIQS<Container, Type>::IIQS(Container &container, configuration_t &configuration, std::vector<snapshot_t> &snapshots, snapshot_t &snapshot): 
    IQS<Container, Type>(container, configuration, snapshots, snapshot) {
    this->extracted_count = 0;
    this->stack = std::stack<std::size_t>();
    this->stack.push(container.size()-1);
    std::cout << "Initializing IIQS \n";
}

/**
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 */
template<class Container, class Type>
IIQS<Container, Type>::IIQS(): 
    IQS<Container, Type>() {
}
