# Double FP Compression
###### Programs for researching various methods of compression on doubles

## How to run
Some data is contained in /data/ and is currently compressed by fpc.
These datasets and fpc source code were obtained from Martin
Burtscher's site http://cs.txstate.edu/~burtscher/research/FPC/
In order to decompress these datasets to obtain the raw data, do this:

**1. Compile fpc.c**

`gcc -O3 fpc.c -o fpc`

**2. Run fpc on compressed data**

`./fpc < dataset_name.trace.fpc > output_file_name.out`

**3. Run compression program**

`g++ -std=c++11 program.cpp`
