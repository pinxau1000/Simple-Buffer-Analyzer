if __name__ == "__main__":
    import sys
    
    try:
        file_r = open(sys.argv[1], 'r')
        file_w = open(sys.argv[2], 'w')
        fps = sys.argv[3]
    except IndexError:
        raise AttributeError("Expected <input-file> <output-file> <fps>!")
    
    count = 0
    frames2convert = None

    print(f"Input: {sys.argv[1]}")
    print(f"Outut: {sys.argv[2]}")
    print(f"FPS: {sys.argv[3]}")
    if len(sys.argv) > 4:
        frames2convert = int(sys.argv[4])
        print(f"Frames 2 Convert: {sys.argv[4]}")

    file_w.write(f"{fps},0\n")
    file_w.close()
    file_w = open(sys.argv[2], 'a')

    while True:
        line = file_r.readline()

        if not line.strip().startswith("#"):
            try:
                [frame_time, frame_type, frame_bytes] = line.strip().split()
                file_w.write(f"{frame_bytes},0\n")
                count+=1
            except ValueError:
                pass
        
        if frames2convert:
            if count >= frames2convert:
                break

        # if line is empty
        # end of file is reached
        if not line:
            break
    
    file_r.close()
    file_w.close()
    print(f"Done. File saved to {sys.argv[2]}.")
