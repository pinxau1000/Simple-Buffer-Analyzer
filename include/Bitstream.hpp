#ifndef MAIN_BITSTREAM_HPP
#define MAIN_BITSTREAM_HPP

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <climits>
#include <limits.h> // Needed for windows compilation
#include "Definitions.hpp"
#include "Frame.hpp"

class Bitstream
{
private:
    std::vector<Frame> _bitstream;      //frames that are part of bitstream
    int _fps = 0;                       //frame rate of the bitstream (HZ)
    std::string _path = "";             //path of the bitstream
    int _frames = 0;                    //total frames of the bitstream
    double _duration = 0;               //total time of the bitstream (s)
    unsigned long long int _bits = 0;   //total of bits of the bitstream
    double _psnr = 0;                   //total of psnr of bitstream
    long long int _min_bits = LLONG_MAX;    //bits of the frame with lower bits
    long long int _max_bits = LLONG_MIN;    //bits of the frame with higher bits
    int _verbose = VERBOSE_STANDARD;
    void set_frames(int frames);
    void set_duration(double duration);

public:
        Bitstream();
        Bitstream(std::string path, int verbose);
        ~Bitstream();
        std::vector<Frame> get_bitstream();
        Frame get_frame(int idx);
        int get_fps();
        void set_fps(int fps);
        void parse_csv(std::string path);
        std::string get_path();
        int get_total_frames();
        double get_total_duration();
        long long int get_total_bits();
        double get_total_bytes();
        double get_avg_bits_frame();
        double get_avg_bytes_frame();
        double get_total_psnr();
        double get_avg_psnr();
        long long int get_min_bits();
        long long int get_max_bits();
};
#endif //MAIN_BITSTREAM_HPP
