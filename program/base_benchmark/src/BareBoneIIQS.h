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

#ifndef IQS_TEST_BAREBONEIIQS_H
#define IQS_TEST_BAREBONEIIQS_H
#include "cstdlib"
#include "../flags.h"
#include "./BareBoneIQS.h"

template <class T>
class BareBoneIIQS : public BareBoneIQS<T> {
public:
    BareBoneIIQS(T *target_ptr, std::size_t target_size);
    ~BareBoneIIQS();
    T next();
    T bfprt(std::size_t lhs, std::size_t rhs, std::size_t median_length);
    std::size_t median(std::size_t lhs, std::size_t rhs);
};


#endif //IQS_TEST_BAREBONEIIQS_H
