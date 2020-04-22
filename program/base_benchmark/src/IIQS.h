//
// Created by kuky_nekoi on 20-04-20.
//

#ifndef IQS_TEST_IIQS_H
#define IQS_TEST_IIQS_H

#include "../flags.h"

#include "IQS.h"

template<class Container, class Type>
class IIQS : public IQS<Container, Type>{
public:
    IIQS(Container &container);
    Type next();
    inline Type bfprt(Container &container, size_t lhs, size_t rhs, size_t median_length);
    inline size_t median(Container &container, size_t lhs, size_t rhs);
private:
    size_t max_stack_size;
};


#endif //IQS_TEST_IIQS_H
