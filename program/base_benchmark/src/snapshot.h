#ifndef IQS_TEST_SNAPSHOT_T_H
#define IQS_TEST_SNAPSHOT_T_H

#include <string>
#include <chrono>
#include "./../flags.h"
#include <sstream>

#define EXTRACTION_STAGE_BEGIN         10
#define EXTRACTION_STAGE_END           20
#define ITERATION_STAGE_BEGIN       30
#define ITERATION_STAGE_LOOP        40
#define ITERATION_STAGE_INTROSPECT  41
#define ITERATION_STAGE_END         50
#define PARTITION_STAGE_BEGIN       60
#define PARTITION_STAGE_END         70

typedef struct {
    bool log_pivot_time, log_iteration_time, log_extraction_time, log_swaps, use_bfprt, use_dutch_flag, use_random_pivot, enable_reuse;
    double alpha_value, beta_value, pivot_bias, redundant_bias;
    int random_seed_value, use_iiqs;

    std::size_t input_size, extractions;
    std::string input_file_value, output_file_value;
} configuration_t;

typedef struct {
    std::chrono::TIME_UNIT iteration_time;
    std::chrono::TIME_UNIT total_iteration_time;

    std::chrono::TIME_UNIT partition_time;
    std::chrono::TIME_UNIT total_partition_time;

    std::chrono::TIME_UNIT bfprt_partition_time;
    std::chrono::TIME_UNIT total_bfprt_partition_time;

    std::chrono::TIME_UNIT extraction_time;
    std::chrono::TIME_UNIT total_extraction_time;

    size_t current_extraction_executed_partitions;
    size_t total_executed_partitions;
    size_t current_iteration_partition_swaps;
    size_t total_executed_partition_swaps;
    double current_iteration_longest_partition_swap;
    double total_executed_longest_partition_swap;

    size_t current_iteration_executed_bfprt_partitions;
    size_t total_executed_bfprt_partitions;
    size_t current_iteration_bfprt_partition_swaps;
    size_t total_executed_bfprt_partition_swaps;
    double current_iteration_longest_bfprt_partition_swap;
    double total_executed_longest_bfprt_partition_swap;

    double current_extracted_pivot;

    size_t current_stack_size;
    size_t total_pushed_pivots;
    size_t total_pulled_pivots;
    size_t current_iteration_pushed_pivots;
    size_t current_iteration_pulled_pivots;
    
    size_t current_extraction;
    size_t input_size;

    char snapshot_point;
} snapshot_t;

snapshot_t generate_snapshot(char point){
    snapshot_t snapshot;
    snapshot.iteration_time = std::chrono::TIME_UNIT(0); 
    snapshot.total_iteration_time = std::chrono::TIME_UNIT(0); 
    snapshot.partition_time = std::chrono::TIME_UNIT(0); 
    snapshot.total_partition_time = std::chrono::TIME_UNIT(0); 
    snapshot.bfprt_partition_time = std::chrono::TIME_UNIT(0); 
    snapshot.total_bfprt_partition_time = std::chrono::TIME_UNIT(0); 

    snapshot.extraction_time = std::chrono::TIME_UNIT(0); 
    snapshot.total_extraction_time = std::chrono::TIME_UNIT(0);
    snapshot.total_pushed_pivots = 0;
    snapshot.total_pulled_pivots = 0;
    snapshot.current_iteration_pushed_pivots = 0;
    snapshot.current_iteration_pulled_pivots = 0;
    snapshot.current_extraction_executed_partitions = 0;
    snapshot.current_iteration_executed_bfprt_partitions = 0;

    snapshot.current_iteration_partition_swaps = 0;
    snapshot.total_executed_partition_swaps = 0;
    snapshot.current_iteration_longest_partition_swap = 0;
    snapshot.total_executed_longest_partition_swap = 0;
    snapshot.current_iteration_bfprt_partition_swaps = 0;
    snapshot.total_executed_bfprt_partition_swaps = 0;
    snapshot.current_iteration_longest_bfprt_partition_swap = 0;
    snapshot.total_executed_longest_bfprt_partition_swap = 0;

    snapshot.total_executed_partitions = 0;
    snapshot.total_executed_bfprt_partitions = 0;
    snapshot.current_extracted_pivot = 0;   //TODO: rename to current index of extracted pivot
    snapshot.current_stack_size = 0;
    snapshot.current_extraction = 0;        // how many elements have been extracted
    snapshot.input_size = 0;
    snapshot.snapshot_point = point;

    //TODO: partition lenght, l
    return snapshot;
}

