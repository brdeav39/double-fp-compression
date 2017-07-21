#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstring>
#include <math.h>
#include <map>

using namespace std;

int main() {

	cout << "\nEntropy program" << endl;
	cout << "-------------------------" << endl;
	cout << "(1) Entropy of bin file using XOR" << endl;
	cout << "(2) Entropy of bin file using differentials" << endl;
	cout << "\nEnter an option: ";
	int opt; cin >> opt;
        
	cout << "Enter name of file: ";
	string fName; cin >> fName;
	ifstream file(fName, ios::in|ios::binary|ios::ate);
	cout.setf(std::ios::unitbuf);
	if (file.is_open()) {
		file.seekg (0, ios::beg);
		unsigned long long prev;
		bool isFirst = true;
		map<unsigned long long, int> probDist;
		int count = 0;
		cout << "\ncalculating entropy";
		while(!file.eof()) {
			unsigned long long x;
			unsigned char * memblock = new unsigned char [8];
			if(file.read((char*)memblock, 8)) {
				memcpy(&x, memblock, 8);
				unsigned long long bit_gap;
				if(isFirst) {
					bit_gap = x;
				}
				else
					if(opt == 1)
						bit_gap = x ^ prev;
					else
						bit_gap = x - prev;
				probDist[bit_gap]++;

			}
		    prev = x;
		    if(count % 1000000 == 0)
		    	cout << ".";
		    count++;
			delete[] memblock;
		}

		//calculate entropy
		double entropy = 0.0;
		for (auto it : probDist) {
			double probability = (double) it.second/count;
			entropy -= probability * log2(probability);
		}
		cout << "finished." << endl;
		cout << "\nentropy: " << entropy << endl;

	}
	else 
		cout << "Unable to open file" << endl;

    return 0;
}
