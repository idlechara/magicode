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
#include "rIIQS.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include "./snapshot.h"
#include <cmath>
#include <vector>


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
inline rIIQSStackContainer_t rIIQS<Container, Type>::ranged_partition(Type pivot_value, std::size_t lhs, std::size_t rhs, bool alternate_implementation) {
    std::size_t i = lhs;
    std::size_t j = lhs;
    std::size_t k = rhs+1;

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



    /** Adaptation for redundant **/
    rIIQSStackContainer_t element;
    element.start = i;
    element.end = j-1;
    return element;
}


/**
 * Returns the next sorted element
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @return the next sorted element
 */
template<class Container, class Type>
Type rIIQS<Container, Type>::next() {
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

        rIIQSStackContainer_t top_element = this->rStack.top();
        //std::size_t range = top_element - this->extracted_count;
        

        if (this->extracted_count == top_element.start){
            this->extracted_count++;


            this->rStack.pop();
            /** Custom implementation of pop operation **/
            if(top_element.start < top_element.end){
                top_element.start = top_element.start + 1;
                this->rStack.push(top_element);
            }
            /** end of custom implementation **/

            LOCAL_CLOCK_END(this->configuration.log_iteration_time, iteration_time_clock, ITERATION_STAGE_END,
            this->snapshot, this->snapshots,
            {
                this->snapshot.current_iteration_pulled_pivots += 1;
                this->snapshot.total_pulled_pivots += 1;
            })

            return this->container[top_element.start];
        }

        rIIQSStackContainer_t pivot_idx;

        if (this->configuration.use_random_pivot)
            pivot_idx.start = this->random_between(this->extracted_count, top_element.start-1);
        else
            pivot_idx.start = this->biased_between(this->extracted_count, top_element.start-1, this->configuration.pivot_bias);
        Type pivot_value = this->container[pivot_idx.start];

        CLOCK_ROUTINE(
            this->configuration.log_pivot_time,
            {
                pivot_idx = this->ranged_partition(pivot_value, this->extracted_count, top_element.start-1, this->configuration.use_bfprt);
            },
            PARTITION_STAGE_END,
            this->snapshot, this->snapshots,
            partition_time, total_partition_time,
            {
                this->snapshot.current_extracted_pivot = pivot_idx.start / this->container.size();
                this->snapshot.total_executed_partitions += 1;
                this->snapshot.current_extraction_executed_partitions += 1;
                this->snapshot.current_stack_size += this->rStack.size();
            }
        )
        
        std::size_t previous_pivot_idx = pivot_idx.start;

        // IIQS changes start! only check if range is less than the square root of the total size
        // First, we need to check if this pointer belongs P70 \union P30
        std::size_t p70_idx = this->biased_between(this->extracted_count, top_element.start-1, this->configuration.beta_value); //this->extracted_count + (std::size_t)std::ceil(range * this->configuration.beta_value);
        std::size_t p30_idx = this->biased_between(this->extracted_count, top_element.start-1, this->configuration.alpha_value); //this->extracted_count + (std::size_t)std::floor(range * this->configuration.alpha_value); // actually, if we don't care about balancing the stack, you can ignore the p30 condition

        //apply introspection rule, now it evaluates if the segment deviates
        if ( (this->extracted_count < pivot_idx.start) && (p70_idx - p30_idx > 3) && (pivot_idx.end < p30_idx || pivot_idx.start > p70_idx)){


            CLOCK_ROUTINE(
                this->configuration.log_pivot_time,
                    // if we enter here, then it's because the index needs to be recomputed.
                    // So, we ditch the index and get a nice approximate median median and reuse previous computation
                    std::size_t bfprt_pivot = this->bfprt(this->container, this->extracted_count, top_element.start-1, 5);
                    pivot_value = this->container[bfprt_pivot];
                    // then we re-partition, assuming that this median is better
                    pivot_idx = this->ranged_partition(pivot_value, this->extracted_count, top_element.start-1, this->configuration.use_bfprt);
                ,
                ITERATION_STAGE_INTROSPECT,
                this->snapshot, this->snapshots,
                partition_time, total_partition_time,
                
                    this->snapshot.current_extracted_pivot = pivot_idx.start / this->container.size();
                    this->snapshot.total_executed_bfprt_partitions += 1;
                    this->snapshot.current_iteration_executed_bfprt_partitions += 1;
                    this->snapshot.current_stack_size += this->rStack.size();
                
            )


        }
        // Push and recurse the loop
        this->rStack.push(pivot_idx);

        LOCAL_CLOCK_END(this->configuration.log_iteration_time, iteration_time_clock, ITERATION_STAGE_LOOP,
        this->snapshot, this->snapshots,
        {
            this->snapshot.current_iteration_pushed_pivots += 1;
            this->snapshot.total_pushed_pivots += 1;
        })
    }
}


/**
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 * @param container reference to the container element to apply IQS on
 */
template<class Container, class Type>
rIIQS<Container, Type>::rIIQS(Container &container, configuration_t &configuration, std::vector<snapshot_t> &snapshots, snapshot_t &snapshot): 
    IIQS<Container, Type>(container, configuration, snapshots, snapshot) {
    this->extracted_count = 0;
    /* At this point we ignore the original stack and replace it with a newer one */
    this->rStack = std::stack<rIIQSStackContainer_t>();
    
    rIIQSStackContainer_t element;
    element.start = container.size()-1;
    element.end = container.size()-1;

    this->rStack.push(element);

    std::cout << "Initializing rIIQS \n";
}

/**
 * @tparam Container container type to handle on the class
 * @tparam Type type used for comparison
 */
template<class Container, class Type>
rIIQS<Container, Type>::rIIQS(): 
    IQS<Container, Type>() {
}
