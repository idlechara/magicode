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

#ifndef IQS_TEST_FLAGS_H
#define IQS_TEST_FLAGS_H

#define FIXED_PIVOT_SELECTION 1 // remove to allow random initial pivot selection
#define TEST_SIZE 20000         // Size of the array to test
#define ELEMENTS_TO_EXTRACT 20000  // how many sorted elements you want
#define TYPE_TO_USE long        // what you're comparing?
#define USE_FAT_PARTITION 1    // use three-way-partitioning
#define USE_ALPHA_LESS_THAN_P30 1    // use three-way-partitioning
#define TIME_UNIT nanoseconds
//#define INCLUDE_TESTING_WITH_FIXED_ARRAY 1    // use three-way-partitioning

/* use those to test where you want to bias the pivot selection. If none selected, then chooses the middle of the group */
//#define FORCE_PIVOT_SELECTION_LEFT 1
//#define FORCE_PIVOT_SELECTION_RIGHT 1

//#define REUSE_PIVOTS 1


#define EXECUTION_MODE_IQS 0
#define EXECUTION_MODE_IIQS 1
#define EXECUTION_MODE_RIIQS 2

#define SWAP_BETWEEN(flag, param1, param2, operation) \
if ( flag ) { \
    param1 operation;\
}\
else {\
    param2 operation;\
}\


#define SWITCH_BETWEEN(flag, param1, param2, param3, operation) \
switch (flag){ \
case EXECUTION_MODE_IQS: \
    param1 operation; \
    break; \
case EXECUTION_MODE_IIQS: \
    param2 operation; \
    break; \
case EXECUTION_MODE_RIIQS: \
    param3 operation; \
    break; \
default: \
    break; \
}

#endif //IQS_TEST_FLAGS_H
