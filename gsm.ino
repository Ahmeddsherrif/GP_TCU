#include <SoftwareSerial.h>




const byte buttonaccident = 7;
const byte buttonsos = 8;
#define TX_PIN     3
#define RX_PIN     2
//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(TX_PIN, RX_PIN); //SIM800L Tx & Rx is connected to Arduino #3 & #2

#define uart_sendln       	mySerial.println  
#define uart_send           mySerial.print    
  
#define sim_uart_sendln   	Serial.println  
#define sim_uart_send       Serial.print   

#define uart_write          mySerial.write  
#define sim_uart_write      Serial.write  

#define uart_read          mySerial.read  
#define sim_uart_read      Serial.read  

#define uart_begin         mySerial.begin
#define sim_uart_begin     Serial.begin

#define uart_available     mySerial.available
#define sim_uart_available Serial.available


int phone_list[2] ={"01007351990","01129821431"};

char accident=0;
char sos=0;
char phone_list_counter=0;
char waiting_check=1;
char init_flag=0;
char Phone_Activity_Status_falg;

enum phone_enum{
Ringing,
call_in_progress,
Unknown,
Ready
};

void setup()
{
  config_baud_rate();
 
   pinMode(buttonaccident, INPUT_PULLUP);
   pinMode(buttonsos, INPUT_PULLUP);



 while (sim808_init()==0)
 {
   sim_uart_sendln("Sim808 init error\r\n");
          if(sim808_init()==1)
          {
            break;
          }
 }


}

void loop()
{
  updateSerial();
 
  
 
Ecall(); 


}

//functions 

void updateSerial()
{ char sim_read_flag,sim_read_flag2;
  delay(500);
  while (sim_uart_available()) 
  {sim_read_flag2=sim_uart_read();
        uart_write(sim_read_flag2);//Forward what Serial received to Software Serial Port
}
  while(uart_available()) 
  {
    sim_read_flag=(uart_read());
   sim_uart_write(sim_read_flag);//Forward what Software Serial received to Serial Port);//Forward what Software Serial received to Serial Port
  }
}

void callup(char * cellPhoneNumber )
{ 
uart_send("ATD");        
	uart_send(cellPhoneNumber);
	uart_send(";\r\n");


}

