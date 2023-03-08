#include <iostream>
#include <stdint.h>
#include <queue>
#include <vector>
#include <string>

using namespace std;

#include "BitStreamEncoder.h"
#include "BitStreamDecoder.h"
#include "MSD.h"



int main() {

	MSD msd;

	msd.config();
	string EncodedMSDString = msd.encode();


	cout << "MSD: " << EncodedMSDString << endl;

	msd.decode(EncodedMSDString);

	msd.print();

	return 0;
}

