
/*_________________________________includes___________________________________________*/

#include <SoftwareSerial.h>

/*____________________________________________________________________________________*/

/* ________________________________pins identifications_______________________________*/

const byte buttonaccident = 7;
const byte buttonsos = 8;
#define TX_PIN     3
#define RX_PIN     2
//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(TX_PIN, RX_PIN); //SIM800L Tx & Rx is connected to Arduino #3 & #2

/* ____________________________________________________________________________________*/


/* ____________________________Arduino functions identifications_______________________*/


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

/* ___________________________________________________________________________________________*/

/*________________________________________enum & struct_______________________________________*/

enum State_mach {
Init_state,
Idle_state,
waiting_for_call_state,
answer_call_state
};

enum phone_enum{
Ringing,
call_in_progress,
Unknown,
Ready
};

struct minimum_setup_data{

char vehicle_registeration[30];
char vehicle_type[30];
char propulsion_type[30];
char position_data[30];
char time_of_the_incident[30];
 
};


/*________________________________________________________________________________________*/

/*_____________________________________configurations_____________________________________*/

#define no_of_phone_list 2
int phone_list[no_of_phone_list] ={"01007351990","01129821431"};


minimum_setup_data MSD_info=
{
"1339MN",
"Med_size car",
"ICEs",
"130,00,v",
"10:44"

};

/*________________________________________________________________________________________*/


/*___________________________variables declearations and macros___________________________*/

#define button_off 0
#define button_on  1
#define no_1  0
#define no_2  1

char accident=LOW;
char sos=LOW;
char phone_list_counter=LOW;
char waiting_check=HIGH;
char init_flag=LOW;
char Phone_Activity_Status_falg;
char currState = Init_state;
char sms_check=HIGH;

/*________________________________________________________________________________________*/

/*_______________________________________main_____________________________________________*/

void setup()
{   
  config_baud_rate();
 
   pinMode(buttonaccident, INPUT_PULLUP);
   pinMode(buttonsos, INPUT_PULLUP);


switch (currState){

/*************init state************/
  
  case Init_state:
 while (sim808_init()==LOW)
 {
   sim_uart_sendln("Sim808 init error\r\n");
          if(sim808_init()==HIGH)
          {
            break;
          }
 }
 currState=Idle_state;
 break;
 /**********************************/ 
 }

}

void loop()
{
  updateSerial();
 
Ecall();

}

/*______________________________________________________________________________________*/


/*________________________________________functions_____________________________________*/

char sim808_init()
{
switch (sendATcommand("AT", "OK", 2000)) {
case HIGH: 
	    
		init_flag=HIGH;
    break;
default:
  init_flag=LOW;
}
return init_flag;
}

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