void send_sms (char * sms_message,char * cellPhoneNumber )
{
uart_sendln("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
 uart_send("AT+CMGS=\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
 uart_send(cellPhoneNumber);
uart_send("\"\r\n");
  updateSerial();
 uart_send(sms_message); //text content
  updateSerial();       
  uart_write(26);
  updateSerial();
  
}

void Read_SMS()
{

uart_sendln("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  uart_sendln("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
}

void config_baud_rate()
{ 

//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  sim_uart_begin(115200);
  
  //Begin serial communication with Arduino and SIM800L
  uart_begin(115200);
delay(1000);
  uart_sendln("AT+IPR=9600");
 updateSerial();

 //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  sim_uart_begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  uart_begin(9600);
delay(1000);
uart_sendln("AT+IPR=9600");
 updateSerial();
  
}

void answer_call()
{ 
  while(sendATcommand("ATA", "OK", 60000)==0)
  {
    
  }
  
}

void hangup_call()
{

while(sendATcommand("ATH", "OK", 2000)==0)
  {
    
  }
  
}


void speaker_mode()
{
  
 uart_sendln("ATM");
  
}

void automatic_answer()
{
updateSerial();

 uart_send("ATS0=1\r\n");
  
  updateSerial();

}

void close_automatic_answer()
{
updateSerial();

uart_send("ATS0=0\r\n");
  
  updateSerial();

}


int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout){

    uint8_t x=0,  answer=0;
    unsigned long previous;
    char readVar[200];
    char * auxChar;
    char response[200];


    memset(response, '\0', 200);    // Initialize the string
    memset(readVar, '\0', 200);    // Initialize the string

    while( mySerial.available() > 0) mySerial.read();    // Clean the input buffer
    while( Serial.available() > 0) Serial.read();    // Clean the input buffer
 
    mySerial.write(ATcommand);    // Send the AT command 
    mySerial.write("\r\n\r\n");    // Send enter
    
    Serial.println(ATcommand);
    
 
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(mySerial.available() != 0){    
            readVar[x] = mySerial.read();
            x++;
            // check if the desired answer is in the response of the module
            auxChar = strstr(readVar, expected_answer1);
            if (auxChar != NULL)    
            {
                if( strstr(readVar, "+CGPSINF:") == NULL)
                  strcpy (response, auxChar);
                else
                  strcpy (response, readVar);
    
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && ((millis() - previous) < timeout));  

    if(auxChar == NULL)
      Serial.println(readVar);
    
    return answer;
}

char sim808_init()
{
switch (sendATcommand("AT", "OK", 2000)) {
case 1: 
	    
		init_flag=1;
    break;
default:
  init_flag=0;
}
return init_flag;
}

phone_enum Phone_Activity_Status()
{ switch (sendATcommand("AT+CPAS", "+CPAS: 3", 200)) {
case 1://Ringing (MT is ready for commands from TA/TE, but the ringer is active)
  Phone_Activity_Status_falg=Ringing;
break;
case 0:
switch (sendATcommand("AT+CPAS", "+CPAS: 4",200)) {
case 1:// call in progress
  Phone_Activity_Status_falg=call_in_progress;
  break;
case 0:
  switch (sendATcommand("AT+CPAS", "+CPAS: 2", 200)) {
  case 1://Unknown (MT is not guaranteed to respond to instructions)
  Phone_Activity_Status_falg=Unknown;
  break;

    case 0:// Ready (MT allows commands from TA/TE)
    Phone_Activity_Status_falg=Ready;
    break;
  }
  
  }

}

return Phone_Activity_Status_falg;

}


int8_t check_command(char* expected_answer1, unsigned int timeout){

    uint8_t x=0,  answer=0;
    unsigned long previous;
    char readVar[200];
    char * auxChar;
    char response[200];


    memset(response, '\0', 200);    // Initialize the string
    memset(readVar, '\0', 200);    // Initialize the string

    while( mySerial.available() > 0) mySerial.read();    // Clean the input buffer
    while( Serial.available() > 0) Serial.read();    // Clean the input buffer
 
  
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(mySerial.available() != 0){    
            readVar[x] = mySerial.read();
            x++;
            // check if the desired answer is in the response of the module
            auxChar = strstr(readVar, expected_answer1);
            if (auxChar != NULL)    
            {
                if( strstr(readVar, "+CGPSINF:") == NULL)
                  strcpy (response, auxChar);
                else
                  strcpy (response, readVar);
    
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && ((millis() - previous) < timeout));  

    if(auxChar == NULL)
      Serial.println(readVar);
    
    return answer;
}

void Ecall ()
{
   accident=digitalRead(buttonaccident);
  sos=digitalRead(buttonsos);
  
  switch (accident&&sos) {
  case 1:
            while (Phone_Activity_Status()==Ringing)
{

hangup_call();

}
break;
default:
    // send_sms("SoS", "01129821431");
     
    while(waiting_check==1)
     {
       //callup("01129821431");
     
         if(check_command("+CLIP:", 2000)==1)
 { 
    if(check_phone_list()==1)
    {
      answer_call();
         while(check_command("NO CARRIER", 2000)==0) 
{    
  if(check_command("NO CARRIER", 2000)==1)
  {
         break;
  }
}     break;
      } 
else      
{
    hangup_call();
}
       }

   

   }}
   
   
 
}
char check_phone_list(){
  
    switch (check_command(phone_list[0],3000)) {
      case 1:
   phone_list_counter=1;
 break;
  case 0:
switch (check_command(phone_list[1],3000)) {
case 1:
 phone_list_counter=1;
 break;
  default:
  phone_list_counter=0;
  }
  }

  return phone_list_counter;
}
