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

string encode16(bool isFirst, unsigned long long k, unsigned long long prev = -1, bool isXor = false) {
	long long q, p, bit_gap;
	string bit_string;

	if(isFirst) {
		//take the first 16 bits
		bit_gap = k >> 48;

		//encode
		if(bit_gap >= 1)
			bit_string = encode(bit_gap+1);
		else if(bit_gap <= 0)
			bit_string = ("1"+encode(abs(bit_gap)+1));
	}
	else {
		q = k >> 48;
		p = prev >> 48;

		//take the gap between the first 16 bits and encode
		if(isXor) {
			bit_gap = q ^ p;
			bit_string = encode(bit_gap+1);
		}
		else {
 			bit_gap = q - p;
			if(bit_gap >= 1)
				bit_string = encode(bit_gap+1);
			else if(bit_gap <= 0)
				bit_string = ("1"+encode(abs(bit_gap)+1));
 		}
	}

	return bit_string;
}

int main() {
	cout << "\nDelta compression program (16 MSB)" << endl;
	cout << "----------------------------------" << endl;
	cout << "(1) Compress from file using XOR" << endl;
	cout << "(2) Compress from file using differentials" << endl;
	cout << "(3) Decode from terminal" << endl;
	cout << "(4) Encode from terminal" << endl;
	cout << "\nEnter an option: ";
	int opt; cin >> opt;
        
	if(opt == 1 || opt == 2) {
		cout << "Enter name of file to compress: ";
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
			cout << "calculating";
			while(!file.eof()) {
				unsigned long long x;
				unsigned char * memblock = new unsigned char [8];
				if(file.read((char*)memblock, 8)) {
					memcpy(&x, memblock, 8);
					/*  x >>= 32;   shift right 32 */
					/*  x = x & 0x7FF;  mask to remove sign */
				    if(isFirst) {
						n = encode16(isFirst, x);			
						isFirst = false;
				    }
				    else {
				    	if(opt == 1)
				    		isXor = true;
				    	else
				    		isXor = false;
						n = encode16(isFirst, x, prev, isXor);
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

			//calculate rate
			float rate = totalBits/count;
			cout << "total rate: " << rate << " bits per 16MSB of doubles." << endl;

		}
		else 
			cout << "Unable to open file" << endl;
	}
	else if(opt == 3) {
		string deltaInput;
		cout << "Enter bit-string to be decoded: ";
		cin >> deltaInput;
		cout << "Decoded: " << decode(deltaInput) << endl;
	}
	else if(opt == 4) {
		long long input;
		cout << "Enter integer to be encoded: ";
		cin >> input;
		cout << "Encoded: " << encode(input) << endl;
	}

    return 0;
}
