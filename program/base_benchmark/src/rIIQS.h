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

#ifndef IQS_TEST_rIIQS_H
#define IQS_TEST_rIIQS_H

#include "../flags.h"
#include "stdlib.h"

#include "./IIQS.h"


typedef struct {
    std::size_t start;
    std::size_t end;
} rIIQSStackContainer_t;


template<class Container, class Type>
class rIIQS : public IIQS<Container, Type>{
public:
    rIIQS();
    rIIQS(Container &container, configuration_t &configuration, std::vector<snapshot_t> &snapshots, snapshot_t &snapshot);
    rIIQSStackContainer_t ranged_partition(Type pivot_value, std::size_t lhs, std::size_t rhs, bool alternate_implementation);
    Type next() override;

    std::stack<rIIQSStackContainer_t> rStack;
};


#endif //IQS_TEST_rIIQS_H
