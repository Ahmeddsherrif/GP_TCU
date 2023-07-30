/*
 * Mobile.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef MOBILE_H_
#define MOBILE_H_






class Mobile {
	public:
		Error_t begin();

		void clearAllSMS();

		Error_t sendSMS(const string& recipientPhoneNumber, const string& SMStext);
		Error_t readLatestSMS(vector<string>& SMS_Data);

		Error_t Call(const string& phoneNumber);
		Error_t Hang();
		Error_t Answer();

		Error_t PollLine();
		bool isSMSRecieved();
		bool isRinging();
		bool isNoCarrier();

	private:
		Serial mySerial;
		string polledResponse;
		string latestSMS_Index;

		Error_t sendCMD(const string &CMD);
		Error_t recieveResponse(vector<string> &responseVector);
		Error_t recieveResponse();
};

#endif /* MOBILE_H_ */