std::string point_to_string(char point){
    switch (point) {
        case EXTRACTION_STAGE_BEGIN:
            return "EXTRACTION_STAGE_BEGIN";
        case EXTRACTION_STAGE_END:
            return "EXTRACTION_STAGE_END";
        case ITERATION_STAGE_BEGIN:
            return "ITERATION_STAGE_BEGIN";
        case ITERATION_STAGE_LOOP:
            return "ITERATION_STAGE_LOOP";
        case ITERATION_STAGE_INTROSPECT:
            return "ITERATION_STAGE_INTROSPECT";
        case ITERATION_STAGE_END:
            return "ITERATION_STAGE_END";
        case PARTITION_STAGE_BEGIN:
            return "PARTITION_STAGE_BEGIN";
        case PARTITION_STAGE_END:
            return "PARTITION_STAGE_END";
        default:
            return "UNKNOWN";
    }
}

#define LOCAL_CLOCK_START(flag, clock_name, stage_label) \
std::chrono::high_resolution_clock::time_point clock_name##_start;\
if ( flag ) { \
    clock_name##_start = std::chrono::high_resolution_clock::now();\
}

#define LOCAL_CLOCK_END(flag, clock_name, stage_label, snapshot_name, target_array, extra_routines) \
if ( flag ) { \
    std::chrono::high_resolution_clock::time_point clock_name##_end = std::chrono::high_resolution_clock::now();\
    std::chrono::TIME_UNIT clock_name = std::chrono::duration_cast<std::chrono::TIME_UNIT>(clock_name##_end - clock_name##_start);\
    snapshot_name.snapshot_point = stage_label;\
    extra_routines\
    target_array.emplace_back(snapshot_name);\
}

#define CLOCK_ROUTINE(flag,routine_to_clock, stage_label, snapshot_name, target_array, registry, cumulative_registry, extra_routines) \
if ( flag ) { \
    std::chrono::high_resolution_clock::time_point extraction_start = std::chrono::high_resolution_clock::now();\
    routine_to_clock;\
    std::chrono::high_resolution_clock::time_point extraction_end = std::chrono::high_resolution_clock::now();\
    snapshot_name.snapshot_point = stage_label;\
    snapshot_name.registry = std::chrono::duration_cast<std::chrono::TIME_UNIT>(extraction_end - extraction_start);\
    snapshot_name.cumulative_registry = snapshot_name.cumulative_registry + snapshot_name.registry;\
    extra_routines\
    target_array.emplace_back(snapshot_name);\
}\
else {\
    routine_to_clock;\
}

#define OPTIONAL_REGISTRY(flag, routine_to_regster, extra_routines) \
if ( flag ) { \
    routine_to_regster;\
    extra_routines;\
}\
else {\
    routine_to_regster;\
}

std::string build_snapshot_header(){
    std::ostringstream ss;
    std::string param_headers = "param.log_pivot_time,param.log_iteration_time,param.log_extraction_time,param.log_swaps,param.use_bfprt,param.use_iiqs,param.use_dutch_flag,param.use_random_pivot,param.enable_reuse,alpha_value,param.beta_value,param.pivot_bias,param.redundant_bias,random_seed_value,input_size,param.extractions,input_file_value,param.output_file_value";
    std::string snapshot_headers = "snapshot.iteration_time,snapshot.total_iteration_time,snapshot.partition_time,snapshot.total_partition_time,snapshot.bfprt_partition_time,snapshot.total_bfprt_partition_time,snapshot.extraction_time,snapshot.total_extraction_time,snapshot.current_extraction_executed_partitions,snapshot.total_executed_partitions,snapshot.current_iteration_executed_bfprt_partitions,snapshot.total_executed_bfprt_partitions,snapshot.current_iteration_partition_swaps,snapshot.total_executed_partition_swaps,snapshot.current_iteration_longest_partition_swap,snapshot.total_executed_longest_partition_swap,snapshot.current_iteration_bfprt_partition_swaps,snapshot.total_executed_bfprt_partition_swaps,snapshot.current_iteration_longest_bfprt_partition_swap,snapshot.total_executed_longest_bfprt_partition_swap,snapshot.current_extracted_pivot,snapshot.current_stack_size,snapshot.total_pushed_pivots,snapshot.total_pulled_pivots,snapshot.current_iteration_pushed_pivots,snapshot.current_iteration_pulled_pivots,snapshot.current_extraction,snapshot.input_size,snapshot.snapshot_point";
    ss << param_headers << "," << snapshot_headers;
    return ss.str();
}

std::string build_snapshot_values(configuration_t &configuration, snapshot_t &snapshot){
    std::ostringstream ss;

    ss << configuration.log_pivot_time << ",";
    ss << configuration.log_iteration_time << ",";
    ss << configuration.log_extraction_time << ",";
    ss << configuration.log_swaps << ",";
    ss << configuration.use_bfprt << ",";
    ss << configuration.use_iiqs << ",";
    ss << configuration.use_dutch_flag << ",";
    ss << configuration.use_random_pivot << ",";
    ss << configuration.enable_reuse << ",";
    ss << configuration.alpha_value << ",";
    ss << configuration.beta_value << ",";
    ss << configuration.pivot_bias << ",";
    ss << configuration.redundant_bias << ",";
    ss << configuration.random_seed_value << ",";
    ss << configuration.input_size << ",";
    ss << configuration.extractions << ",";
    ss << configuration.input_file_value << ",";
    ss << configuration.output_file_value << ",";

    ss << snapshot.iteration_time.count() << ",";
    ss << snapshot.total_iteration_time.count() << ",";
    ss << snapshot.partition_time.count() << ",";
    ss << snapshot.total_partition_time.count() << ",";
    ss << snapshot.bfprt_partition_time.count() << ",";
    ss << snapshot.total_bfprt_partition_time.count() << ",";
    ss << snapshot.extraction_time.count() << ",";
    ss << snapshot.total_extraction_time.count() << ",";
    ss << snapshot.current_extraction_executed_partitions << ",";
    ss << snapshot.total_executed_partitions << ",";
    ss << snapshot.current_iteration_executed_bfprt_partitions << ",";
    ss << snapshot.total_executed_bfprt_partitions << ",";

    
    ss << snapshot.current_iteration_partition_swaps << ",";
    ss << snapshot.total_executed_partition_swaps << ",";
    ss << snapshot.current_iteration_longest_partition_swap << ",";
    ss << snapshot.total_executed_longest_partition_swap << ",";
    ss << snapshot.current_iteration_bfprt_partition_swaps << ",";
    ss << snapshot.total_executed_bfprt_partition_swaps << ",";
    ss << snapshot.current_iteration_longest_bfprt_partition_swap << ",";
    ss << snapshot.total_executed_longest_bfprt_partition_swap << ",";


    ss << snapshot.current_extracted_pivot << ",";
    ss << snapshot.current_stack_size << ",";
    ss << snapshot.total_pushed_pivots << ",";
    ss << snapshot.total_pulled_pivots << ",";
    ss << snapshot.current_iteration_pushed_pivots << ",";
    ss << snapshot.current_iteration_pulled_pivots << ",";
    ss << snapshot.current_extraction << ",";
    ss << snapshot.input_size << ",";
    ss << point_to_string(snapshot.snapshot_point);

    return ss.str();
}



#endif