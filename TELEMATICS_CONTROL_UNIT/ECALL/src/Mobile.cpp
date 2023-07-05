/*
 * Mobile.cpp
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#include <iostream>

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <string>
#include <vector>

#include <thread>
#include <chrono>

using namespace std;

#include "error.h"
#include "configuration.h"
#include "debug.h"

#include "Serial.h"

#include "Mobile.h"



#define CMD_TEST 						("AT")

#define CMD_HANG_UP 					("ATH")
#define CMD_ANSWER 						("ATA")
#define CMD_SEND 						("ATH")
#define CMD_DIAL(PHONE_NUMBER) 			("ATD"+PHONE_NUMBER+';')

#define CMD_CONFIG_TEXT_MODE 			("AT+CMGF=1")
#define CMD_SMS_START(PHONE_NUMBER) 	("AT+CMGS=\"" + PHONE_NUMBER + '\"')
#define CMD_CLEAR_ALL_MESSEGES			("AT+CMGDA=\"DEL ALL\"")
#define CMD_GET_SMS(INDEX)				("AT+CMGR=" + INDEX)


#define RESPONSE_ACK		            ("0")
#define RESPONSE_RINGING 			    ("2")
#define RESPONSE_NO_CARRIER 			("3")
#define RESPONSE_NACK				    ("4")

#define RESPONSE_BREAK_SMS				("> ")
#define RESPONSE_NAN					("NaN")

#define INPUT_CMD_DIAL 					("DIAL")
#define INPUT_CMD_END 					("END")
#define INPUT_CMD_TERMINATE 			("EXIT")


#define TEXT_END_OF_EMERGENCY_INDICATION "KILL"




Error_t Mobile::begin(){
	Error_t rtnError = Error_t::NO_ERROR;

	latestSMS_Index = "-1";

	rtnError = mySerial.begin(DEVICE_FILE, DEVICE_BAUDRATE);

	rtnError = sendCMD(CMD_TEST);
	rtnError = recieveResponse();

	return rtnError;
}

void Mobile::clearAllSMS(){
	sendCMD(CMD_CLEAR_ALL_MESSEGES);
	recieveResponse();
}

Error_t Mobile::sendCMD(const string& CMD){
	Error_t rtnError = Error_t::NO_ERROR;

	rtnError = mySerial.dump();
	rtnError = mySerial.send(CMD);

	LOG("CMD SENT: " << CMD);

	return rtnError;
}


Error_t Mobile::sendSMS(const string& recipientPhoneNumber, const string& SMStext){
	Error_t rtnError = Error_t::NO_ERROR;

	char SUB = 26;
	string SMS_END;

	SMS_END += SUB;

	rtnError = sendCMD(CMD_CONFIG_TEXT_MODE);
	rtnError = recieveResponse();

	rtnError = sendCMD(CMD_SMS_START(recipientPhoneNumber));
	//rtnError = recieveResponse(RESPONSE_BREAK_SMS);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	rtnError = sendCMD(SMStext + SMS_END);
	rtnError = recieveResponse();

	return rtnError;
}

Error_t Mobile::readLatestSMS(vector<string>& SMS_Data){
	Error_t rtnError = Error_t::NO_ERROR;

	if (latestSMS_Index != "-1") {
		rtnError = sendCMD(CMD_GET_SMS(latestSMS_Index));
		rtnError = recieveResponse(SMS_Data);
	} else {
		rtnError = Error_t::UNEXPECTED_RESPONSE;
	}

	return rtnError;
}

Error_t Mobile::Call(const string& phoneNumber){
	Error_t rtnError = Error_t::NO_ERROR;

	rtnError = sendCMD(CMD_DIAL(phoneNumber));
	rtnError = recieveResponse();

	return rtnError;
}

Error_t Mobile::Hang(){
	Error_t rtnError = Error_t::NO_ERROR;

	rtnError = sendCMD(CMD_HANG_UP);
	rtnError = recieveResponse();

	return rtnError;
}


Error_t Mobile::Answer(){
	Error_t rtnError = Error_t::NO_ERROR;

	rtnError = sendCMD(CMD_ANSWER);
	rtnError = recieveResponse();

	return rtnError;
}

Error_t Mobile::recieveResponse(vector<string> &responseVector)
{
	Error_t rtnError = Error_t::NO_ERROR;
	string response;

	responseVector.clear();

	while (true) {
		mySerial.recieve(response);

		if (response != "") {

			responseVector.push_back(response);

			LOG("RESPONSE RECIEVED: " << response);

			if (response == RESPONSE_ACK) {
				rtnError = Error_t::NO_ERROR;
				break;
			} else if (response == RESPONSE_NACK) {
				rtnError = Error_t::MOBILE_CMD_NACK_ERROR;
				break;
			}
		}

	}

	return rtnError;
}



Error_t Mobile::recieveResponse()
{
	Error_t rtnError = Error_t::NO_ERROR;

	vector<string> dumpResponse;

	rtnError = recieveResponse(dumpResponse);

	return rtnError;
}


Error_t Mobile::PollLine(){
	Error_t rtnError = Error_t::NO_ERROR;

	rtnError = mySerial.recieve(polledResponse);

	if (polledResponse != "") {
		LOG("POLLED RESPONSE RECIEVIED:" << polledResponse);
	}

	return rtnError;
}

bool Mobile::isSMSRecieved(){
	bool rtnValue = false;

	if (polledResponse != "") {
		if (polledResponse.substr(0, 6).compare("+CMTI:") == 0) {
			latestSMS_Index = string { polledResponse.back() };
			rtnValue = true;
		}
	}

	return rtnValue;
}

bool Mobile::isRinging(){
	bool rtnValue = false;

	if (polledResponse != "") {
		if(polledResponse == RESPONSE_RINGING){
			rtnValue = true;
		}
	}

	return rtnValue;
}

bool Mobile::isNoCarrier() {
	bool rtnValue = false;

	if (polledResponse != "") {
		if (polledResponse == RESPONSE_NO_CARRIER) {
			rtnValue = true;
		}
	}

	return rtnValue;
}



