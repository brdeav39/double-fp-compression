#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>

using namespace std;

ifstream::pos_type size;

int main () {
  ifstream file("file.out", ios::in|ios::binary|ios::ate);
  if (file.is_open()) {
    	file.seekg (0, ios::beg);

      ofstream myFile;
      myFile.open ("fp_data.txt");
      cout << "begin writing to file..." << endl;
      int k = 0;
    	while(!file.eof()) {
          long long x;
          size = file.tellg();
          unsigned char * memblock = new unsigned char [size];
          if(file.read((char*)memblock, 8)) {
              memcpy(&x, memblock, 8);
          /*  x >>= 32;   shift right 32 */
          /*  x = x & 0x7FF;  mask to remove sign */
          }
          myFile << x << endl;
          delete[] memblock;
          k++;
          if(k > 200)
            break;

    	}

      cout << "finished writing to file 'fp_data.txt'" << endl;

    	myFile.close();
  }
  else 
      cout << "Unable to open file." << endl;

  return 0;
}
