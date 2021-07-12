# Simple Buffer Analyzer Tool


## Requirements
- Python and Matplotlib are required when generating plots. To install them run `python -m pip install --upgrade matplotlib numpy` on the cmd or terminal.
- A compatible operating system (OS). Available for Windows x64 and Linux x64.


## Install
1. [Download](https://github.com/pinxau1000/Simple-Buffer-Analyzer/releases) a executable compatible with your OS and installed python version.
   - Stable versions are tagged with the version number. Development versions are under [latest](https://github.com/pinxau1000/Simple-Buffer-Analyzer/releases/tag/latest) tag.

**Or**

2. You can also build the source files using CMAKE so that the installation fits your requirements.
   - **Dependencies Build Notes:**
   - TCLAP: https://sourceforge.net/p/tclap/code/ci/v1.2.4/tree/INSTALL
   

## Run
- Open the cmd or terminal and run the downloaded file.

exampe windows:
``` shell
main-window_amd64.exe --help
```

exampe ubuntu:
``` shell
main-ubuntu_amd64 --help
```


## Examples
Run analysis with on Terse_DieFirma_H261_64kbps with a bit-rate of 63920 bits, and a buffer with a size of 144856 bits and generating a csv and a plot file with the name output.
```shell
$ manual_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 63920 144856 -c output.csv -p output.png
```
The same command and above but with bit-rate and buffer size defined in kilo-Bytes (kB).
```shell
$ manual_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 7.99 18.107 -u kB -c output.csv -p output.png 
```
Outputting readable textual data to output.txt.
```shell
$ manual_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 63920 144855 -c output.csv -p output.png > output.txt
```
Outputting all readable textual data to output.txt. Increasing verbosity also increases the messages outputted.
```shell
$ manual_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 63920 144855 -c output.csv -p output.png -v 4 > output.txt
```
Overwrites the default initial buffer size parameter and sets it 20000 and also overwrites FPS parameter parsed from the bitstream file.
```shell
$ manual_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 63920 144855 -i 20000 -f 25 -c output.csv -p output.png -v 4 > output.txt
```
Iteratively computes the minimum buffer size some buffer initial size. To decrease the computation time of auto_buffer_size you may set a "learning rate" close to 1 (e.g. 0.95) with the drawback of some accuracy loss.
```shell
$ auto_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 63920 
```
When dealing with large sequences this auto_buffer_size may take a while to compute. Is recommended to set a high "learning rate" and decrease it progressively while setting the buffer initial value (seed) according to the previous output.
This way you may get accurate results after some iterations saving some time. The example bellow show this kind of operation.
```shell
$ auto_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 63920 72428 -r 0.95
*****************************************
*				 AUTO LENGTH 			*
*****************************************
compute	| minimal buffer length: 144859;
compute	| initial buffer length: 72428;
compute	| iterations: 85;
*****************************************
*			 BUFFER SUCCESS 			*
*****************************************
output	| buffer status: done;
output	| processed frames: 90000;
output	| empty buffer cycles: 51;
*****************************************
*				 INPUT STATS 			*
*****************************************
output	| buffer input avg bit-rate: 63919.6 (bits/s);
*****************************************
*				 OUTPUT STATS 			*
*****************************************
output	| buffer output min bit-rate: 37000 (bits/s);
output	| buffer output max bit-rate: 397800 (bits/s);


$ auto_buffer_size video_traces/Terse_DieFirma_H261_64kbps.txt 63920 72428 -s 144859 -r 0.1
*****************************************
*				 AUTO LENGTH 			*
*****************************************
compute	| minimal buffer length: 144856;
compute	| initial buffer length: 72428;
compute	| iterations: 860;
*****************************************
*			 BUFFER SUCCESS 			*
*****************************************
output	| buffer status: done;
output	| processed frames: 90000;
output	| empty buffer cycles: 51;
*****************************************
*				 INPUT STATS 			*
*****************************************
output	| buffer input avg bit-rate: 63919.6 (bits/s);
*****************************************
*				 OUTPUT STATS 			*
*****************************************
output	| buffer output min bit-rate: 37000 (bits/s);
output	| buffer output max bit-rate: 397800 (bits/s);


```

## Modifications and Contributions
You are allowed to modify this project and contribute to it. Just fork this project and work on it. Then if you want to contribute open a pull request (merge you work with the current project).
