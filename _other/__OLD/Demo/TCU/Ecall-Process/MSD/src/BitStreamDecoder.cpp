/*
 * BitStreamDecoder.cpp
 *
 *  Created on: Mar 5, 2023
 *      Author: Master
 */

#include <stdint.h>
#include <string>
#include <queue>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std;

#include "BitStreamDecoder.h"

#define GET_BIT(BYTE, BIT_NUMBER)	(((BYTE) >> (BIT_NUMBER))&1)

BitStreamDecoder::BitStreamDecoder() :
		currentBitIndex { 0 } {
	
}



uint32_t BitStreamDecoder::extract_bits(int startMSB, int numBits) {
	int endBit = startMSB + numBits - 1;

	int startByte = startMSB / 8;
	int endByte = endBit / 8;

	int startIndex = startMSB % 8;
	int endIndex;

	int currentBit;
	int shiftDecrement = 0;
	int totalShift;

	uint32_t result = 0;

	for (int i = startByte; i <= endByte; i++) {

		if (i == endByte) {
			endIndex = endBit % 8;
		} else {
			endIndex = 7;
		}

		for (int j = startIndex; j <= endIndex; j++) {
			currentBit = GET_BIT(rawDataByteVector[i], 7 - j);
			totalShift = numBits - 1 - shiftDecrement;
			result |= currentBit << totalShift;
			shiftDecrement++;
		}

		startIndex = 0;
	}

	return result;
}


void BitStreamDecoder::parse(string encodedString, const int *parseSequance, const uint8_t &structElements) {

	fillVector(encodedString);

	int startBit = 0;
	int bitLength;
	uint32_t structData;

	int i;
	for(i = 0; i<structElements; i++){
		bitLength = parseSequance[i];
		structData = extract_bits(startBit, bitLength);
		structElementsQueue.push(structData);
		startBit += bitLength;
	}

}

void BitStreamDecoder::fillVector(string encodedString) {
	string byteString;
	uint8_t byteData;

	for (size_t i = 0; i < encodedString.length(); i += 2) {
		byteString = encodedString.substr(i, 2);
		byteData = hexStringToUint8(byteString);
		rawDataByteVector.push_back(byteData);
	}
}

uint8_t BitStreamDecoder::hexStringToUint8(string hexString) {
	uint32_t result;
	stringstream ss;
	ss << hex << hexString;
	ss >> result;
	return static_cast<uint8_t>(result);
}

queue<uint32_t> BitStreamDecoder::getStructQueue() {
	return structElementsQueue;
}
