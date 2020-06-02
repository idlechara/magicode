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

#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iterator>
#include <array>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "flags.h"
#include "src/snapshot.h"

#include "src/IQS.cpp"
// #include "src/BareBoneIQS.cpp"
// #include "src/IIQS.cpp"
// #include "src/BareBoneIIQS.cpp"



int main(int argc, char const *argv[])
{

    bool log_pivot_time, log_iteration_time, log_extraction_time, use_bfprt, use_iiqs;
    double alpha_value, beta_value;
    int random_seed_value;
    std::size_t input_size, extractions;
    std::string input_file_value, output_file_value;

    boost::program_options::options_description desc("Program arguments");
    desc.add_options()
        ("help", "produce help message")
        ("log-pivot-time",      boost::program_options::value<bool>(&log_pivot_time)->default_value(false),             "record time taken by pivot selection")
        ("log-iteration-time",  boost::program_options::value<bool>(&log_iteration_time)->default_value(false),         "record time taken by iteration selection")
        ("log-extraction-time", boost::program_options::value<bool>(&log_extraction_time)->default_value(false),        "record time taken by extraction selection")
        ("use-bfprt",           boost::program_options::value<bool>(&use_bfprt)->default_value(false),                  "use bfprt as median selection strategy")
        ("use-bfprt",           boost::program_options::value<bool>(&use_iiqs)->default_value(false),                  "use IIQS as main algorithm")
        ("set-bfprt-alpha",     boost::program_options::value<double>(&beta_value)->default_value(0.7f),                "set bfprt alpha value")
        ("set-bfprt-beta",      boost::program_options::value<double>(&alpha_value)->default_value(0.3f),               "set bfprt beta value")
        ("set-random-seed",     boost::program_options::value<int>(&random_seed_value)->default_value(42),              "set random seed value")
        ("input-file",          boost::program_options::value<std::string>(&input_file_value)->required(),              "input file containing ascii numbers separated by a space")
        ("output-file",         boost::program_options::value<std::string>(&output_file_value)->required(),             "file to dump logs after execution is terminated")
        ("input-size",          boost::program_options::value<std::size_t>(&input_size)->required(),                    "amount of integers to read and load as input")
        ("extractions",         boost::program_options::value<std::size_t>(&extractions)->required(),                   "extractions to perform")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

    if(vm.count("help") || argc == 1){
        std::cout << desc << std::endl;
        return 1;
    }

    boost::program_options::notify(vm);

    // Begin start procedure
    // Load input entries

    std::vector<TYPE_TO_USE> input_array;
    std::fstream input_file(input_file_value, std::ios_base::in);

    for(size_t i = 0; i < input_size; i++){
        TYPE_TO_USE input_element;
        input_file >> input_element;
        input_array.push_back(input_element);
        std::cout << input_array[i] << " ";
    }

    input_file.close();


    std::vector<snapshot_t> snapshots;

    // TODO: add change for IQS

    IQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iqs(input_array);

    snapshot_t extraction_snapshot = generate_snapshot(EXTRACTION_STAGE_BEGIN);
    extraction_snapshot.input_size = input_size;

    for(int i = 0; i < extractions; i++){
        if (log_extraction_time) {
            extraction_snapshot.current_extraction = i;
            std::chrono::high_resolution_clock::time_point extraction_start = std::chrono::high_resolution_clock::now();
            iqs.next();
            std::chrono::high_resolution_clock::time_point extraction_end = std::chrono::high_resolution_clock::now();
            extraction_snapshot.snapshot_point = EXTRACTION_STAGE_END;
            extraction_snapshot.extraction_time = std::chrono::duration_cast<std::chrono::TIME_UNIT>(extraction_end - extraction_start);
            extraction_snapshot.total_extraction_time = extraction_snapshot.total_extraction_time + extraction_snapshot.extraction_time;
            extraction_snapshot.current_stack_size = iqs.stack.size();
            snapshots.push_back(extraction_snapshot);
        }
    }

    std::cout << "Amount of snapshots: " << snapshots.size() << std::endl;
    // Convert snapshot into string
    std::fstream output_file(output_file_value, std::ios_base::out);
    // Print headers
    output_file << "params.log_pivot_time, params.log_iteration_time, params.log_extraction_time, params.use_bfprt, "
                << "params.use_iiqs, params.alpha_value, params.beta_value, params.random_seed_value, params.input_size , "
                << "params.extractions, params.input_file_value, params.output_file_value , "
                << "snapshot.iteration_time, snapshot.total_iteration_time, snapshot.partition_time, "
                << "snapshot.bfprt_partition_time, snapshot.total_bfprt_partition_time, "
                << "snapshot.total_partition_time, snapshot.extraction_time, snapshot.total_extraction_time, "
                << "snapshot.current_iteration_executed_partitions, snapshot.total_executed_partitions, "
                << "snapshot.current_iteration_executed_bfprt_partitions, snapshot.total_executed_bfprt_partitions, "
                << "snapshot.current_extracted_pivot, snapshot.current_stack_size, snapshot.current_extraction, "
                << "snapshot.input_size, snapshot.snapshot_point)" << std::endl;


    for(size_t i=0; i<snapshots.size(); i++){
        snapshot_t snapshot = snapshots[i];
        output_file << log_pivot_time << ", " << log_iteration_time << ", " << log_extraction_time << ", " << use_bfprt << ", " 
                    << use_iiqs << ", " << alpha_value << ", " << beta_value << ", " << random_seed_value << ", " << input_size  << ", " 
                    << extractions << ", " << input_file_value << ", " << output_file_value  << ", " 
                    << snapshot.iteration_time.count() << ", " << snapshot.total_iteration_time.count() << ", " << snapshot.partition_time.count() << ", " 
                    << snapshot.bfprt_partition_time.count() << ", " << snapshot.total_bfprt_partition_time.count() << ", " 
                    << snapshot.total_partition_time.count() << ", " << snapshot.extraction_time.count() << ", " << snapshot.total_extraction_time.count() << ", " 
                    << snapshot.current_iteration_executed_partitions << ", " << snapshot.total_executed_partitions << ", " 
                    << snapshot.current_iteration_executed_bfprt_partitions << ", " << snapshot.total_executed_bfprt_partitions << ", " 
                    << snapshot.current_extracted_pivot << ", " << snapshot.current_stack_size << ", " << snapshot.current_extraction << ", " 
                    << snapshot.input_size << ", " << point_to_string(snapshot.snapshot_point) << std::endl;
    }
    
    output_file.close();


    std::cout << std::endl;



}