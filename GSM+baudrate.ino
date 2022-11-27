#include <SoftwareSerial.h>

#define phone_number    "01129821431"
#define sms_message     "L"

#define TX_PIN     3
#define RX_PIN     2
//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(TX_PIN, RX_PIN); //SIM800L Tx & Rx is connected to Arduino #3 & #2

void setup()
{
  config_baud_rate();

  Serial.println("Initializing...");
  delay(1000);
 
/* 
call the function u want to do one time
Read_SMS();
delay(5000);
send_sms();
delay(5000);
callup();
*/

callup();

}


void loop()
{
  updateSerial();

/* 
call the function u want to do more than one time
Read_SMS();
delay(5000);
send_sms();
delay(5000);
callup();
*/


}

void updateSerial()
{ 
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

void callup()
{

mySerial.print("ATD");
	mySerial.print(phone_number);
	mySerial.print(";\r\n");
updateSerial();

}

void send_sms ()
{

mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
 mySerial.print("AT+CMGS=\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
 mySerial.print(phone_number);
 mySerial.print("\"\r\n");
  updateSerial();
  mySerial.print(sms_message); //text content
  updateSerial();
  mySerial.write(26);
  updateSerial();
}

void Read_SMS()
{

mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
}

void config_baud_rate()
{

//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(115200);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(115200);
delay(1000);
  mySerial.println("AT+IPR=9600");
 updateSerial();

 //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
delay(1000);
  mySerial.println("AT+IPR=9600");
 updateSerial();
  
}


