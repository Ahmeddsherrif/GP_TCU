#include <stdint.h>
#include <stack>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace std;

#include "BitStreamEncoder.h"

BitStreamEncoder::BitStreamEncoder() :
		position { 0 }, bitIndex { 8 }, byteIndex { 0 } {
	memset(msdAsByte, 0, MSD_BUFFER_SIZE);

}

string BitStreamEncoder::get_hex_string(){
	stringstream ss;
	ss << hex << setfill('0') << uppercase;
	for (uint8_t i = 0; i < byteIndex + 1; i++) {
		ss << setw(2) << static_cast<uint32_t>(msdAsByte[i]);
	}
	return ss.str();
}


void BitStreamEncoder::appendBits(uint32_t src, uint8_t length )
{
	position = bitIndex;
	if ( length <= position )
	{
		msdAsByte[byteIndex] |= src << ( position - length );
		updateIndexes ( length );
	}
	else
	{
		msdAsByte[byteIndex] |= ( src >> (length - position) ) & 0xFF;
		updateIndexes ( bitIndex );
		appendBits(src, length - position );
	}
}


void BitStreamEncoder::updateIndexes(uint8_t consumedBits) {
	if (!(consumedBits % 8)) {
		byteIndex += (consumedBits / 8);
	} else if (bitIndex <= consumedBits) {
		bitIndex = (8 - (consumedBits - bitIndex) % 8);
		byteIndex += ((consumedBits - bitIndex) / 8) + 1;
	} else {
		bitIndex -= consumedBits;
	}
}
