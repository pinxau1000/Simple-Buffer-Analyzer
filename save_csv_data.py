if __name__ == "__main__":
    from sys import argv
    assert len(argv) == 3, "Error! Expecting 2 parameters (input_csv_name, output_png_name) got " + len(argv) + "!"
    
    from os.path import isfile
    assert isfile(argv[1]), "Error! File " + argv[1] + " didn't exist!"

    try: 
        from matplotlib import pyplot as plt
    except ImportError:
        raise ImportError("Please install matplotlib in order to save figures.")

    time_stamp = []
    bit_rate_in = []
    bit_rate_out = []
    buffer_occupation = []
    buffer_length = []
    with open(argv[1], 'r') as file_r:
        headers = file_r.readline().split(',')
        while True:
            line = file_r.readline()
            if not line:
                break

            [_time_stamp, _bit_rate_in, _bit_rate_out, _buffer_occupation, _buffer_length, _last_char] = line.split(',')

            time_stamp.append(float(_time_stamp))
            bit_rate_in.append(int(_bit_rate_in))
            bit_rate_out.append(int(_bit_rate_out))
            buffer_occupation.append(int(_buffer_occupation))
            buffer_length.append(int(_buffer_length))

    plt.figure()
    plt.plot(time_stamp, bit_rate_in, label="bits in (bit)")
    plt.plot(time_stamp, bit_rate_out, label="bits out (bit)")
    plt.plot(time_stamp, buffer_occupation, "--", label="buffer occupation (bit)")
    plt.plot(time_stamp, buffer_length, "--", label="buffer size (bit)")
    plt.title("Buffer Statistics")
    plt.legend()
    plt.xlabel("time (s)")
    plt.ylabel("bits (bit)")
    plt.grid(True)
    plt.savefig(argv[2])







