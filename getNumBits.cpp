

void getNumBits() {
	float ratio = 0;
	int totalBits = 0;
	for (int i = 1; i <= 100; i++) {
		int numBits = 0;
		int length = 0;
		int prefixLength = 0;

		//calculate bit-length of number from 1-100
		for (int j = i; j > 0; j >>= 1)
			length++;

		//calculate bit-length of each prefix
		for (int k = length; k > 0; k >>= 1)
			prefixLength++;

		numBits = (length - 1) + (prefixLength) + (prefixLength - 1); //bit-length + prefix bit-length + zeroes
		cout << "input: " << i << " has this many bits: " << numBits << endl;
		totalBits += numBits;
	}
	ratio = totalBits / 100;
	cout << ratio << endl;
	return;
}