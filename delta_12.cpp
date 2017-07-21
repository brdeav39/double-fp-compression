#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstring>
#include <math.h>

using namespace std;

string getPrefix(int n) {
	string prefix;
	int bitLength = 0;
	do
	{
		prefix.push_back('0' + (n & 1));
		bitLength++;
	} while (n >>= 1);
	reverse(prefix.begin(), prefix.end());

	string gamma;
	//add zeroes
	for (int i = 0; i < (bitLength - 1); i++)
		gamma.push_back('0');
	gamma.append(prefix);
	return gamma;
}

string encode(int x) {
	/*converts an integer to a string representation
	of its binary. Builds string backwards and uses reverse() */
	string s;
	int numBits = 0;
	do
	{
		s.push_back('0' + (x & 1));
		numBits++;
	} while (x >>= 1);
	reverse(s.begin(), s.end());

	//drops the leading 1
	s.erase(0, 1);

	//returns the prefix based on # of bits
	string result = getPrefix(numBits);
	result.append(s);
	return result;
}

int decode(string x) {
	//parse the zeroes
	int i = 0;
	while(x[i] == '0')
		i++;

	//calculate the length from the prefix
	int length = 0;
	for (int j = i; j >= 0; j--) {
		if(x[i] == '1')
			length += pow(2, j);
		i++;
	}

	length--;
	int result = pow(2, length);
	//calculate the result once the length is known
	for (int k = length - 1; k >= 0; k--) {
		if (x[i] == '1')
			result += pow(2, k);
		i++;
	}
	return result;
}

long long doOperation(unsigned long long x, unsigned long long y, bool isXor) {
	if(isXor)
		return x ^ y;
	else
		return x - y;
}

string encode12(bool isFirst, float bitCounts[], int bitGaps[], unsigned long bitMasks[],
unsigned long long k, unsigned long long prev = 0, bool isXor = false) {

	string result = "";
	for(int i = 0; i < 3; i++) {
		long long q, p, bit_gap;
		string bit_string;

		//divide the 32 MSB into their respective gaps
		if(i == 0) {
			q = k >> bitGaps[0];
			p = prev >> bitGaps[0];
		}
		else if(i == 1) {
			q = ((k >> bitGaps[1]) & bitMasks[0]);
			p = ((prev >> bitGaps[1]) & bitMasks[0]);
		}
		else if(i == 2) {
			q = ((k >> bitGaps[2]) & bitMasks[1]);
			p = ((prev >> bitGaps[2]) & bitMasks[1]);
		}

		if(!isFirst)
			bit_gap = doOperation(q, p, isXor);

		//encode the gaps based on xor or differntials
		if(!isXor) {
			if(bit_gap >= 1)
				bit_string = encode(bit_gap+1);
			else if(bit_gap <= 0)
				bit_string = ("1"+encode(abs(bit_gap)+1));
		}
		else
			bit_string = encode(bit_gap+1);

		//increment bit-counters (only needed for options 2 and 3)
		if(i == 0)
			bitCounts[0] += bit_string.length();
		else if(i == 1)
			bitCounts[1] += bit_string.length();
		else if(i == 2)
			bitCounts[2] += bit_string.length();

		//concatenate bit_strings
		result = result + bit_string;

		//if bitMasks == -1, program is running 12 MSB ONLY, therefore no masks
		if(bitMasks[0] == -1)
			break;
	}
	return result;
}

int main() {

	cout << "\nDelta compression program (12 MSB)" << endl;
	cout << "-------------------------" << endl;
	cout << "(1) Compress 12 MSB only" << endl;
	cout << "(2) Compress 32 MSB as [12, 12, 8]" << endl;
	cout << "(3) Compress 28 MSB as [12, 8, 8]" << endl;
	cout << "(4) Decode from terminal" << endl;
	cout << "(5) Encode from terminal" << endl;
	cout << "\nEnter an option: ";
	int opt; cin >> opt;
        
	if(opt == 1 || opt == 2 || opt == 3) {
		cout << "\n(1) Compress using XOR" << endl;
		cout << "(2) Compress using differentials" << endl;
		cout << "\nEnter an option: ";
		int optTwo; cin >> optTwo;
		cout << "\nEnter name of file to compress: ";
		string fName; cin >> fName;
		ifstream file(fName, ios::in|ios::binary|ios::ate);
		cout.setf(std::ios::unitbuf);
		if (file.is_open()) {
			file.seekg (0, ios::beg);
			string line;
			long long prev;
			bool isFirst = true;
			bool isXor = false;
			int count = 0;
			float totalBits = 0.0;
			string n;

			/*
			* bitCounts keeps running counter of # of bits in each segment
			* bitGaps stores values that mark of bit numbers of each segment
			* bitMasks stores the masks needed for each segment
				- bitMasks[0] is set to -1 as a check for option 1 (12 MSB only)
			*/
			float bitCounts[3] = {0.0, 0.0, 0.0};
			int bitGaps[3]; bitGaps[0] = 52;
			unsigned long bitMasks[2]; bitMasks[0] = -1;
			if(opt == 2) {
				bitGaps[1] = 40; bitGaps[2] = 32;
				bitMasks[0] = 0x000FFF; bitMasks[1] = 0x000000FF;
			}
			else if(opt == 3) {
				bitGaps[1] = 44; bitGaps[2] = 36;
				bitMasks[0] = 0x000FF; bitMasks[1] = 0x00000FF;
			}

			cout << "calculating";
			while(!file.eof()) {
				unsigned long long x;
				unsigned char * memblock = new unsigned char [8];
				if(file.read((char*)memblock, 8)) {
					memcpy(&x, memblock, 8);
					/*  x >>= 32;   shift right 32 */
					/*  x = x & 0x7FF;  mask to remove sign */
				    if(isFirst) {
						n = encode12(isFirst, bitCounts, bitGaps, bitMasks, x);			
						isFirst = false;
				    }
				    else {
				    	//set xor flag appropriately
				    	if(optTwo == 1)
				    		isXor = true;
				    	else
				    		isXor = false;
						n = encode12(isFirst, bitCounts, bitGaps, bitMasks, x, prev, isXor);
				    }
				}
			    prev = x;
			    if(count % 1000000 == 0)
			    	cout << ".";
			    count++;
			    totalBits = totalBits + n.length();
				delete[] memblock;
			}
			cout << "finished.\n~\n";

			//calculate rates & display results
			float rate = totalBits/count;
			bitCounts[0] = bitCounts[0]/count;
			bitCounts[1] = bitCounts[1]/count;
			bitCounts[2] = bitCounts[2]/count;

			cout << "first 12 bits-rate: " << bitCounts[0] << " bits per." << endl;
			if(opt == 1)
				rate = rate + 52;
			if(opt == 2) {
				cout << "next 12 bits-rate: ";
				rate = rate + 32;
			}
			else if(opt == 3) {
				cout << "next 8 bits-rate: ";
				rate = rate + 36;
			}
			cout << bitCounts[1] << " bits per." << endl;
			cout << "final 8 bits-rate: " << bitCounts[2] << " bits per." << endl;
			cout << "total rate: " << rate << " bits per double (64 bits)." << endl;

		}
		else 
			cout << "Unable to open file" << endl;
	}
	else if(opt == 4) {
		string deltaInput;
		cout << "Enter bit-string to be decoded: ";
		cin >> deltaInput;
		cout << "Decoded: " << decode(deltaInput) << endl;
	}
	else if(opt == 5) {
		long long input;
		cout << "Enter integer to be encoded: ";
		cin >> input;
		cout << "Encoded: " << encode(input) << endl;
	}

    return 0;
}
