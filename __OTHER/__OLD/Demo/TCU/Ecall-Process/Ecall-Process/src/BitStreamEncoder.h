#ifndef BITSTREAMENCODER_H_
#define BITSTREAMENCODER_H_

#define MSD_BUFFER_SIZE		100

class BitStreamEncoder  {

	public:
		BitStreamEncoder();
		void appendBits(uint32_t src, uint8_t length);
		string get_hex_string();

	private:
		uint8_t position;
		uint8_t bitIndex;
		uint8_t byteIndex;

		uint8_t msdAsByte[MSD_BUFFER_SIZE];
		void updateIndexes(uint8_t consumedBits);


};

#endif
