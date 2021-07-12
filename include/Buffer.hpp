#ifndef MAIN_BUFFER_H
#define MAIN_BUFFER_H

#include <cmath>
#include <iostream>
#include <string>
#include "Definitions.hpp"

enum class State {
    init, normal, final, overflow, underflow, done, invalid
};

class Buffer
{
private:
    long long int _length = 0;                      //length in bits
    long long int _initial_occupation = 0;         //initial occupation in bits
    long long int _occupation = 0;                 //current occupation in bits
    int _total_frames = 0;              //total frames to receive
    int _cur_frame_idx = 0;             //count the frame that entered the buffer (idx because starts at 0)
    State _state = State::init;         //buffer state (init, overflow, underflow, final)
    State _last_state = State::init;    //buffer last state (init, overflow, underflow, final)
    int _verbose = VERBOSE_STANDARD;
    void set_length(long long int length);
    void set_total_frames(int total_frames);
    void change_state(long long int in_bits, long long int out_bits);

public:
        Buffer();
        Buffer(long long int length, int total_frames, long long int initial_occupation, int verbose);
        ~Buffer();
        long long int get_length();
        long long int get_occupation();
        long long int get_initial_occupation();
        void set_initial_occupation(long long int initial_occupation);
        State get_state();
        std::string get_state_str();
        std::string get_state_str_inverted();
        State get_last_state();
        std::string get_last_state_str();
        int get_total_frames();
        int get_cur_frame();
        void in_out(long long int in_bits, long long int out_bits);
        void in(long long int in_bits);
        void out(long long int out_bits);

        static std::string get_state_str(State state)
        {
            switch (state)
            {
                case State::init:
                    return "init";
                case State::normal:
                    return "normal";
                case State::final:
                    return "final";
                case State::overflow:
                    return "overflow";
                case State::underflow:
                    return "underflow";
                case State::done:
                    return "done";
                default:
                    return "invalid";
            }
        }
};
#endif //MAIN_BUFFER_H
