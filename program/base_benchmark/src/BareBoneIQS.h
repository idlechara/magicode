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

#ifndef IQS_TEST_BAREBONEIQS_H
#define IQS_TEST_BAREBONEIQS_H
#include "cstdlib"
#include "../flags.h"

template<class T>
class BareBoneIQS {
public:
    BareBoneIQS(T *target_ptr, size_t target_size);
    ~BareBoneIQS();
    inline void swap(size_t lhs, size_t rhs);
    inline size_t partition(T pivot_value, size_t lhs, size_t rhs);
    size_t partition_redundant(T pivot_value, size_t lhs, size_t rhs);
    inline size_t stack_pop();
    inline size_t stack_peek();
    inline void stack_push(size_t value);
    virtual T next();

protected:
    /**
     * In this example we used a stack which is the same length of the array. This is only for
     * testing purposes and can be changed into a proper stack later on if desired
     *
     */
    size_t *stack;
    size_t stack_length;

    size_t target_size;
    size_t extracted_count;
    T *target_ptr;

};


#endif //UNTITLED1_BAREBONEIQS_H
