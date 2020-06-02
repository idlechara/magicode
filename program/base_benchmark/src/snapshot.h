#ifndef IQS_TEST_SNAPSHOT_T_H
#define IQS_TEST_SNAPSHOT_T_H

#include <chrono>
#include "./../flags.h"

#define PROGRAM_STAGE_BEGIN         1
#define PROGRAM_STAGE_END           2
#define EXTRACTION_STAGE_BEGIN      3
#define EXTRACTION_STAGE_END        4
#define PARTITION_STAGE_BEGIN       5
#define PARTITION_STAGE_END         6

typedef struct {
    bool log_pivot_time, log_iteration_time, log_extraction_time, use_bfprt, use_iiqs;
    double alpha_value, beta_value, pivot_bias;
    int random_seed_value;
    std::size_t input_size, extractions;
    std::string input_file_value, output_file_value;
} configuration_t;

typedef struct
{
    std::chrono::TIME_UNIT iteration_time;
    std::chrono::TIME_UNIT total_iteration_time;

    std::chrono::TIME_UNIT partition_time;
    std::chrono::TIME_UNIT total_partition_time;

    std::chrono::TIME_UNIT bfprt_partition_time;
    std::chrono::TIME_UNIT total_bfprt_partition_time;

    std::chrono::TIME_UNIT extraction_time;
    std::chrono::TIME_UNIT total_extraction_time;

    size_t current_iteration_executed_partitions;
    size_t total_executed_partitions;

    size_t current_iteration_executed_bfprt_partitions;
    size_t total_executed_bfprt_partitions;

    size_t current_extracted_pivot;

    size_t current_stack_size;
    
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
    snapshot.current_iteration_executed_partitions = 0;
    snapshot.current_iteration_executed_bfprt_partitions = 0;
    snapshot.total_executed_partitions = 0;
    snapshot.total_executed_bfprt_partitions = 0;
    snapshot.current_extracted_pivot = 0;
    snapshot.current_stack_size = 0;
    snapshot.current_extraction = 0;
    snapshot.input_size = 0;
    snapshot.snapshot_point = point;
    return snapshot;
}

std::string point_to_string(char point){
    switch (point) {
        case PROGRAM_STAGE_BEGIN:
            return "PROGRAM_STAGE_BEGIN";
        case PROGRAM_STAGE_END:
            return "PROGRAM_STAGE_END";
        case EXTRACTION_STAGE_BEGIN:
            return "EXTRACTION_STAGE_BEGIN";
        case EXTRACTION_STAGE_END:
            return "EXTRACTION_STAGE_END";
        case PARTITION_STAGE_BEGIN:
            return "PARTITION_STAGE_BEGIN";
        case PARTITION_STAGE_END:
            return "PARTITION_STAGE_END";
        default:
            return "UNKNOWN";
    }
}

#endif