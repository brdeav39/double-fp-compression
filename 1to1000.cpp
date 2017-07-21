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

int main() {

    //write 1 to 1000 with delta code to file in a csv format
    ofstream myFile;
    myFile.open ("1to1000.csv");
    for(int i = 1; i <= 1000; i++) {
        string n = encode(i);
        myFile << i << ", ";
        myFile << n << ", ";
        myFile << n.length() << ", ";
        myFile << decode(n) << endl;
    }
    cout << "file written successfully." << endl;
    myFile.close();
}	