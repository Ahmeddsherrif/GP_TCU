#ifndef ENCODEPER_H_
#define ENCODEPER_H_

#include <iostream>
#include <cstring>
#include "MSD_data.h"

using namespace std;

class encoding{

private:

uint8_t byteIndex = 0;
uint8_t bitIndex = 8;
uint8_t position;
   /* convert character to actual encode value */
uint8_t getEncodeValue( char character);

void updateIndexes( uint8_t consumedBits );

/* serialize MSD bits into MSD buffer */
void appendBits( uint8_t *msdAsByte, uint32_t src, uint8_t length );

public:

uint8_t encodeMSD( tECallMessage *eCallMessage, uint8_t *msdAsByte );

};

#endif