int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout){

    uint8_t expected_answer_flag=LOW,  answer=LOW;
    unsigned long previous;
    char readVar[200];
    char * auxChar;
    char response[200];


    memset(response, '\0', 200);    // Initialize the string
    memset(readVar, '\0', 200);    // Initialize the string

    while( uart_available() > LOW) uart_read();    // Clean the input buffer
    while( sim_uart_available() > LOW) sim_uart_read();    // Clean the input buffer
 
    uart_write(ATcommand);    // Send the AT command 
    uart_write("\r\n\r\n");    // Send enter
    
    sim_uart_sendln(ATcommand);
    
 
    expected_answer_flag = LOW;
    previous = millis();

    // this loop waits for the answer
    do{
        if(uart_available() != LOW){    
            readVar[expected_answer_flag] = uart_read();
            expected_answer_flag++;
            // check if the desired answer is in the response of the module
            auxChar = strstr(readVar, expected_answer1);
            if (auxChar != NULL)    
            {
                if( strstr(readVar, "+CGPSINF:") == NULL)
                  strcpy (response, auxChar);
                else
                  strcpy (response, readVar);
    
                answer = HIGH;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == LOW) && ((millis() - previous) < timeout));  

    if(auxChar == NULL)
      sim_uart_sendln(readVar);
    
    return answer;
}

/*________________________________________________________________________________________*/


/*________________________________________actions_________________________________________*/

void answer_call()
{ 
  while(sendATcommand("ATA", "OK", 60000)==LOW)
  {
    
  }
  
}

void hangup_call()
{

while(sendATcommand("ATH", "OK", 2000)==LOW)
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

uart_send("ATS0=LOW\r\n");
  
  updateSerial();

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

void send_MSD_sms (char * sms_message1,char * sms_message2,char * sms_message3,char * sms_message4,char * sms_message5,char * cellPhoneNumber )
{


uart_sendln("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
 uart_send("AT+CMGS=\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
 uart_send(cellPhoneNumber);
uart_send("\"\r\n");
  updateSerial();
 uart_send(sms_message1); //text content
 uart_send("\"\r\n");
 uart_send(sms_message2); //text content
 uart_send("\"\r\n");
 uart_send(sms_message3); //text content
 uart_send("\"\r\n");
 uart_send(sms_message4); //text content
  uart_send("\"\r\n");
 uart_send(sms_message5); //text content
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



/*________________________________________________________________________________________*/


/*__________________________________________events________________________________________*/

char check_phone_list(){
  
    switch (check_command(phone_list[no_1],3000)) {
      case HIGH:
   phone_list_counter=HIGH;
 break;
  case LOW:
switch (check_command(phone_list[no_2],3000)) {
case HIGH:
 phone_list_counter=HIGH;
 break;
  default:
  phone_list_counter=LOW;
  }
  }

  return phone_list_counter;
}

phone_enum Phone_Activity_Status()
{ switch (sendATcommand("AT+CPAS", "+CPAS: 3", 200)) {
case HIGH://Ringing (MT is ready for commands from TA/TE, but the ringer is active)
  Phone_Activity_Status_falg=Ringing;
break;
case LOW:
switch (sendATcommand("AT+CPAS", "+CPAS: 4",200)) {
case HIGH:// call in progress
  Phone_Activity_Status_falg=call_in_progress;
  break;
case LOW:
  switch (sendATcommand("AT+CPAS", "+CPAS: 2", 200)) {
  case HIGH://Unknown (MT is not guaranteed to respond to instructions)
  Phone_Activity_Status_falg=Unknown;
  break;

    case LOW:// Ready (MT allows commands from TA/TE)
    Phone_Activity_Status_falg=Ready;
    break;
  }
  
  }

}

return Phone_Activity_Status_falg;

}

int8_t check_command(char* expected_answer1, unsigned int timeout){

    uint8_t expected_answer_flag=LOW,  answer=LOW;
    unsigned long previous;
    char readVar[200];
    char * auxChar;
    char response[200];


    memset(response, '\0', 200);    // Initialize the string
    memset(readVar, '\0', 200);    // Initialize the string

    while( uart_available() > LOW) uart_read();    // Clean the input buffer
    while( sim_uart_available() > LOW) sim_uart_read();    // Clean the input buffer
 
  
    expected_answer_flag = LOW;
    previous = millis();

    // this loop waits for the answer
    do{
        if(uart_available() != LOW){    
            readVar[expected_answer_flag] = uart_read();
            expected_answer_flag++;
            // check if the desired answer is in the response of the module
            auxChar = strstr(readVar, expected_answer1);
            if (auxChar != NULL)    
            {
                if( strstr(readVar, "+CGPSINF:") == NULL)
                  strcpy (response, auxChar);
                else
                  strcpy (response, readVar);
    
                answer = HIGH;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == LOW) && ((millis() - previous) < timeout));  

    if(auxChar == NULL)
     sim_uart_sendln(readVar);
    
    return answer;
}


/*________________________________________________________________________________________*/


/*___________________________________Ecall function_______________________________________*/




void Ecall(){

MSD_info.vehicle_registeration;
MSD_info.vehicle_type;
MSD_info.propulsion_type;
MSD_info.position_data;
MSD_info.time_of_the_incident;


 accident=digitalRead(buttonaccident);
  sos=digitalRead(buttonsos);

  
  switch(currState){

    /*************idle state**************/
    case Idle_state:
if((accident&&sos)!=button_off) 
   {   
            while (Phone_Activity_Status()==Ringing)
{

hangup_call();

} }

   else{   currState = waiting_for_call_state;}
   break;
    /*************************************************/

    /*************waiting for call state**************/
   
   case waiting_for_call_state:
     while (sms_check==HIGH){
         send_MSD_sms(MSD_info.vehicle_registeration,MSD_info.vehicle_type,MSD_info.propulsion_type,MSD_info.position_data,MSD_info.time_of_the_incident, phone_list[no_2]);   
         sms_check=LOW;
         break;
     }  
     while(waiting_check==HIGH)
     {
     
         if(check_command("+CLIP:", 2000)==HIGH)
 { 
    if(check_phone_list()==HIGH)
    {  currState =answer_call_state;
    }
    else      
{
    hangup_call();
}
break;
/************************************************/

/*******************answer call state************/
case answer_call_state:
      answer_call();
         while(check_command("NO CARRIER", 2000)==LOW) 
{    
  if(check_command("NO CARRIER", 2000)==HIGH)
  {      currState=Idle_state;
         break;
  }
}     break;
      } 
break;
/************************************************/

       }

  }

   }
   
/*________________________________________________________________________________________*/
