#include <cstdio>
#include <ctime>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iterator>
#include <array>
#include <iostream>

#include "flags.h"

#include "src/IQS.cpp"
#include "src/BareBoneIQS.cpp"
#include "src/IIQS.cpp"
#include "src/BareBoneIIQS.cpp"



#define TRACK_TIME_START {                                              \
    clock_t start, end;                                                 \
    double cpu_time_used;                                               \
    start = clock();

#define TRACK_TIME_END(EXPERIMENT_NAME)                                 \
    end = clock();                                                      \
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;          \
    printf("Time %s: %lf\n", EXPERIMENT_NAME, cpu_time_used);           \
}

#define CHECK_AGAINST_BASELINE(BASELINE_VARIABLE, VARIABLE_TO_CHECK)                                                \
    for(size_t i=0; i<ELEMENTS_TO_EXTRACT; i++) {                                                                   \
        if(BASELINE_VARIABLE[i] != VARIABLE_TO_CHECK[i]){                                                           \
            printf("ERROR ON %s, index %ld \n", #VARIABLE_TO_CHECK, i);                                             \
            std::cout << "Expected: " << BASELINE_VARIABLE[i] << ", but got: " << VARIABLE_TO_CHECK[i] << std::endl;\
            break;                                                                                                  \
        }                                                                                                           \
    }

void experiment_1()
{
    const size_t test_size = TEST_SIZE;

    TYPE_TO_USE *a = new TYPE_TO_USE[test_size];
    std::array<TYPE_TO_USE, test_size> b;
    std::array<TYPE_TO_USE, test_size> c;
    std::array<TYPE_TO_USE, test_size> d;
    std::array<TYPE_TO_USE, test_size> f;
    std::array<TYPE_TO_USE, test_size> result_temp;
    TYPE_TO_USE *g = new TYPE_TO_USE[test_size];


    for(size_t i=0; i<test_size; i++) {
        TYPE_TO_USE val = rand() % (test_size + 1);
        a[i] = val;
        b[i] = val;
        c[i] = val;
        d[i] = val;
        f[i] = val;
        g[i] = val;
        result_temp[i] = val;
    }

    TRACK_TIME_START
        BareBoneIQS<TYPE_TO_USE> biqs(a, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++) biqs.next();
    TRACK_TIME_END("Bare-bones C IQS")


    TRACK_TIME_START
        IQS<std::array<TYPE_TO_USE, test_size>, TYPE_TO_USE> iqs(b);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iqs.next();
    TRACK_TIME_END("C++ portable IQS")

    TRACK_TIME_START
        std::partial_sort(c.begin(), c.begin() + ELEMENTS_TO_EXTRACT, c.end());
    TRACK_TIME_END("std::partial_sort complete")

    TRACK_TIME_START
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++) {
            std::partial_sort(d.begin() + i, d.begin() + i, d.end());
        };
    TRACK_TIME_END("std::partial_sort incremental")

    TRACK_TIME_START
        IIQS<std::array<TYPE_TO_USE, test_size>, TYPE_TO_USE> iiqs(f);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iiqs.next();
    TRACK_TIME_END("C++ portable IIQS")

    TRACK_TIME_START
        BareBoneIIQS<TYPE_TO_USE> biiqs(g, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++) biiqs.next();
    TRACK_TIME_END("Bare-bones C IIQS");


    std::sort(result_temp.begin(), result_temp.end());
    CHECK_AGAINST_BASELINE(result_temp, a);
    CHECK_AGAINST_BASELINE(result_temp, b);
    CHECK_AGAINST_BASELINE(result_temp, c);
    CHECK_AGAINST_BASELINE(result_temp, d);
    CHECK_AGAINST_BASELINE(result_temp, f);
    CHECK_AGAINST_BASELINE(result_temp, g);


}


