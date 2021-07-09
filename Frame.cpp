class Frame
{
private:
    double _psnr;           //psnr of the frame
    int _bits;              //Number of bits that encode the frame

public:
    Frame()
    {
    }

    ~Frame()
    {
    }

    double get_psnr()
    {
        return _psnr;
    }

    void set_psnr(double psnr)
    {
        _psnr = psnr;
    }

    void set_bytes(double bytes)
    {
        if (bytes < 1)
        {
            std::cerr << ERROR << "Bytes should be a positive double. Got " << bytes << " expected >0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        if (fmod(bytes*(double)8, 1) != 0)
        {
            std::cerr << ERROR << "Bytes should be a multiple of 8. Got " << bytes << " expected (bytes*8)%1==0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _bits = bytes*8;
    }
    
    double get_bytes()
    {
        return _bits/(double)8;
    }

    int get_bits()
    {
        return _bits;
    }

    void set_bits(int bits)
    {
        if (bits < 1)
        {
            std::cerr << ERROR << "Bits should be a positive integer. Got" << bits << " expected >0!" << std::endl;
            throw VALUE_ERROR_EXCEPTION;
        }
        _bits = bits;
    }
};
