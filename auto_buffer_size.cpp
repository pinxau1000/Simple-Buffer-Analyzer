#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <limits.h> // Needed for windows compilation
#include "include/Buffer.hpp"
#include "include/Definitions.hpp"
#include "include/Analyzer.hpp"
#include "include/tclap/CmdLine.h"

int main(int argc, char** argv)
{
    // Parse Args
    TCLAP::CmdLine cmdParser("========================================             \n             _______________\nframes (in) |   buffer      |  bit-rate (out)\n       --->>|<--  length -->|--->>\n            |_______________|\n\nRequired inputs:            \n - frames:          provided by a csv input file;\n - bit-rate:        user input as parameter;\n - buffer length:   user input as parameter;\n\nBuffer Bit-Rate Tool by github.com/pinxau1000", ' ', VERSION);
    TCLAP::UnlabeledValueArg<std::string> arg_csv_file("csv_file", "Path to CSV file. The expected CSV structure is:\n<FPS>,0\n<BYTES>,<PSNR>\n ... \n<BYTES>,<PSNR>\n where, FPS: frames/seg; BYTES: bytes; PSNR: dB. (path)", true, "", "std::string", cmdParser);
    TCLAP::UnlabeledValueArg<long long int> arg_bit_rate("bit_rate", "Bit-rate of the network. (bits/s)", true, 0, "double", cmdParser);
    TCLAP::UnlabeledValueArg<long long int> arg_init_buffer_length("initial_buffer_occupation", "Number of bits required to start outputting data to the network. (bits)", true, 0, "int", cmdParser);
    TCLAP::ValueArg<long long int> arg_buffer_seed("s", "arg_buffer_seed", "The initial buffer value (int bits) that will be used to predict the optimal value. Defaults to random.", false, 0, "int", cmdParser);
    TCLAP::ValueArg<double> arg_learning_rate("r", "learning_rate", "Should be between 0 and 1 non-inclusive. Lower values cause the iteration step to be lower. Higher values cause the iteration step to be larger. Defaults to 0.5.", false, 0.5, "double", cmdParser);
    TCLAP::ValueArg<int> arg_fps("f", "frame_rate", "If passed then the FPS read from the csv_file is ignored! (frames/sec)", false, 0, "int", cmdParser);
    TCLAP::ValueArg<std::string> arg_unit("u", "unit", "The unit of the bits based fields [bit_rate, buffer_length & initial_buffer_occupation]. Possible options are: b, kb, Mb, Gb, B, kB, MB, GB. Default to b.", false, "b", "std::string", cmdParser);
    TCLAP::ValueArg<std::string> arg_generate_csv("c", "generate_csv", "If passed a csv file containing some statistics will be generated with the name provided.", false, "", "std::string", cmdParser);
    TCLAP::ValueArg<std::string> arg_generate_plot("p", "generate_plot", "If passed a plot with some statistics will be generated with the name provided.", false, "", "std::string", cmdParser);
    TCLAP::ValueArg<int> arg_verbose("v", "verbose", "Sets the verbose level. Values are: " + std::to_string(VERBOSE_NONE) + " - No output; " + std::to_string(VERBOSE_MINIMAL) + " - Minimal output; " + std::to_string(VERBOSE_STANDARD) + " - Standard output; " + std::to_string(VERBOSE_EXTRA) + " - Extra output; " + std::to_string(VERBOSE_ALL) + " - All output. Defaults to " + std::to_string(VERBOSE_STANDARD), false, VERBOSE_STANDARD, "int", cmdParser);
    try
    {
        cmdParser.parse(argc, argv);
    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    if ((arg_init_buffer_length.getValue() < 1) || (arg_init_buffer_length.getValue() > BITS_MAX)) {
        std::cerr << STDERR_ERROR << "Buffer initial length should be between 1 and " << BITS_MAX << ". Got " << arg_init_buffer_length.getValue() << "!" << std::endl;
        throw VALUE_ERROR_EXCEPTION;
    }

    if ((arg_buffer_seed.isSet()) && (arg_buffer_seed.getValue() < 1 || arg_buffer_seed.getValue() > BITS_MAX)) {
        std::cerr << STDERR_ERROR << "Buffer length initial search value (seed) should be between 1 and " << BITS_MAX << ". Got " << arg_init_buffer_length.getValue() << "!" << std::endl;
        throw VALUE_ERROR_EXCEPTION;
    }
    if (arg_buffer_seed.isSet() && arg_buffer_seed.getValue() < arg_init_buffer_length.getValue()) {
        std::cerr << STDERR_ERROR << "Buffer length initial search value (seed) should be bigger than buffer occupation. Got " << arg_buffer_seed.getValue() << " expected <" << arg_init_buffer_length.getValue() << "!" << std::endl;
        throw VALUE_ERROR_EXCEPTION;
    }

    std::default_random_engine generator;
    std::uniform_int_distribution<char> distribution_step(0, 1);

    long long int cur_buffer_length = arg_buffer_seed.getValue();
    if (arg_buffer_seed.getValue() < arg_init_buffer_length.getValue())
    {
        std::uniform_int_distribution<long long int> distribution_current_buffer_length(arg_init_buffer_length.getValue(), BITS_MAX);
        cur_buffer_length = distribution_current_buffer_length(generator);
    }
    long long int step = llround(cur_buffer_length * arg_learning_rate.getValue());
    Buffer buffer;
    State last_invalid_state = State::underflow;
    long long int min_buffer_length = LONG_LONG_MAX;
    int iter_count = 0;

    if (arg_verbose >= VERBOSE_EXTRA) { if (arg_verbose >= VERBOSE_ALL) { std::cout << SEPARATOR_41 << std::endl; std::cout << "*\t\t AUTO BUFFER VAR VALUES \t\t*" << std::endl; std::cout << SEPARATOR_41 << std::endl; std::cout << STDOUT_PARSE << "input file: " << arg_csv_file.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "bit-rate: " << arg_bit_rate.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "buffer initial search value (seed): " << arg_buffer_seed.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "learning rate: " << arg_learning_rate.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "buffer initial length: " << arg_init_buffer_length.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "fps: " << arg_fps.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "units: " << arg_unit.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "output csv: " << arg_generate_csv.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "output plot: " << arg_generate_plot.getValue() << ";" << std::endl; std::cout << STDOUT_PARSE << "verbosity: " << arg_verbose.getValue() << ";" << std::endl; } std::cout << STDOUT_COMPUTE << "buffer initial search value (seed): " << cur_buffer_length << ";" << std::endl; std::cout << STDOUT_COMPUTE << "buffer search learning rate: " << arg_learning_rate.getValue() << ";" << std::endl; std::cout << STDOUT_COMPUTE << "buffer search step: " << step << ";" << std::endl; }
    if (arg_verbose.getValue() >= VERBOSE_STANDARD) { std::cout << SEPARATOR_41 << std::endl; std::cout << "*\t\t\t\t AUTO LENGTH \t\t\t*" << std::endl; std::cout << SEPARATOR_41 << std::endl; }

    while (true)
    {
        if (cur_buffer_length > arg_init_buffer_length.getValue())
        {
            buffer = analyze(arg_csv_file.getValue(), arg_bit_rate.getValue(), cur_buffer_length,
                             arg_init_buffer_length.getValue(), arg_fps.getValue(), arg_unit.getValue(),
                             "", "", VERBOSE_NONE);

            step = ceil(step * arg_learning_rate.getValue());
            step += distribution_step(generator);

            if (buffer.get_state() == State::overflow)
            {
                cur_buffer_length += llabs(step);
                last_invalid_state = State::overflow;
            }
            else if (buffer.get_state() == State::underflow)
            {
                cur_buffer_length -= llabs(step);
                last_invalid_state = State::underflow;
            }
            else if (buffer.get_state() == State::done)
            {
                if (last_invalid_state == State::overflow && cur_buffer_length == min_buffer_length) {
                    break;
                } else {
                    // Update min
                    min_buffer_length = (cur_buffer_length < min_buffer_length) ? cur_buffer_length : min_buffer_length;
                    cur_buffer_length -= llabs(step);
                }
            }
        }
        else
        {
            if (arg_verbose >= VERBOSE_ALL) { std::cout << STDOUT_WARNING << "ignoring: buffer length test - cause: possible high learning rate - buffer length: " << cur_buffer_length << " - step: " << step << " - iteration: " << iter_count << ";" << std::endl; }
            cur_buffer_length += llabs(step);
            step = ceil(step * arg_learning_rate.getValue());
            step += distribution_step(generator);
        }
        iter_count++;

        if (arg_verbose.getValue() >= VERBOSE_ALL) { std::cout << STDOUT_COMPUTE << "buffer last/state: " << buffer.get_last_state_str() << "/" << buffer.get_state_str() << " - length minimum/current: " << min_buffer_length << "/" << cur_buffer_length << ";" << std::endl; }
    }

    if (arg_verbose.getValue() >= VERBOSE_STANDARD)
    {
        std::cout << STDOUT_COMPUTE << "minimal buffer length: " << cur_buffer_length << ";" << std::endl;
        std::cout << STDOUT_COMPUTE << "initial buffer length: " << arg_init_buffer_length.getValue() << ";" << std::endl;
        std::cout << STDOUT_COMPUTE << "iterations: " << iter_count << ";" << std::endl;
    }
    if (arg_verbose.getValue() == VERBOSE_MINIMAL)
    {
        std::cout << iter_count << ": " << cur_buffer_length << ";" << std::endl;
    }

    // output the final results
    analyze(arg_csv_file.getValue(),
            arg_bit_rate.getValue(),
            cur_buffer_length,
            arg_init_buffer_length.getValue(),
            arg_fps.getValue(),
            arg_unit.getValue(),
            arg_generate_csv.getValue(),
            arg_generate_plot.getValue(),
            arg_verbose.getValue()
    );
    return EXIT_SUCCESS;
}
