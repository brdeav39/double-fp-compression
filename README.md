# Double FP Compression
###### Programs for researching various methods of compression on doubles

## How to run
All data is contained in /data/ and is currently compressed by fpc.
In order to decompress these files to obtain the raw data, do this:

1. Compile fpc.c
`cc -O3 fpc.c -o fpc`

2. Run fpc on compressed data
`./fpc < program_name.trace.fpc > output_file_name.out`

3. Run compression program
`g++ -std=c++11 program.cpp`
