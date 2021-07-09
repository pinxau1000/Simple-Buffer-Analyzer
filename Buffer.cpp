#include <cmath>
#include <iostream>
#include <string>
#include "Definitions.hpp"

enum class State { init, normal, final, overflow, underflow, done };

class Buffer
{
private:
    int _length = 0;                    //length in bits
    int _initial_ocupation = 0;         //initial ocupation in bits
    int _ocupation = 0;                 //current ocupation in bits
    int _total_frames = 0;              //total frames to receive
    int _cur_frame_idx = 0;             //count the frame that entered the buffer (idx because starts at 0)
    State _state = State::init;         //buffer state (init, overflow, underflow, final)
    State _last_state = State::init;    //buffer last state (init, overflow, underflow, final)

    int _verbose = VERBOSE_STANDARD;

    void set_length(int length){
        if (length < 1)
        {
            std::cerr << ERROR << "Buffer length should be a positive integer. Got" << length << " expected >0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _length = length;
    }
    
    void set_total_frames(int total_frames){
        if (total_frames < 1)
        {
            std::cerr << ERROR << "Total frames should be a positive integer. Got" << total_frames << " expected >0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _total_frames = total_frames;
    }

    void change_state(int in_bits, int out_bits)
    {
        _last_state = _state;
        
        //Overflow condition
        if (_ocupation > _length)
        {
            _state = State::overflow;
        }
        //Underflow condition
        else if (_ocupation < 0)
        {
            //If the buffer is emptying then is not overflow!
            if (_state != State::final)
            {
                _state = State::underflow;
            }
            else
            {
                _state = State::done;
            }
        }
        else
        {
            //Initial ocupation filled the buffer -> change to Normal state
            if (_last_state == State::init && _ocupation > _initial_ocupation)
            {
                _state = State::normal;
            }
            //Normal State but no more frames to push in -> change to final state 
            else if (_last_state == State::normal && _cur_frame_idx == _total_frames)
            {
                _state = State::final;
            }
            //Final state and underflow -> change to done (this is implemented under the undeflow IF statement)
        }
    }

public:
    Buffer()
    {
    }

    Buffer(int length, int total_frames, int initial_ocupation = -1, int verbose = VERBOSE_STANDARD)
    {
        set_length(length);
        set_total_frames(total_frames);
        if (initial_ocupation < 0)
        {
            set_initial_ocupation(round((double) 0.5 * _length));
        }
        _verbose = verbose;
    }
    
    ~Buffer()
    {
    }

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
            return "invalid state";
        }
    }

    int get_length()
    {
        return _length;
    }

    int get_ocupation()
    {
        return _ocupation;
    }

    int get_initial_ocupation()
    {
        return _initial_ocupation;
    }

    void set_initial_ocupation(int initial_ocupation)
    {
        if (initial_ocupation > _length)
        {
            std::cerr << ERROR << "Buffer initital ocupation should be less than buffer total length. Got " << initial_ocupation << " expected <" << _length << "!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _initial_ocupation = initial_ocupation;
    }

    State get_state()
    {
        return _state;
    }

    std::string get_state_str()
    {
        return Buffer::get_state_str(_state);
    }

    State get_last_state()
    {
        return _last_state;
    }
    
    std::string get_last_state_str()
    {
        return Buffer::get_state_str(_last_state);
    }

    int get_total_frames()
    {
        return _total_frames;
    }

    int get_cur_frame()
    {
        return _cur_frame_idx + 1;
    }

    void in_out(int in_bits, int out_bits)
    {
        _ocupation = _ocupation + (in_bits - out_bits);
        _cur_frame_idx++;
        change_state(in_bits, out_bits);
        if (_verbose >= VERBOSE_ALL) {
            std::cout << COMPUTE << "buffer occupation: " << _ocupation << "/" << _length << " - in/out: " << in_bits << "/" << out_bits  << " - last/state: " << get_last_state_str() << "/" << get_state_str() << ";" << std::endl;
        }

    }

    void in(int in_bits)
    {
        _ocupation += in_bits;
        _cur_frame_idx++;
        change_state(in_bits, 0);
        if (_verbose >= VERBOSE_ALL) {
            std::cout << COMPUTE << "buffer occupation: " << _ocupation << "/" << _length << " - in/out: " << in_bits << "/0 - last/state: " << get_last_state_str() << "/" << get_state_str() << ";" << std::endl;
        }
    }

    void out(int out_bits)
    {
        _ocupation -= out_bits;
        change_state(0, out_bits);
        if (_verbose >= VERBOSE_ALL) {
            std::cout << COMPUTE << "buffer occupation: " << _ocupation << "/" << _length << " - in/out: 0/" << out_bits << " - last/state: " << get_last_state_str() << "/" << get_state_str() << ";" << std::endl;
        }
    }

};
