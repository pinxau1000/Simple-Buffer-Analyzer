#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include "Buffer.cpp"
#include "Bitstream.cpp"
#include "Definitions.hpp"
#include "include/tclap/CmdLine.h"

using namespace std;

int main(int argc, char** argv)
{
    // Parse Args
    TCLAP::CmdLine cmdParser("========================================             \n             _______________\nframes (in) |   buffer      |  bit-rate (out)\n       --->>|<--  length -->|--->>\n            |_______________|\n\nRequired inputs:            \n - frames:          provided by a csv input file;\n - bit-rate:        user input as parameter;\n - buffer length:   user input as parameter;\n\nBuffer Bit-Rate Tool by Jose Rosa (github.com/pinxau1000)", ' ', VERSION);
    TCLAP::UnlabeledValueArg<std::string> arg_csv_file("csv_file", "Path to CSV file. The expected CSV structure is:\n<FPS>,0\n<BYTES>,<PSNR>\n ... \n<BYTES>,<PSNR>\n where, FPS: frames/seg; BYTES: bytes; PSNR: dB. (path)", true, "", "string", cmdParser);
    TCLAP::UnlabeledValueArg<int> arg_bit_rate("bit_rate", "Bit-rate of the network. ((defined -u)/s)", true, 0, "int", cmdParser);
    TCLAP::UnlabeledValueArg<int> arg_buffer_length("buffer_length", "Buffer length or size. (defined -u)", true, 0, "int", cmdParser);
    TCLAP::ValueArg<int> arg_init_buffer_length("i", "initial_buffer_occupation", "Number of bits required to start outputing data to the network. If not specified then it's 0.5*buffer_length. (defined -u)", false, 0, "int", cmdParser);
    TCLAP::ValueArg<int> arg_fps("f", "frame_rate", "If passed then the FPS read from the csv_file is ignored! (frames/sec)", false, 0, "int", cmdParser);
    TCLAP::ValueArg<std::string> arg_unit("u", "unit", "The unit of the bits based fields [bit_rate, buffer_length & initial_buffer_occupation]. Possible options are: b, kb, Mb, Gb, B, kB, MB, GB. Default to b.", false, "b", "string", cmdParser);
    TCLAP::ValueArg<std::string> arg_generate_csv("c", "generate_csv", "If passed a csv file containing some statistics will be generated with the name provided.", false, "", "string", cmdParser);
    TCLAP::ValueArg<std::string> arg_generate_plot("p", "generate_plot", "If passed a plot with some statistics will be generated with the name provided.", false, "", "string", cmdParser);
    TCLAP::ValueArg<int> arg_verbose("v", "verbose", "Sets the verbose level. Values are: " + to_string(VERBOSE_STANDARD) + " - Standard output; " + to_string(VERBOSE_EXTRA) + " - Extra output; " + to_string(VERBOSE_ALL) + " - All output. Defaults to 1.", false, 1, "int", cmdParser);
    try
    {  
	    cmdParser.parse(argc, argv);
	}
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    
    // Parse units and get multiplier factor
    int factor = 1;
    if (arg_unit.getValue() == "kb"){ factor = 1e3; }
    else if (arg_unit.getValue() == "Mb"){ factor = 1e6; }
    else if (arg_unit.getValue() == "B"){ factor = 8; }
    else if (arg_unit.getValue() == "kB"){ factor = (1e3)/(double)8; }
    else if (arg_unit.getValue() == "MB"){ factor = (1e6)/(double)8; }

    if (arg_verbose >= VERBOSE_EXTRA)
    {
        cout << SEPARATOR_41 << endl;
        cout << "*\t\t\t\t FILE PARSING \t\t\t*" << endl;
        cout << SEPARATOR_41 << endl;
    }
    // Define the Communication System variables
    Bitstream bitstream(arg_csv_file.getValue(), arg_verbose.getValue());   //bitstream with the frames and fps
    if (arg_fps.isSet()){
        bitstream.set_fps(arg_fps.getValue());
    }

    Buffer buffer(arg_buffer_length.getValue()*(double)factor, bitstream.get_total_frames(), -1, arg_verbose.getValue());   //the encoder buffer
    if (arg_init_buffer_length.isSet()){
        buffer.set_initial_occupation(arg_init_buffer_length.getValue()*(double)factor);
    }

    // Actual bit-rate, since we are working with the frame time base, we need 
    // to calculate the bit-rate needed for each frame. So with 1 frame comes
    // between T secs then the actual bit-rate (the frame bit-rate) is equal to
    // T*bit-rate.
    // 
    // bit-rate (bit/s) ___ 1 (s)       <=> bit-rate (bit/s) ___ 1 (s)      
    //    x (bit/frame) ___ T frame (s)        x (bit/frame) ___ 1/Hz (Frames/s)
    // 
    int frame_bit_rate = round((arg_bit_rate.getValue() * (double) factor) / (double) bitstream.get_fps());    //the bit rate per frame (b/s)
    int actual_bit_rate = frame_bit_rate *(double) bitstream.get_fps();                                     //actual bit-rate, since the bit-rate per frame is a rounded value (b/s)
    double delay = buffer.get_length() / (double) arg_bit_rate.getValue();                                  //delay with the original parameters (s)
    double actual_delay = buffer.get_length() / (double) actual_bit_rate;                                   //actual delay (s)
    double actual_delay_frames = actual_delay * (double) bitstream.get_fps();                                   //actual delay (s)

    if (arg_verbose.getValue() >= VERBOSE_EXTRA)
    {
        cout << SEPARATOR_41 << endl;
        cout << "*\t\t\t\t VAR VALUES \t\t\t*" << endl;
        cout << SEPARATOR_41 << endl;
        cout << PARSE << "units: " <<  arg_unit.getValue() << ";" << endl;
        if (arg_verbose.getValue() >= VERBOSE_EXTRA)
        {
            cout << PARSE << "factor: " <<  factor << ";" << endl;
        }
        cout << PARSE << "FPS: " <<  bitstream.get_fps() << " (Hz);" << endl;
        cout << PARSE << "buffer length: " <<  buffer.get_length() << " (bits);" << endl;
        cout << PARSE << "buffer initial occupation: " <<  buffer.get_initial_occupation() << " (bits);" << endl;
        cout << PARSE << "bit-rate: " <<  arg_bit_rate.getValue()*(double)factor << " (bits/s);" << endl;
        cout << COMPUTE << "delay: " <<  delay << " (s);" << endl;
        if (arg_verbose.getValue() >= VERBOSE_EXTRA)
        {
            cout << COMPUTE << "frame bit-rate: " << frame_bit_rate << " (bits/frame);" << endl;
        }
        cout << COMPUTE << "actual bit-rate: " << actual_bit_rate << " (bits/s);" << endl;
        cout << COMPUTE << "actual delay: " << actual_delay << " (s);" << endl;
        cout << COMPUTE << "actual delay: " << actual_delay_frames << " (frames);" << endl;
    }

    // 2.1 - Verificação de conformidade
    int count = 0;
    vector<double> time_stamp;
    vector<int> bit_rate_in;
    vector<int> bit_rate_out;
    vector<int> buffer_occupation;
    vector<int> buffer_length;

    if (arg_verbose >= VERBOSE_ALL)
    {
        cout << SEPARATOR_41 << endl;
        cout << "*\t\t\t\t BUFFER STATUS \t\t\t*" << endl;
        cout << SEPARATOR_41 << endl;
    }
    while (true)
    {
        if (buffer.get_state() == State::overflow || buffer.get_state() == State::underflow) {
            
            cout << SEPARATOR_41 << endl;
            cout << "*\t\t\t\t BUFFER FAIL \t\t\t*" << endl;
            cout << SEPARATOR_41 << endl;
            cout << OUTPUT << "buffer (encoder) status: " << buffer.get_state_str() << ";" << endl;
            cout << OUTPUT << "buffer (encoder) fail frame: " << buffer.get_cur_frame() - 1 << ";" << endl;
            cout << OUTPUT << "buffer (encoder) fail time: " << (buffer.get_cur_frame() - 1)/bitstream.get_fps() << ";" << endl;
            cout << OUTPUT << "buffer (decoder) status: " << buffer.get_state_str_inverted() << ";" << endl;
            cout << OUTPUT << "buffer (decoder) fail frame: " << ((buffer.get_cur_frame() - 1) + (actual_delay*bitstream.get_fps())) << ";" << endl;
            cout << OUTPUT << "buffer (decoder) fail time: " << (((buffer.get_cur_frame() - 1)/bitstream.get_fps()) + actual_delay) << ";" << endl;
            cout << OUTPUT << "buffer (decoder) status: " << buffer.get_state_str_inverted() << ";" << endl;
            cout << OUTPUT << "buffer (decoder) fail frame: " << (buffer.get_cur_frame() - 1) + floor(actual_delay_frames) << ";" << endl;
            cout << OUTPUT << "buffer (decoder) fail time: " << ((buffer.get_cur_frame() - 1)/bitstream.get_fps()) + actual_delay << ";" << endl;
            break;
        }
        if (buffer.get_state() == State::done)
        {
            cout << SEPARATOR_41 << endl;
            cout << "*\t\t\t BUFFER SUCCESS \t\t\t*" << endl;
            cout << SEPARATOR_41 << endl;
            cout << OUTPUT << "buffer status: " << buffer.get_state_str() << ";" << endl;
            cout << OUTPUT << "processed frames: " << buffer.get_cur_frame() - 1 << ";" << endl;
            cout << OUTPUT << "empty buffer cycles: " << count-(buffer.get_cur_frame()) << ";" << endl;
            // TODO CONFIRM cout << OUTPUT << "empty time: " << (count-frame_idx) / (double) bitstream.get_fps() << " (s);" << endl;
            break;
        }

        if (buffer.get_state() == State::init)
        {
            buffer.in(bitstream.get_frame(buffer.get_cur_frame() - 1).get_bits());
        }
        else if (buffer.get_state() == State::normal)
        {
            buffer.in_out(bitstream.get_frame(buffer.get_cur_frame() - 1).get_bits(), frame_bit_rate);
        }
        else
        {
            buffer.out(frame_bit_rate);
        }
        count += 1;

        // Capture
        if (!arg_generate_csv.getValue().empty() || !arg_generate_plot.getValue().empty())
        {
            time_stamp.push_back(count/(double)bitstream.get_fps());
            count < bitstream.get_total_frames() ? bit_rate_in.push_back(bitstream.get_frame(buffer.get_cur_frame() - 1).get_bits()) : bit_rate_in.push_back(0);
            bit_rate_out.push_back(frame_bit_rate);
            buffer_occupation.push_back(buffer.get_occupation());
            buffer_length.push_back(buffer.get_length());
        }
    }

    if (!arg_generate_csv.getValue().empty() || !arg_generate_plot.getValue().empty())
    {
        time_t raw_time;
        tm* time_info;
        char plt_csv_name[27];
        time_info = localtime(&raw_time);
        strftime(plt_csv_name, 14, "plotdata_%Y%m%d%H%M%S.csv", time_info);
        
        string csv_name = (!arg_generate_csv.getValue().empty()) ? arg_generate_csv.getValue() : plt_csv_name ;

        fstream output_file(csv_name, ios::out);
        if (!output_file.is_open())
        {
            std::cerr << ERROR << "Unable to create " << csv_name << "!" << std::endl;
            throw FILE_UNABLE_TO_WRITE;
        }

        output_file << "time stamp (s)" << CSV_FILE_SEPARATOR << "bit in (bits)" << CSV_FILE_SEPARATOR << "bits out (bits)" << CSV_FILE_SEPARATOR << "buffer occupation (bit)" << CSV_FILE_SEPARATOR << "buffer length (bit)" << CSV_FILE_SEPARATOR << endl;
        for (size_t i = 0; i < time_stamp.size(); i++)
        {
            output_file << time_stamp[i] << CSV_FILE_SEPARATOR << bit_rate_in[i] << CSV_FILE_SEPARATOR << bit_rate_out[i] << CSV_FILE_SEPARATOR << buffer_occupation[i] << CSV_FILE_SEPARATOR << buffer_length[i] << CSV_FILE_SEPARATOR << endl;
        }
        output_file.close();

        if (!arg_generate_plot.getValue().empty())
        {
            string command = "python save_csv_data.py " + csv_name + " " + arg_generate_plot.getValue();
            system(command.c_str());
            if (arg_generate_csv.getValue().empty()) {
                remove(csv_name.c_str());
            }
        }
    }
    

    // 2.2 - Débito binário médio
    cout << SEPARATOR_41 << endl;
    cout << "*\t\t\t\t INPUT STATS \t\t\t*" << endl;
    cout << SEPARATOR_41 << endl;
    if (arg_verbose >= VERBOSE_EXTRA)
    {
        cout << OUTPUT << "sequence duration: " << bitstream.get_total_duration() << " (s);" << endl;
        cout << OUTPUT << "sequence frames: " << bitstream.get_total_frames() << ";" << endl;
        cout << OUTPUT << "sequence total bits: " << bitstream.get_total_bits() << " (bits);" << endl;
        cout << OUTPUT << "sequence total psnr: " << bitstream.get_total_psnr() << " (bits);" << endl;
        cout << OUTPUT << "frame avg bits: " << round(bitstream.get_total_bits()/(double)bitstream.get_total_frames()) << " (bits);" << endl;
        cout << OUTPUT << "frame avg psnr: " << round(bitstream.get_total_psnr()/(double)bitstream.get_total_frames()) << " (bits);" << endl;
    }
    cout << OUTPUT << "buffer input avg bit-rate: " << round(bitstream.get_total_bits()/(double)bitstream.get_total_duration()) << " (bits/s);" << endl;
    
    // 2.3 - Débitos binários máximo e mínimo em VBR
    // No pior cenário podem vir N frames com poucos bits ou com muitos bits. O N é dado pelos FPS!
    cout << SEPARATOR_41 << endl;
    cout << "*\t\t\t\t OUTPUT STATS \t\t\t*" << endl;
    cout << SEPARATOR_41 << endl;
    if (arg_verbose >= VERBOSE_EXTRA)
    {
        cout << OUTPUT << "sequence FPS: " << bitstream.get_fps() << " (Hz);" << endl;
        cout << OUTPUT << "frame smaller: " << bitstream.get_min_bits() << " (bits);" << endl;
        cout << OUTPUT << "frame avg: " << round(bitstream.get_total_bits()/(double)bitstream.get_total_frames()) << " (bits);" << endl;
        cout << OUTPUT << "frame bigger: " << bitstream.get_max_bits() << " (bits);" << endl;
    }
    cout << OUTPUT << "buffer output min bit-rate: " << round(bitstream.get_min_bits()*(double)bitstream.get_fps()) << " (bits/s);" << endl;
    // TODO CONFIRM cout << OUTPUT << "buffer output avg bit-rate: " << round((bitstream.get_total_bits()/(double)bitstream.get_total_frames())*(double)bitstream.get_fps()) << " (bits/s);" << endl;
    cout << OUTPUT << "buffer output max bit-rate: " << round(bitstream.get_max_bits()*(double)bitstream.get_fps()) << " (bits/s);" << endl;
    
    // 2.4 - Transmissão VBR
    // TODO
    return EXIT_SUCCESS;
}


