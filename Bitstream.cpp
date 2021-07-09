#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <exception>
#include <cmath>
#include <climits>
#include "Frame.cpp"
#include "Definitions.hpp"

class Bitstream
{
private:
    std::vector<Frame> _bitstream;  //frames that are part of bitstream
    int _fps = 0;                   //frame rate of the bitstream (HZ)
    std::string _path = "";         //path of the bitstream
    int _frames = 0;                //total frames of the bitstream
    double _duration = 0;           //total time of the bitstream (s)
    int _bits = 0;                  //total of bits of the bitstream
    double _psnr = 0;               //total of psnr of bitstream
    int _min_bits = INT_MAX;        //bits of the frame with lower bits
    int _max_bits = INT_MIN;        //bits of the frame with higher bits

    int _verbose = VERBOSE_STANDARD;

    void set_frames(int frames)
    {
        if (frames < 1)
        {
            std::cerr << ERROR << "Bitstream length should be a positive integer. Got" << frames << " expected >0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _frames = frames;
    }

    void set_duration(double duration)
    {
        if (duration < 1)
        {
            std::cerr << ERROR << "Bitstream length should be a positive integer. Got" << duration << " expected >0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _duration = duration;
    }
    
public:
    Bitstream()
    {
    }

    Bitstream(std::string path, int verbose)
    {
        _verbose = verbose;
        parse_csv(path);
    }

    ~Bitstream()
    {

    }

    std::vector<Frame> get_bitstream()
    {
        return _bitstream;
    }

    Frame get_frame(int idx)
    {
        return _bitstream[idx];
    }

    int get_fps()
    {
        return _fps;
    }

    void set_fps(int fps)
    {
        if (fps < 1)
        {
            std::cerr << ERROR << "FPS should be a positive integer. Got" << fps << " expected >0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _fps = fps;
    }

    void parse_csv(std::string path)
    {
        std::fstream csv_file(path, std::ios::in);
        if (csv_file.fail())
        {
            std::cerr << ERROR << "File " << path << " not found!" << std::endl;
            throw FILE_NOT_FOUND_EXCEPTION;
        }

        for (std::string line; getline(csv_file, line); )
        {
            if (_fps == 0){
                try
                {
                    set_fps(stoi(line.substr(0, line.find(","))));
                }
                catch(const std::invalid_argument& e)
                {
                    std::cerr << ERROR << "Unable to parse Comma Separed File (CSV), expecting <fps>,0 \\n <bytes_0>,<psnr_0> \\n ... <bytes_n>,<psnr_n> got " << line << "!" << std::endl;
                    throw FILE_PARSE_ERROR;
                }
                if (_verbose >= VERBOSE_ALL)
                {
                    std::cout << PARSE << "fps: " << _fps << ";" << std::endl;
                }
            }
            else
            {

                Frame this_frame;
                try
                {
                    this_frame.set_bytes(stod(line.substr(0, line.find(","))));
                }
                catch(const std::invalid_argument& e)
                {
                    std::cerr << ERROR << "Unable to parse Comma Separed File (CSV), expecting <fps>,0 \\n <bytes_0>,<psnr_0> \\n ... <bytes_n>,<psnr_n> got " << line << "!" << std::endl;
                    throw FILE_PARSE_ERROR;
                }
                // Aditional bit metrics
                _bits += this_frame.get_bits();
                if (this_frame.get_bits() < _min_bits) { _min_bits = this_frame.get_bits(); }
                if (this_frame.get_bits() > _max_bits) { _max_bits = this_frame.get_bits(); }

                try
                {
                    this_frame.set_psnr(stof(line.substr(line.find(",")+1, line.length())));
                    _psnr += this_frame.get_psnr();
                }
                catch(const std::invalid_argument& e)
                {
                    std::cerr << ERROR << "Unable to parse Comma Separed File (CSV), expecting <fps>,0 \\n <bytes_0>,<psnr_0> \\n ... <bytes_n>,<psnr_n> got " << line << "!" << std::endl;
                    throw FILE_PARSE_ERROR;
                }

                if (_verbose >= VERBOSE_ALL)
                {
                    std::cout << PARSE << "bytes[" << _bitstream.size()+1 << "]: " << this_frame.get_bytes() << " - psnr[" << _bitstream.size()+1 << "]: " << this_frame.get_psnr() << ";" << std::endl;
                }

                _bitstream.push_back(this_frame);
            }
        }
        csv_file.close();
        
        _path = path;
        set_frames(_bitstream.size());
        set_duration(_frames/(double)_fps);

        if (_verbose >= VERBOSE_EXTRA)
        {
            std::cout << PARSE << "complete parsing: " << _path << ";" << std::endl;
            std::cout << PARSE << "total of frames: " << _frames << ";" << std::endl;
            std::cout << PARSE << "total of bits: " << _bits << " (bits);" << std::endl;
            std::cout << PARSE << "total of psnr: " << _psnr << " (dB);" << std::endl;
            std::cout << PARSE << "frame rate: " << _fps << " (Hz);" << std::endl;
        }
    }


    std::string get_path()
    {
        return _path;
    }

    int get_total_frames()
    {
        return _frames;
    }

    double get_total_duration()
    {
        return _duration;
    }

    int get_total_bits()
    {
        return round(_bits);
    }

    double get_total_bytes()
    {
        return _bits/(double)8;
    }

    int get_avg_bits_frame()
    {
        return round(_bits/(double)_frames);
    }

    double get_avg_bytes_frame()
    {
        return (_bits/(double)8)/(double)_frames;
    }

    double get_total_psnr()
    {
        return _psnr;
    }

    double get_avg_psnr()
    {
        return _psnr/(double)_frames;
    }

    int get_min_bits()
    {
        return _min_bits;
    }

    int get_max_bits()
    {
        return _max_bits;
    }
};
