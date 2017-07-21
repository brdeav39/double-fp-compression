#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstring>
#include <math.h>

using namespace std;

/*

*This program simply counts the number of bits required
*to run GVI compression on a dataset. It doesn't actually
*do the compression and is only used for calculating
*compression rate

*/

int gvi(unsigned long x) {
	int n = 32;
	int numZeroes;
	unsigned y;

	//calculate leading zero count
	y = x >> 16; if(y != 0) {n = n - 16; x = y;}
	y = x >> 8; if(y != 0) {n = n - 8; x = y;}
	y = x >> 4; if(y != 0) {n = n - 4; x = y;}
	y = x >> 2; if(y != 0) {n = n - 2; x = y;}
	y = x >> 1; 
	if(y != 0) {numZeroes = (n - 2);}
	else {numZeroes = (n - x);}

	//now do the "packing" - return the number of bits required
	if(numZeroes >= 24)
		return 10;
	else if(numZeroes >= 16)
		return 18;
	else if(numZeroes >= 8)
		return 26;
	else
		return 34;
}

int main() {

	cout << "\ngroup varint compression program" << endl;
	cout << "-------------------------" << endl;
	cout << "(1) Input from bin file using XOR" << endl;
	cout << "(2) Input from bin file using differentials" << endl;
	cout << "\nEnter an option: ";
	int opt; cin >> opt;
        
	cout.setf(std::ios::unitbuf);
	cout << "Enter name of file: ";
	string fName; cin >> fName;
	ifstream file(fName, ios::in|ios::binary|ios::ate);
	if (file.is_open()) {
		file.seekg (0, ios::beg);
		string line;
		long prev;
		bool isFirst = true;
		int count = 0;
		float totalBits = 0.0;
		int b;
		cout << "writing results to file";
		while(!file.eof()) {
			unsigned long long x;
			long n, k;
			unsigned char * memblock = new unsigned char [8];
			if(file.read((char*)memblock, 8)) {
				memcpy(&x, memblock, 8);

				//save the 32MSB into a long for gvi purposes
				n = x >> 32;
			    if(isFirst) {
			    	k = n;
					totalBits += gvi(k);			
					isFirst = false;
			    }
			    else {
			    	if(opt == 1) {
			    		k = n ^ prev;
			    		totalBits += gvi(k);
			    	}
			    	else {
			    		k = n - prev;
			    		if(n < 0) {
			    			totalBits += gvi(abs(k));
			    		}
			    		else {
			    			totalBits += gvi(k+1);
			    		}
			    		totalBits++;	//increment for sign bit
			    	}
			    }
			}
		    prev = n;
		    if(count % 1000000 == 0)
		    	cout << ".";
		    count++;
			delete[] memblock;
		}
		cout << "finished.\n~\n";

		//calculate rate
		float rate = totalBits/count;
		rate = rate + 32;
		cout << "total rate: " << rate << " bits per double (64 bits)." << endl;

	}
	else 
		cout << "Unable to open file" << endl;

	return 0;
}