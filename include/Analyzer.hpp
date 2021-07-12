//
// Created by pinxau1000 on 12/07/21.
//

#ifndef MAIN_ANALYZER_HPP
#define MAIN_ANALYZER_HPP

#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include "Buffer.hpp"
#include "Bitstream.hpp"
#include "Definitions.hpp"

Buffer analyze(std::string arg_csv_file, double arg_bit_rate, double arg_buffer_length, double arg_init_buffer_length = 0,
               int arg_fps = 0, std::string arg_unit = "b", std::string arg_generate_csv = "",
               std::string arg_generate_plot = "", int arg_verbose = VERBOSE_STANDARD);

#endif //MAIN_ANALYZER_HPP
