#ifndef MAIN_FRAME_HPP
#define MAIN_FRAME_HPP

#include <cmath>
#include <iostream>
#include "Definitions.hpp"

class Frame
{
private:
    double _psnr;
    int _bits;
public:
        Frame();
        ~Frame();
        double get_psnr();
        void set_psnr(double psnr);
        void set_bytes(double bytes);
        double get_bytes();
        int get_bits();
        void set_bits(int bits);
};
#endif //MAIN_FRAME_HPP
