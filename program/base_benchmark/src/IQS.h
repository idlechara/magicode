//
// Created by kuky_nekoi on 19-04-20.
//

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
