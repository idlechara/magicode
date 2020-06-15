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
#include "src/IIQS.cpp"
// #include "src/BareBoneIIQS.cpp"



int main(int argc, char const *argv[])
{
    configuration_t configuration;
    configuration_t *configuration_ptr = &configuration;

    boost::program_options::options_description options("Program arguments");
    options.add_options()
        ("help", "produce help message")
        ("log-pivot-time",      boost::program_options::value<bool>(&(configuration_ptr)->log_pivot_time)->default_value(false),             "record time taken by pivot selection")
        ("log-iteration-time",  boost::program_options::value<bool>(&(configuration_ptr)->log_iteration_time)->default_value(false),         "record time taken by iteration selection")
        ("log-extraction-time", boost::program_options::value<bool>(&(configuration_ptr)->log_extraction_time)->default_value(false),        "record time taken by extraction selection")
        ("log-swaps",           boost::program_options::value<bool>(&(configuration_ptr)->log_swaps)->default_value(false),                  "record swaps used")
        ("use-bfprt",           boost::program_options::value<bool>(&(configuration_ptr)->use_bfprt)->default_value(false),                  "use bfprt as median selection strategy")
        ("use-iiqs",            boost::program_options::value<bool>(&(configuration_ptr)->use_iiqs)->default_value(false),                   "use IIQS as main algorithm")
        ("use-random-pivot",    boost::program_options::value<bool>(&(configuration_ptr)->use_random_pivot)->default_value(false),           "use random pivot selection")
        ("set-bfprt-alpha",     boost::program_options::value<double>(&(configuration_ptr)->beta_value)->default_value(0.7f),                "set bfprt alpha value")
        ("set-bfprt-beta",      boost::program_options::value<double>(&(configuration_ptr)->alpha_value)->default_value(0.3f),               "set bfprt beta value")
        ("set-random-seed",     boost::program_options::value<int>(&(configuration_ptr)->random_seed_value)->default_value(42),              "set random seed value")
        ("set-pivot-bias",      boost::program_options::value<double>(&(configuration_ptr)->pivot_bias)->default_value(0.5),                 "set pivot selection bias, 0.0= left, 1.0=right, 0.5=center")
        ("input-file",          boost::program_options::value<std::string>(&(configuration_ptr)->input_file_value)->required(),              "input file containing ascii numbers separated by a space")
        ("output-file",         boost::program_options::value<std::string>(&(configuration_ptr)->output_file_value)->required(),             "file to dump logs after execution is terminated")
        ("input-size",          boost::program_options::value<std::size_t>(&(configuration_ptr)->input_size)->required(),                    "amount of integers to read and load as input")
        ("extractions",         boost::program_options::value<std::size_t>(&(configuration_ptr)->extractions)->required(),                   "extractions to perform")
    ;

    boost::program_options::variables_map variable_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), variable_map);

    if(variable_map.count("help") || argc == 1){
        std::cout << options << std::endl;
        return 1;
    }

    boost::program_options::notify(variable_map);

    // Begin start procedure
    // Load input entries

    std::vector<TYPE_TO_USE> input_array;
    std::fstream input_file(configuration.input_file_value, std::ios_base::in);

    for(size_t i = 0; i < configuration.input_size; i++){
        TYPE_TO_USE input_element;
        input_file >> input_element;
        input_array.emplace_back(input_element);
    }

    input_file.close();

    std::vector<snapshot_t> snapshots;
    snapshot_t extraction_snapshot = generate_snapshot(EXTRACTION_STAGE_BEGIN);
    extraction_snapshot.input_size = configuration.input_size;

    IQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE> iqs = (configuration.use_iiqs) ? IIQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE>(input_array, configuration, snapshots, extraction_snapshot) : IQS<std::vector<TYPE_TO_USE>, TYPE_TO_USE>(input_array, configuration, snapshots, extraction_snapshot);


    for(int i = 0; i < configuration.extractions; i++){
        extraction_snapshot.current_extraction = i;

        if (configuration.log_extraction_time) {
            std::chrono::high_resolution_clock::time_point extraction_start = std::chrono::high_resolution_clock::now();
            iqs.next();
            std::chrono::high_resolution_clock::time_point extraction_end = std::chrono::high_resolution_clock::now();
            extraction_snapshot.snapshot_point = EXTRACTION_STAGE_END;
            extraction_snapshot.extraction_time = std::chrono::duration_cast<std::chrono::TIME_UNIT>(extraction_end - extraction_start);
            extraction_snapshot.total_extraction_time = extraction_snapshot.total_extraction_time + extraction_snapshot.extraction_time;
            extraction_snapshot.current_stack_size = iqs.stack.size();
            snapshots.emplace_back(extraction_snapshot);
        }
        else{
            iqs.next();
        }
    }

    std::cout << "Amount of snapshots: " << snapshots.size() << std::endl;
    // Convert snapshot into string
    std::fstream output_file(configuration.output_file_value, std::ios_base::out);
    // Print headers
    output_file << "index," << build_snapshot_header() << std::endl;


    for(size_t i=0; i<snapshots.size(); i++) {
        snapshot_t snapshot = snapshots[i];
        output_file << i << "," << build_snapshot_values(configuration, snapshot) << std::endl;
    }
    
    output_file.close();


    std::cout << std::endl;



}