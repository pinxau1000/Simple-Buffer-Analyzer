#include <iostream>
#include <string>
#include <cmath>
#include "include/Buffer.hpp"
#include "include/Definitions.hpp"
#include "include/Analyzer.hpp"
#include "include/tclap/CmdLine.h"

int main(int argc, char** argv)
{
    // Parse Args
    TCLAP::CmdLine cmdParser("========================================             \n             _______________\nframes (in) |   buffer      |  bit-rate (out)\n       --->>|<--  length -->|--->>\n            |_______________|\n\nRequired inputs:            \n - frames:          provided by a csv input file;\n - bit-rate:        user input as parameter;\n - buffer length:   user input as parameter;\n\nBuffer Bit-Rate Tool by Jose Rosa (github.com/pinxau1000)", ' ', VERSION);
    TCLAP::UnlabeledValueArg<std::string> arg_csv_file("csv_file", "Path to CSV file. The expected CSV structure is:\n<FPS>,0\n<BYTES>,<PSNR>\n ... \n<BYTES>,<PSNR>\n where, FPS: frames/seg; BYTES: bytes; PSNR: dB. (path)", true, "", "std::string", cmdParser);
    TCLAP::UnlabeledValueArg<int> arg_bit_rate("bit_rate", "Bit-rate of the network. ((defined -u)/s)", true, 0, "int", cmdParser);
    TCLAP::ValueArg<int> arg_init_buffer_length("i", "initial_buffer_occupation", "Number of bits required to start outputing data to the network. If not specified then it's 0.5*buffer_length. (defined -u)", false, 0, "int", cmdParser);
    TCLAP::ValueArg<int> arg_fps("f", "frame_rate", "If passed then the FPS read from the csv_file is ignored! (frames/sec)", false, 0, "int", cmdParser);
    TCLAP::ValueArg<std::string> arg_unit("u", "unit", "The unit of the bits based fields [bit_rate, buffer_length & initial_buffer_occupation]. Possible options are: b, kb, Mb, Gb, B, kB, MB, GB. Default to b.", false, "b", "std::string", cmdParser);
    TCLAP::ValueArg<std::string> arg_generate_csv("c", "generate_csv", "If passed a csv file containing some statistics will be generated with the name provided.", false, "", "std::string", cmdParser);
    TCLAP::ValueArg<std::string> arg_generate_plot("p", "generate_plot", "If passed a plot with some statistics will be generated with the name provided.", false, "", "std::string", cmdParser);
    TCLAP::ValueArg<int> arg_verbose("v", "verbose", "Sets the verbose level. Values are: " + std::to_string(VERBOSE_MINIMAL) + " - Minimal output; " + std::to_string(VERBOSE_STANDARD) + " - Standard output; " + std::to_string(VERBOSE_EXTRA) + " - Extra output; " + std::to_string(VERBOSE_ALL) + " - All output. Defaults to 1.", false, 1, "int", cmdParser);
    try
    {
        cmdParser.parse(argc, argv);
    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    int cur_buffer_length = rand();
    Buffer buffer;

    while (true)
    {
        buffer = analyze(arg_csv_file.getValue(), arg_bit_rate.getValue(), cur_buffer_length,
                    arg_init_buffer_length.getValue(), arg_fps.getValue(), arg_unit.getValue(), "", "",
                    VERBOSE_NONE);

        if (buffer.get_state() == State::normal ||
            buffer.get_state() == State::overflow ||
            buffer.get_state() == State::underflow)
        {
            break;
        }
    }

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
