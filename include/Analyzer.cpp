#include "Analyzer.hpp"

Buffer analyze(std::string arg_csv_file, int arg_bit_rate, int arg_buffer_length, int arg_init_buffer_length,
            int arg_fps, std::string arg_unit, std::string arg_generate_csv, std::string arg_generate_plot,
            int arg_verbose){

    // Parse units and get multiplier factor
    int factor = 1;
    if (arg_unit == "kb"){ factor = 1e3; }
    else if (arg_unit == "Mb"){ factor = 1e6; }
    else if (arg_unit == "B"){ factor = 8; }
    else if (arg_unit == "kB"){ factor = (1e3)/(double)8; }
    else if (arg_unit == "MB"){ factor = (1e6)/(double)8; }

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

    Buffer buffer(arg_buffer_length * (double) factor, bitstream.get_total_frames(), -1,
                  arg_verbose);   //the encoder buffer
    if (arg_init_buffer_length > 0){
        buffer.set_initial_occupation(arg_init_buffer_length*(double)factor);
    }

    // Actual bit-rate, since we are working with the frame time base, we need
    // to calculate the bit-rate needed for each frame. So with 1 frame comes
    // between T secs then the actual bit-rate (the frame bit-rate) is equal to
    // T*bit-rate.
    //
    // bit-rate (bit/s) ___ 1 (s)       <=> bit-rate (bit/s) ___ 1 (s)
    //    x (bit/frame) ___ T frame (s)        x (bit/frame) ___ 1/Hz (Frames/s)
    //
    int frame_bit_rate = round((arg_bit_rate * (double) factor) / (double) bitstream.get_fps()); //the bit rate per frame (b/s)
    int actual_bit_rate = frame_bit_rate *(double) bitstream.get_fps();                             //actual bit-rate, since the bit-rate per frame is a rounded value (b/s)
    double delay = buffer.get_length() / (double) arg_bit_rate;                                     //delay with the original parameters (s)
    double actual_delay = buffer.get_length() / (double) actual_bit_rate;                           //actual delay (s)
    double actual_delay_frames = actual_delay * (double) bitstream.get_fps();                       //actual delay (s)

    if (arg_verbose >= VERBOSE_EXTRA)
    {
        std::cout << SEPARATOR_41 << std::endl;
        std::cout << "*\t\t\t\t VAR VALUES \t\t\t*" << std::endl;
        std::cout << SEPARATOR_41 << std::endl;
        std::cout << PARSE << "units: " <<  arg_unit << ";" << std::endl;
        if (arg_verbose >= VERBOSE_EXTRA)
        {
            std::cout << PARSE << "factor: " <<  factor << ";" << std::endl;
        }
        std::cout << PARSE << "FPS: " <<  bitstream.get_fps() << " (Hz);" << std::endl;
        std::cout << PARSE << "buffer length: " <<  buffer.get_length() << " (bits);" << std::endl;
        std::cout << PARSE << "buffer initial occupation: " <<  buffer.get_initial_occupation() << " (bits);" <<
            std::endl;
        std::cout << PARSE << "bit-rate: " <<  arg_bit_rate*(double)factor << " (bits/s);" << std::endl;
        std::cout << COMPUTE << "delay: " <<  delay << " (s);" << std::endl;
        if (arg_verbose >= VERBOSE_EXTRA)
        {
            std::cout << COMPUTE << "frame bit-rate: " << frame_bit_rate << " (bits/frame);" << std::endl;
        }
        std::cout << COMPUTE << "actual bit-rate: " << actual_bit_rate << " (bits/s);" << std::endl;
        std::cout << COMPUTE << "actual delay: " << actual_delay << " (s);" << std::endl;
        std::cout << COMPUTE << "actual delay: " << actual_delay_frames << " (frames);" << std::endl;
    }

    // 2.1 - Algorithm implementation
    int count = 0;
    std::vector<double> time_stamp;
    std::vector<int> bit_rate_in;
    std::vector<int> bit_rate_out;
    std::vector<int> buffer_occupation;
    std::vector<int> buffer_length;

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
                std::cout << OUTPUT << "buffer (encoder) status: " <<
                    buffer.get_state_str() << ";" << std::endl;
                std::cout << OUTPUT << "buffer (encoder) fail frame: " <<
                    buffer.get_cur_frame() - 1 << ";" << std::endl;
                std::cout << OUTPUT << "buffer (encoder) fail time: " <<
                    (buffer.get_cur_frame() - 1)/bitstream.get_fps() << ";" << std::endl;
                std::cout << OUTPUT << "buffer (decoder) status: " <<
                    buffer.get_state_str_inverted() << ";" << std::endl;
                std::cout << OUTPUT << "buffer (decoder) fail frame: " <<
                    (buffer.get_cur_frame() - 1) + floor(actual_delay_frames) << ";" << std::endl;
                std::cout << OUTPUT << "buffer (decoder) fail time: " <<
                    ((buffer.get_cur_frame() - 1)/bitstream.get_fps()) + actual_delay << ";" << std::endl;
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
                std::cout << OUTPUT << "buffer status: " << buffer.get_state_str() << ";" << std::endl;
                std::cout << OUTPUT << "processed frames: " << buffer.get_cur_frame() - 1 << ";" << std::endl;
                std::cout << OUTPUT << "empty buffer cycles: " << count-(buffer.get_cur_frame()) << ";" << std::endl;
                // TODO CONFIRM std::cout << OUTPUT << "empty time: " << (count-frame_idx) /
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
            buffer_length.push_back(buffer.get_length());
        }
    }

    if (!arg_generate_csv.empty() || !arg_generate_plot.empty())
    {
        time_t raw_time;
        tm* time_info;
        char plt_csv_name[27];
        time_info = localtime(&raw_time);
        strftime(plt_csv_name, 14, "plotdata_%Y%m%d%H%M%S.csv", time_info);

        std::string csv_name = (!arg_generate_csv.empty()) ? arg_generate_csv : plt_csv_name ;

        std::fstream output_file(csv_name, std::ios::out);
        if (!output_file.is_open())
        {
            std::cerr << ERROR << "Unable to create " << csv_name << "!" << std::endl;
            throw FILE_UNABLE_TO_WRITE;
        }

        output_file << "time stamp (s)" << CSV_FILE_SEPARATOR << "bit in (bits)" << CSV_FILE_SEPARATOR <<
        "bits out (bits)" << CSV_FILE_SEPARATOR << "buffer occupation (bit)" << CSV_FILE_SEPARATOR <<
        "buffer length (bit)" << CSV_FILE_SEPARATOR << std::endl;
        for (size_t i = 0; i < time_stamp.size(); i++)
        {
            output_file << time_stamp[i] << CSV_FILE_SEPARATOR << bit_rate_in[i] << CSV_FILE_SEPARATOR <<
            bit_rate_out[i] << CSV_FILE_SEPARATOR << buffer_occupation[i] << CSV_FILE_SEPARATOR << buffer_length[i] <<
            CSV_FILE_SEPARATOR << std::endl;
        }
        output_file.close();

        if (!arg_generate_plot.empty())
        {
            std::string command = "python save_csv_data.py " + csv_name + " " + arg_generate_plot;
            system(command.c_str());
            if (arg_generate_csv.empty()) {
                remove(csv_name.c_str());
            }
        }
    }


    // 2.2 - Average Bit-rate
    std::cout << SEPARATOR_41 << std::endl;
    std::cout << "*\t\t\t\t INPUT STATS \t\t\t*" << std::endl;
    std::cout << SEPARATOR_41 << std::endl;
    if (arg_verbose >= VERBOSE_EXTRA)
    {
        std::cout << OUTPUT << "sequence duration: " << bitstream.get_total_duration() << " (s);" << std::endl;
        std::cout << OUTPUT << "sequence frames: " << bitstream.get_total_frames() << ";" << std::endl;
        std::cout << OUTPUT << "sequence total bits: " << bitstream.get_total_bits() << " (bits);" << std::endl;
        std::cout << OUTPUT << "sequence total psnr: " << bitstream.get_total_psnr() << " (bits);" << std::endl;
        std::cout << OUTPUT << "frame avg bits: " <<
            round(bitstream.get_total_bits()/(double)bitstream.get_total_frames()) << " (bits);" << std::endl;
        std::cout << OUTPUT << "frame avg psnr: " <<
            round(bitstream.get_total_psnr()/(double)bitstream.get_total_frames()) << " (bits);" << std::endl;
    }
    std::cout << OUTPUT << "buffer input avg bit-rate: " <<
        round(bitstream.get_total_bits()/(double)bitstream.get_total_duration()) << " (bits/s);" << std::endl;

    // 2.3 - Maximum and Minimum VBR Rate
    std::cout << SEPARATOR_41 << std::endl;
    std::cout << "*\t\t\t\t OUTPUT STATS \t\t\t*" << std::endl;
    std::cout << SEPARATOR_41 << std::endl;
    if (arg_verbose >= VERBOSE_EXTRA)
    {
        std::cout << OUTPUT << "sequence FPS: " << bitstream.get_fps() << " (Hz);" << std::endl;
        std::cout << OUTPUT << "frame smaller: " << bitstream.get_min_bits() << " (bits);" << std::endl;
        std::cout << OUTPUT << "frame avg: " <<
            round(bitstream.get_total_bits()/(double)bitstream.get_total_frames()) << " (bits);" << std::endl;
        std::cout << OUTPUT << "frame bigger: " << bitstream.get_max_bits() << " (bits);" << std::endl;
    }
    std::cout << OUTPUT << "buffer output min bit-rate: " <<
        round(bitstream.get_min_bits()*(double)bitstream.get_fps()) << " (bits/s);" << std::endl;
    // TODO CONFIRM std::cout << OUTPUT << "buffer output avg bit-rate: " <<
    //      round((bitstream.get_total_bits()/(double)bitstream.get_total_frames())*(double)bitstream.get_fps()) <<
    //      " (bits/s);" << std::endl;
    std::cout << OUTPUT << "buffer output max bit-rate: " <<
        round(bitstream.get_max_bits()*(double)bitstream.get_fps()) << " (bits/s);" << std::endl;

    // 2.4 - VBR Transmission
    // Implemented as another executable that call this function iteratively.
    return Buffer();
}
