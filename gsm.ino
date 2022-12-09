#include <SoftwareSerial.h>

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


typedef enum {
NO_SERVICE_AVAILABLE,
NO_ENOUGH_CHARGE,
BUSY,
OK
}SIM808_status;

char init_flag=0;
char Phone_Activity_Status_falg=4;
char Extended_Error_Report_falg =1;
void setup()
{
  config_baud_rate();
 
 while (sim808_init()==0)
 {
   sim_uart_sendln("Sim808 init error\r\n");
 }



  

}


void loop()
{
  updateSerial();





}

//functions 

void updateSerial()
{ 
  delay(500);
  while (sim_uart_available()) 
  {
    uart_write(sim_uart_read());//Forward what Serial received to Software Serial Port
  }
  while(uart_available()) 
  {
   sim_uart_write(uart_read());//Forward what Software Serial received to Serial Port
  }
}

SIM808_status callup(char * cellPhoneNumber )
{ char call_status; 
SIM808_status return_status = OK;
uart_send("ATD");
	uart_send(cellPhoneNumber);
	uart_send(";\r\n");
call_status = mySerial.read() ;

uart_send(call_status);

return return_status;

}

SIM808_status send_sms (char * sms_message,char * cellPhoneNumber )
{
SIM808_status return_status =OK;
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


  return return_status;

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

uart_sendln("ATA");
  
}

void hangup_call()
{

uart_sendln("ATH");
  
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

if(sendATcommand("AT", "OK", 2000) == 1){   
	    
		init_flag=1;
		
    }
else {
  init_flag=0;
}
return init_flag;
}

char Phone_Activity_Status()
{
if (sendATcommand("AT+CPAS", "+CPAS: 4", 2000)==1)   // call in progress
{
  Phone_Activity_Status_falg=0;
}
else if(sendATcommand("AT+CPAS", "+CPAS: 2", 2000)==1) //Unknown (MT is not guaranteed to respond to instructions)
{
  Phone_Activity_Status_falg=1;
}
else if(sendATcommand("AT+CPAS", "+CPAS: 3", 2000)==1) //Ringing (MT is ready for commands from TA/TE, but the ringer is active)
{
  Phone_Activity_Status_falg=3;
}
else{ Phone_Activity_Status_falg=4;  // Ready (MT allows commands from TA/TE)
}
  
return Phone_Activity_Status_falg;

}

char Extended_Error_Report()
{
if (sendATcommand("AT+CEER", "+CEER: Cause:17", 2000)==1)   //(user busy)
{
  Extended_Error_Report_falg=0;
}
else if(sendATcommand("AT+CEER", "+CEER: Cause:18", 2000)==1) //(no user responding)
{
  Extended_Error_Report_falg=1;
}
else if(sendATcommand("AT+CEER", "+CEER: Cause:19", 2000)==1) //(user alerting, no answer)
{
  Extended_Error_Report_falg=3;
}
else if (sendATcommand("AT+CEER", "+CEER: Cause:21", 2000)==1) //(call rejected)
  
  { Extended_Error_Report_falg=4;  
}

else if (sendATcommand("AT+CEER", "+CEER: Cause:38", 2000)==1) //(network out of order)
  
  { Extended_Error_Report_falg=5;  
}

else if (sendATcommand("AT+CEER", "+CEER: Cause:49", 2000)==1) //(quality of service unavailable)
  
  { Extended_Error_Report_falg=6;  
}

else if (sendATcommand("AT+CEER", "+CEER: Cause:63", 2000)==1) //(service or option not available, unspecified)
  
  { Extended_Error_Report_falg=7;  
}

else if (sendATcommand("AT+CEER", "+CEER: Cause:79", 2000)==1) //(service or option not implemented,unspecified)
  
  { Extended_Error_Report_falg=8;  
}
  
return Extended_Error_Report_falg;

}