void experiment_2()
{
    const size_t test_size = TEST_SIZE;

    TYPE_TO_USE *a = new TYPE_TO_USE[test_size];
    TYPE_TO_USE *g = new TYPE_TO_USE[test_size];

    std::vector<TYPE_TO_USE> b;
    std::vector<TYPE_TO_USE> c;
    std::vector<TYPE_TO_USE> d;
    std::vector<TYPE_TO_USE> e;
    std::vector<TYPE_TO_USE> f;
    std::vector<TYPE_TO_USE> result_temp;

    for(size_t i=0; i<test_size; i++) {
        TYPE_TO_USE val = rand() % (test_size + 1);
        a[i] = val;
        b.push_back(val);
        c.push_back(val);
        d.push_back(val);
        e.push_back(val);
        f.push_back(val);
        result_temp.push_back(val);
        g[i] = val;
    }


    TRACK_TIME_START
        BareBoneIQS<TYPE_TO_USE> biqs(a, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)biqs.next();
    TRACK_TIME_END("Bare-bones C IQS")

    TRACK_TIME_START
        IQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iqs(b);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iqs.next();
    TRACK_TIME_END("C++ portable IQS")

    TRACK_TIME_START
        std::partial_sort(c.begin(), c.begin() + ELEMENTS_TO_EXTRACT, c.end());
    TRACK_TIME_END("std::partial_sort complete")

    TRACK_TIME_START
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++) std::partial_sort(d.begin() + i, d.begin() + i, d.end());
    TRACK_TIME_END("std::partial_sort incremental")

    TRACK_TIME_START
        std::make_heap(e.begin(), e.end());
        for(int i = 0; i < ELEMENTS_TO_EXTRACT - 1; i++) {
            TYPE_TO_USE temp = e.front();
            std::pop_heap(e.begin(), e.end());
            e.pop_back();
        }
    TRACK_TIME_END("std::heap incremental")

    TRACK_TIME_START
        IIQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iiqs(f);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iiqs.next();
    TRACK_TIME_END("C++ portable IIQS")


    TRACK_TIME_START
        BareBoneIIQS<TYPE_TO_USE> biiqs(g, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)biiqs.next();
    TRACK_TIME_END("Bare-bones C IIQS");



    std::sort(result_temp.begin(), result_temp.end());
    CHECK_AGAINST_BASELINE(result_temp, a);
    CHECK_AGAINST_BASELINE(result_temp, b);
    CHECK_AGAINST_BASELINE(result_temp, c);
    CHECK_AGAINST_BASELINE(result_temp, d);
    CHECK_AGAINST_BASELINE(result_temp, f);
    CHECK_AGAINST_BASELINE(result_temp, g);

}

// ditched vector as vector takes too much resources
void experiment_3()
{
    const size_t test_size = TEST_SIZE;

    TYPE_TO_USE *a = new TYPE_TO_USE[test_size];
    TYPE_TO_USE *g = new TYPE_TO_USE[test_size];

    std::vector<TYPE_TO_USE> b;
    std::vector<TYPE_TO_USE> c;
    std::vector<TYPE_TO_USE> d;
    std::vector<TYPE_TO_USE> e;
    std::vector<TYPE_TO_USE> f;
    std::vector<TYPE_TO_USE> result_temp;

    for(size_t i=0; i<test_size; i++) {
        TYPE_TO_USE val = i;
        a[i] = val;
        b.push_back(val);
        c.push_back(val);
        d.push_back(val);
        e.push_back(val);
        f.push_back(val);
        result_temp.push_back(val);
        g[i] = val;
    }




    TRACK_TIME_START
        BareBoneIQS<TYPE_TO_USE> biqs(a, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)biqs.next();
    TRACK_TIME_END("Bare-bones C IQS")

    TRACK_TIME_START
        IQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iqs(b);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iqs.next();
    TRACK_TIME_END("C++ portable IQS")

    TRACK_TIME_START
        std::partial_sort(c.begin(), c.begin() + ELEMENTS_TO_EXTRACT, c.end());
    TRACK_TIME_END("std::partial_sort complete")

    TRACK_TIME_START
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++) std::partial_sort(d.begin() + i, d.begin() + i, d.end());
    TRACK_TIME_END("std::partial_sort incremental")

    TRACK_TIME_START
        std::make_heap(e.begin(), e.end());
        for(int i = 0; i < ELEMENTS_TO_EXTRACT - 1; i++) {
            TYPE_TO_USE temp = e.front();
            std::pop_heap(e.begin(), e.end());
            e.pop_back();
        }
    TRACK_TIME_END("std::heap incremental")

    TRACK_TIME_START
        IIQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iiqs(f);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iiqs.next();
    TRACK_TIME_END("C++ portable IIQS")


    TRACK_TIME_START
        BareBoneIIQS<TYPE_TO_USE> biiqs(g, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)biiqs.next();
    TRACK_TIME_END("Bare-bones C IIQS");



    std::sort(result_temp.begin(), result_temp.end());
    CHECK_AGAINST_BASELINE(result_temp, a);
    CHECK_AGAINST_BASELINE(result_temp, b);
    CHECK_AGAINST_BASELINE(result_temp, c);
    CHECK_AGAINST_BASELINE(result_temp, d);
    CHECK_AGAINST_BASELINE(result_temp, f);
    CHECK_AGAINST_BASELINE(result_temp, g);

}


