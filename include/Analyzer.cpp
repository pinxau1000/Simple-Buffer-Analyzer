#include "Analyzer.hpp"

Buffer analyze(std::string arg_csv_file, double arg_bit_rate, double arg_buffer_length, double arg_init_buffer_length,
            int arg_fps, std::string arg_unit, std::string arg_generate_csv, std::string arg_generate_plot,
            int arg_verbose){

    // Parse units and get multiplier factor
    int factor = 1;
    if (arg_unit == "kb"){ factor = 1e3; }
    else if (arg_unit == "Mb"){ factor = 1e6; }
    else if (arg_unit == "B"){ factor = 8; }
    else if (arg_unit == "kB"){ factor = (1e3) * 8; }
    else if (arg_unit == "MB"){ factor = (1e6) * 8; }

    long long int bit_rate = round(arg_bit_rate * (double) factor);
    long long int buffer_length = round(arg_buffer_length * (double) factor);
    long long int init_buffer_length = round(arg_init_buffer_length * (double) factor);

    if (arg_verbose >= VERBOSE_EXTRA)
    {
        std::cout << SEPARATOR_41 << std::endl;
        std::cout << "*\t\t\t\t FILE PARSING \t\t\t*" << std::endl;
        std::cout << SEPARATOR_41 << std::endl;
    }
    // Define the Communication System variables
    Bitstream bitstream(arg_csv_file, arg_verbose);   //bitstream with the frames and fps
    if (arg_fps > 0){
        bitstream.set_fps(arg_fps);
    }

    Buffer buffer(buffer_length, bitstream.get_total_frames(), -1,
                  arg_verbose);   //the encoder buffer
    if (init_buffer_length > 0){
        buffer.set_initial_occupation(init_buffer_length);
    }

    // Actual bit-rate, since we are working with the frame time base, we need
    // to calculate the bit-rate needed for each frame. So with 1 frame comes
    // between T secs then the actual bit-rate (the frame bit-rate) is equal to
    // T*bit-rate.
    //
    // bit-rate (bit/s) ___ 1 (s)       <=> bit-rate (bit/s) ___ 1 (s)
    //    x (bit/frame) ___ T frame (s)        x (bit/frame) ___ 1/Hz (Frames/s)
    //
    int frame_bit_rate = bit_rate / (double) bitstream.get_fps(); //the bit rate per frame (b/s)
    int actual_bit_rate = frame_bit_rate *(double) bitstream.get_fps();                             //actual bit-rate, since the bit-rate per frame is a rounded value (b/s)
    double delay = buffer.get_length() / (double) bit_rate;                                     //delay with the original parameters (s)
    double actual_delay = buffer.get_length() / (double) actual_bit_rate;                           //actual delay (s)
    double actual_delay_frames = actual_delay * (double) bitstream.get_fps();                       //actual delay (s)

    if (arg_verbose >= VERBOSE_EXTRA)
    {
        std::cout << SEPARATOR_41 << std::endl;
        std::cout << "*\t\t\t\t VAR VALUES \t\t\t*" << std::endl;
        std::cout << SEPARATOR_41 << std::endl;
        std::cout << STDOUT_PARSE << "units: " << arg_unit << ";" << std::endl;
        if (arg_verbose >= VERBOSE_ALL)
        {
            std::cout << STDOUT_PARSE << "factor: " << factor << ";" << std::endl;
        }
        std::cout << STDOUT_PARSE << "FPS: " << bitstream.get_fps() << " (Hz);" << std::endl;
        std::cout << STDOUT_PARSE << "buffer length: " << buffer.get_length() << " (bits);" << std::endl;
        std::cout << STDOUT_PARSE << "buffer initial occupation: " << buffer.get_initial_occupation() << " (bits);" <<
                  std::endl;
        std::cout << STDOUT_PARSE << "bit-rate: " << bit_rate << " (bits/s);" << std::endl;
        std::cout << STDOUT_COMPUTE << "delay: " << delay << " (s);" << std::endl;
        if (arg_verbose >= VERBOSE_ALL)
        {
            std::cout << STDOUT_COMPUTE << "frame bit-rate: " << frame_bit_rate << " (bits/frame);" << std::endl;
        }
        std::cout << STDOUT_COMPUTE << "actual bit-rate: " << actual_bit_rate << " (bits/s);" << std::endl;
        std::cout << STDOUT_COMPUTE << "actual delay: " << actual_delay << " (s);" << std::endl;
        std::cout << STDOUT_COMPUTE << "actual delay: " << actual_delay_frames << " (frames);" << std::endl;
    }

    // 2.1 - Algorithm implementation
    int count = 0;
    std::vector<double> time_stamp;
    std::vector<long long int> bit_rate_in;
    std::vector<long long int> bit_rate_out;
    std::vector<long long int> buffer_occupation;
    std::vector<long long int> buffer_lengths;

    if (arg_verbose >= VERBOSE_ALL)
    {
        std::cout << SEPARATOR_41 << std::endl;
        std::cout << "*\t\t\t\t BUFFER STATUS \t\t\t*" << std::endl;
        std::cout << SEPARATOR_41 << std::endl;
    }
    while (true)
    {
        if (buffer.get_state() == State::overflow || buffer.get_state() == State::underflow)
        {
            if (arg_verbose >= VERBOSE_STANDARD)
            {
                std::cout << SEPARATOR_41 << std::endl;
                std::cout << "*\t\t\t\t BUFFER FAIL \t\t\t*" << std::endl;
                std::cout << SEPARATOR_41 << std::endl;
                std::cout << STDOUT_OUTPUT << "buffer (encoder) status: " <<
                          buffer.get_state_str() << ";" << std::endl;
                std::cout << STDOUT_OUTPUT << "buffer (encoder) fail frame: " <<
                    buffer.get_cur_frame() - 1 << ";" << std::endl;
                std::cout << STDOUT_OUTPUT << "buffer (encoder) fail time: " <<
                    (buffer.get_cur_frame() - 1)/ (double) bitstream.get_fps() << ";" << std::endl;
                std::cout << STDOUT_OUTPUT << "buffer (decoder) status: " <<
                          buffer.get_state_str_inverted() << ";" << std::endl;
                std::cout << STDOUT_OUTPUT << "buffer (decoder) fail frame: " <<
                    (buffer.get_cur_frame() - 1) + floor(actual_delay_frames) << ";" << std::endl;
                std::cout << STDOUT_OUTPUT << "buffer (decoder) fail time: " <<
                    ((buffer.get_cur_frame() - 1)/ (double) bitstream.get_fps()) + actual_delay << ";" << std::endl;
            }
            else if (arg_verbose >= VERBOSE_MINIMAL)
            {
                std::cout << buffer.get_state_str() << ": " << buffer.get_cur_frame() - 1 << ";" << std::endl;
            }
            else
            {
                return buffer;
            }
            break;
        }
        if (buffer.get_state() == State::done)
        {
            if (arg_verbose >= VERBOSE_STANDARD)
            {
                std::cout << SEPARATOR_41 << std::endl;
                std::cout << "*\t\t\t BUFFER SUCCESS \t\t\t*" << std::endl;
                std::cout << SEPARATOR_41 << std::endl;
                std::cout << STDOUT_OUTPUT << "buffer status: " << buffer.get_state_str() << ";" << std::endl;
                std::cout << STDOUT_OUTPUT << "processed frames: " << buffer.get_cur_frame() - 1 << ";" << std::endl;
                std::cout << STDOUT_OUTPUT << "empty buffer cycles: " << count - (buffer.get_cur_frame()) << ";" << std::endl;
                // TODO CONFIRM std::cout << STDOUT_OUTPUT << "empty time: " << (count-frame_idx) /
                //  (double) bitstream.get_fps() << " (s);" << std::endl;
            }
            else if (arg_verbose >= VERBOSE_MINIMAL)
            {
                std::cout << buffer.get_state_str() << ": " << buffer.get_cur_frame() - 1 << ";" << std::endl;
            }
            else
            {
                return buffer;
            }
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
        if (!arg_generate_csv.empty() || !arg_generate_plot.empty())
        {
            time_stamp.push_back(count/(double)bitstream.get_fps());
            count < bitstream.get_total_frames() ?
                    bit_rate_in.push_back(bitstream.get_frame(buffer.get_cur_frame() - 1).get_bits()) :
                    bit_rate_in.push_back(0);
            bit_rate_out.push_back(frame_bit_rate);
            buffer_occupation.push_back(buffer.get_occupation());
            buffer_lengths.push_back(buffer.get_length());
        }
    }

    if (!arg_generate_csv.empty() || !arg_generate_plot.empty())
    {
        time_t raw_time;
        tm* time_info;
        //plotdata_12341212121212.csv
        //123456789123456789123456789 > 27 chars > 30 chars
        char plt_csv_name[30];
        time_info = localtime(&raw_time);
        strftime(plt_csv_name, 30, "plotdata_%Y%m%d%H%M%S.csv", time_info);

        std::string csv_name = (!arg_generate_csv.empty()) ? arg_generate_csv : std::string(plt_csv_name) ;

        std::fstream output_file(csv_name, std::ios::out);
        if (!output_file.is_open())
        {
            std::cerr << STDERR_ERROR << "Unable to create " << csv_name << "!" << std::endl;
            throw FILE_UNABLE_TO_WRITE;
        }

        output_file << "time stamp (s)" << CSV_FILE_SEPARATOR << "bit in (bits)" << CSV_FILE_SEPARATOR <<
        "bits out (bits)" << CSV_FILE_SEPARATOR << "buffer occupation (bit)" << CSV_FILE_SEPARATOR <<
        "buffer length (bit)" << CSV_FILE_SEPARATOR << std::endl;
        for (size_t i = 0; i < time_stamp.size(); i++)
        {
            output_file << time_stamp[i] << CSV_FILE_SEPARATOR << bit_rate_in[i] << CSV_FILE_SEPARATOR <<
            bit_rate_out[i] << CSV_FILE_SEPARATOR << buffer_occupation[i] << CSV_FILE_SEPARATOR << buffer_lengths[i] <<
            CSV_FILE_SEPARATOR << std::endl;
        }
        output_file.close();

        if (!arg_generate_plot.empty())
        {
            system(PYTHON_COMMAND(csv_name, arg_generate_plot).c_str());
            if (arg_generate_csv.empty()) {
                remove(csv_name.c_str());
            }
        }
    }

    if (arg_verbose <= VERBOSE_MINIMAL) {
        return buffer;
    }
    // 2.2 - Average Bit-rate
    std::cout << SEPARATOR_41 << std::endl;
    std::cout << "*\t\t\t\t INPUT STATS \t\t\t*" << std::endl;
    std::cout << SEPARATOR_41 << std::endl;
    if (arg_verbose >= VERBOSE_EXTRA)
    {
        std::cout << STDOUT_OUTPUT << "sequence duration: " << bitstream.get_total_duration() << " (s);" << std::endl;
        std::cout << STDOUT_OUTPUT << "sequence frames: " << bitstream.get_total_frames() << ";" << std::endl;
        std::cout << STDOUT_OUTPUT << "sequence total bits: " << bitstream.get_total_bits() << " (bits);" << std::endl;
        std::cout << STDOUT_OUTPUT << "sequence total psnr: " << bitstream.get_total_psnr() << " (bits);" << std::endl;
        std::cout << STDOUT_OUTPUT << "frame avg bits: " <<
            bitstream.get_total_bits()/(double)bitstream.get_total_frames() << " (bits);" << std::endl;
        std::cout << STDOUT_OUTPUT << "frame avg psnr: " <<
            bitstream.get_total_psnr()/(double)bitstream.get_total_frames() << " (bits);" << std::endl;
    }
    std::cout << STDOUT_OUTPUT << "buffer input avg bit-rate: " <<
        bitstream.get_total_bits() / (double) bitstream.get_total_duration() << " (bits/s);" << std::endl;

    // 2.3 - Maximum and Minimum VBR Rate
    std::cout << SEPARATOR_41 << std::endl;
    std::cout << "*\t\t\t\t OUTPUT STATS \t\t\t*" << std::endl;
    std::cout << SEPARATOR_41 << std::endl;
    if (arg_verbose >= VERBOSE_EXTRA)
    {
        std::cout << STDOUT_OUTPUT << "sequence FPS: " << bitstream.get_fps() << " (Hz);" << std::endl;
        std::cout << STDOUT_OUTPUT << "frame smaller: " << bitstream.get_min_bits() << " (bits);" << std::endl;
        std::cout << STDOUT_OUTPUT << "frame avg: " << bitstream.get_total_bits() / (double) bitstream.get_total_frames() <<
                  " (bits);" << std::endl;
        std::cout << STDOUT_OUTPUT << "frame bigger: " << bitstream.get_max_bits() << " (bits);" << std::endl;
    }
    std::cout << STDOUT_OUTPUT << "buffer output min bit-rate: " <<
              round(bitstream.get_min_bits()*(double)bitstream.get_fps()) << " (bits/s);" << std::endl;
    // TODO CONFIRM std::cout << STDOUT_OUTPUT << "buffer output avg bit-rate: " <<
    //      round((bitstream.get_total_bits()/(double)bitstream.get_total_frames())*(double)bitstream.get_fps()) <<
    //      " (bits/s);" << std::endl;
    std::cout << STDOUT_OUTPUT << "buffer output max bit-rate: " <<
              round(bitstream.get_max_bits()*(double)bitstream.get_fps()) << " (bits/s);" << std::endl;

    // 2.4 - VBR Transmission
    // Implemented as another executable that call this function iteratively.
    return buffer;
}
