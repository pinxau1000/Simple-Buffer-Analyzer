#include "Buffer.hpp"

void Buffer::set_length(int length){
    if (length < 1)
    {
        std::cerr << ERROR << "Buffer length should be a positive integer. Got" << length << " expected >0!" << std::endl;
        throw VALUE_ERROR_EXCEPTION;
    }
    _length = length;
}

void Buffer::set_total_frames(int total_frames){
    if (total_frames < 1)
    {
        std::cerr << ERROR << "Total frames should be a positive integer. Got" << total_frames << " expected >0!" << std::endl;
        throw VALUE_ERROR_EXCEPTION;
    }
    _total_frames = total_frames;
}

void Buffer::change_state(int in_bits, int out_bits)
{
    _last_state = _state;

    //Overflow condition
    if (_occupation > _length)
    {
        _state = State::overflow;
    }
        //Underflow condition
    else if (_occupation < 0)
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
        //Initial occupation filled the buffer -> change to Normal state
        if (_last_state == State::init && _occupation > _initial_occupation)
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

Buffer::Buffer()
{
}

Buffer::Buffer(int length, int total_frames, int initial_occupation = -1, int verbose = VERBOSE_STANDARD)
{
    set_length(length);
    set_total_frames(total_frames);
    if (initial_occupation < 0)
    {
        set_initial_occupation(round((double) 0.5 * _length));
    }
    _verbose = verbose;
}

Buffer::~Buffer()
{
}

int Buffer::get_length()
{
    return _length;
}

int Buffer::get_occupation()
{
    return _occupation;
}

int Buffer::get_initial_occupation()
{
    return _initial_occupation;
}

void Buffer::set_initial_occupation(int initial_occupation)
{
    if (initial_occupation > _length)
    {
        std::cerr << ERROR << "Buffer initital occupation should be less than buffer total length. Got " << initial_occupation << " expected <" << _length << "!" << std::endl;
        throw VALUE_ERROR_EXCEPTION;
    }
    _initial_occupation = initial_occupation;
}

State Buffer::get_state()
{
    return _state;
}

std::string Buffer::get_state_str()
{
    return Buffer::get_state_str(_state);
}

std::string Buffer::get_state_str_inverted()
{
    if (_state == State::overflow)
    {
        return Buffer::get_state_str(State::underflow);
    }
    if (_state == State::underflow)
    {
        return Buffer::get_state_str(State::overflow);
    }
    return "invalid state";
}

State Buffer::get_last_state()
{
    return _last_state;
}

std::string Buffer::get_last_state_str()
{
    return Buffer::get_state_str(_last_state);
}

int Buffer::get_total_frames()
{
    return _total_frames;
}

int Buffer::get_cur_frame()
{
    return _cur_frame_idx + 1;
}

void Buffer::in_out(int in_bits, int out_bits)
{
    _occupation = _occupation + (in_bits - out_bits);
    _cur_frame_idx++;
    change_state(in_bits, out_bits);
    if (_verbose >= VERBOSE_ALL) {
        std::cout << COMPUTE << "buffer occupation: " << _occupation << "/" << _length << " - in/out: " << in_bits << "/" << out_bits  << " - last/state: " << get_last_state_str() << "/" << get_state_str() << ";" << std::endl;
    }

}

void Buffer::in(int in_bits)
{
    _occupation += in_bits;
    _cur_frame_idx++;
    change_state(in_bits, 0);
    if (_verbose >= VERBOSE_ALL) {
        std::cout << COMPUTE << "buffer occupation: " << _occupation << "/" << _length << " - in/out: " << in_bits << "/0 - last/state: " << get_last_state_str() << "/" << get_state_str() << ";" << std::endl;
    }
}

void Buffer::out(int out_bits)
{
    _occupation -= out_bits;
    change_state(0, out_bits);
    if (_verbose >= VERBOSE_ALL) {
        std::cout << COMPUTE << "buffer occupation: " << _occupation << "/" << _length << " - in/out: 0/" << out_bits << " - last/state: " << get_last_state_str() << "/" << get_state_str() << ";" << std::endl;
    }
}