void experiment_4()
{
    const size_t test_size = TEST_SIZE;

    TYPE_TO_USE *a = new TYPE_TO_USE[test_size];
    TYPE_TO_USE *g = new TYPE_TO_USE[test_size];

    std::vector<TYPE_TO_USE> b;
    std::vector<TYPE_TO_USE> c;
    std::vector<TYPE_TO_USE> d;
    std::vector<TYPE_TO_USE> e;
    std::vector<TYPE_TO_USE> f;
    std::vector<TYPE_TO_USE> result_temp;

    for(size_t i=0; i<test_size; i++) {
        TYPE_TO_USE val = TEST_SIZE - i;
        a[i] = val;
        b.push_back(val);
        c.push_back(val);
        d.push_back(val);
        e.push_back(val);
        f.push_back(val);
        result_temp.push_back(val);
        g[i] = val;
    }



    TRACK_TIME_START
        BareBoneIQS<TYPE_TO_USE> biqs(a, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)biqs.next();
    TRACK_TIME_END("Bare-bones C IQS")

    TRACK_TIME_START
        IQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iqs(b);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iqs.next();
    TRACK_TIME_END("C++ portable IQS")

    TRACK_TIME_START
        std::partial_sort(c.begin(), c.begin() + ELEMENTS_TO_EXTRACT, c.end());
    TRACK_TIME_END("std::partial_sort complete")

    TRACK_TIME_START
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++) std::partial_sort(d.begin() + i, d.begin() + i, d.end());
    TRACK_TIME_END("std::partial_sort incremental")

    TRACK_TIME_START
        std::make_heap(e.begin(), e.end());
        for(int i = 0; i < ELEMENTS_TO_EXTRACT - 1; i++) {
            TYPE_TO_USE temp = e.front();
            std::pop_heap(e.begin(), e.end());
            e.pop_back();
        }
    TRACK_TIME_END("std::heap incremental")

    TRACK_TIME_START
        IIQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iiqs(f);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)iiqs.next();
    TRACK_TIME_END("C++ portable IIQS")


    TRACK_TIME_START
        BareBoneIIQS<TYPE_TO_USE> biiqs(g, test_size);
        for(int i = 0; i < ELEMENTS_TO_EXTRACT; i++)biiqs.next();
    TRACK_TIME_END("Bare-bones C IIQS");


    std::sort(result_temp.begin(), result_temp.end());
    CHECK_AGAINST_BASELINE(result_temp, a);
    CHECK_AGAINST_BASELINE(result_temp, b);
    CHECK_AGAINST_BASELINE(result_temp, c);
    CHECK_AGAINST_BASELINE(result_temp, d);
    CHECK_AGAINST_BASELINE(result_temp, f);
    CHECK_AGAINST_BASELINE(result_temp, g);

}


int main(int argc, char const *argv[])
{

    srand(time(0));
    printf("----EXPERIMENT 1 - Random with std::array----\n");
    experiment_1();

    printf("\n\n----EXPERIMENT 2 - Random with std::vector ----\n");
    experiment_2();

    printf("\n\n----EXPERIMENT 3 - Ordered asc with std::vector ----\n");
    experiment_3();

    printf("\n\n----EXPERIMENT 4 - Ordered desc with std::vector----\n");
    experiment_4();

}