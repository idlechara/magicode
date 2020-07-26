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

#ifndef IncrementalSorting_TEST_IncrementalSorting_H
#define IncrementalSorting_TEST_IncrementalSorting_H

#include "../flags.h"

#include <stack>
#include "snapshot.h"
#include <vector>

template<class Container, class Type>
class IncrementalSorting {
public:
    virtual inline void swap(Container &container, std::size_t lhs, std::size_t rhs, bool alternate_implementation);
    virtual inline std::size_t partition(Type pivot_value, std::size_t lhs, std::size_t rhs);
    virtual Type next();
    virtual inline std::size_t random_between(std::size_t lhs, std::size_t rhs);
    virtual inline std::size_t biased_between(std::size_t lhs, std::size_t rhs, double bias);
    virtual std::size_t partition_redundant(Type pivot_value, std::size_t lhs, std::size_t rhs, bool alternate_implementation);
    virtual std::stack<std::size_t> stack;
};

#endif //UNTITLED1_IQS_H
