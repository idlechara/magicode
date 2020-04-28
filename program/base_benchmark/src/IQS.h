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

#ifndef IQS_TEST_IQS_H
#define IQS_TEST_IQS_H

#include "../flags.h"

#include <stack>

template<class Container, class Type>
class IQS {
public:
    explicit IQS(Container &container);
    inline void swap(Container &container, std::size_t lhs, std::size_t rhs);
    inline std::size_t partition(Type pivot_value, std::size_t lhs, std::size_t rhs);
    virtual Type next();
    inline std::size_t random_between(std::size_t lhs, std::size_t rhs);
    std::size_t partition_redundant(Type pivot_value, std::size_t lhs, std::size_t rhs);

protected:
    Container &container;
    std::stack<std::size_t> stack;
    std::size_t extracted_count;
};

#endif //UNTITLED1_IQS_H
