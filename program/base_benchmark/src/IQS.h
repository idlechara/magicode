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
    IQS(Container &container);
    inline void swap(Container &container, size_t idx_1, size_t idx_2);
    inline size_t partition(Type pivot_value, size_t lhs, size_t rhs);
    Type next();
    inline size_t random_between(size_t lhs, size_t rhs);

protected:
    Container &container;
    std::stack<size_t> stack;
    size_t extracted_count;
};

#endif //UNTITLED1_IQS_H
