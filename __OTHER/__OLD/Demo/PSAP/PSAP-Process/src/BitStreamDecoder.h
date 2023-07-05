/*
 * BitStreamDecoder.h
 *
 *  Created on: Mar 5, 2023
 *      Author: Master
 */

#ifndef BITSTREAMDECODER_H_
#define BITSTREAMDECODER_H_

class BitStreamDecoder {
	public:
		BitStreamDecoder();
		void parse(string encodedString, const int *parseSequance, const uint8_t &structElements);
		queue<uint32_t> getStructQueue();
	private:
		int currentBitIndex;

		uint32_t extract_bits(int start_bit, int num_bits);
		void fillVector(string encodedString);
		uint8_t hexStringToUint8(string hexString);

		vector<uint8_t> rawDataByteVector;
		queue<uint32_t> structElementsQueue;

};

#endif /* BITSTREAMDECODER_H_ */
