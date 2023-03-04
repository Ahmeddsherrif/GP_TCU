#ifndef DECODERPER_H_
#define DECODERPER_H_


#include <bits/stdc++.h>
using namespace std;

class decoding{

private:

int decode(string msdencoded,char start,char len);
string automaticActivation(string eCallMessage);
string testCall(string eCallMessage);
string positionCanBeTrusted(string eCallMessage);
string compressedNaturalGas(string eCallMessage);
string vehicleType(string eCallMessage);
string dieselTankPresent(string eCallMessage);
string electricEnergyStorage(string eCallMessage);
string gasolineTankPresent(string eCallMessage);



public:

void structure(string eCallMessage);

};




#endif
