#include "encodeper.h"

int main(){
encoding ENC;
int cnt;
tECallMessage ECallMessage;
uint8_t msd[140];
uint8_t msdsize;

memset(&ECallMessage, 0, sizeof(ECallMessage));
memset(msd, 0, sizeof(msd));

ECallMessage=MSD_cfg_data();
msdsize = ENC.encodeMSD(&ECallMessage, &msd[0]);

cout<<"MSD["<<unsigned (msdsize)<<"]: ";
for (cnt = 0; cnt < msdsize; cnt++)
printf("%.2X", msd[cnt]);

cout<<endl;


return 0;
}

