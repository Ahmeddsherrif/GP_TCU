#include "main.h"

void state_sys_dead_handler_processing() {

}

void state_sys_idle_handler_processing() {

}

void state_sys_sos_handler_processing() {
}

void state_sys_call_handler_processing() {

}

void sendMSD() {
	MSDEncoder msd(CONFIG_FILE_DIR);
	string emergencyPhoneNumber = msd.getEmergencyPhoneNumber();

	// TODO: change them
	uint32_t passengers = 0;
	if(currentVarMSD.passenger_left == true){
		passengers ++;
	}
	if(currentVarMSD.passenger_right == true){
		passengers ++;
	}

	msd.setActivation(currentVarMSD.activation);
	msd.setThreatDetected(currentVarMSD.threatDetected);
	msd.setPositionLatitude(currentVarMSD.gpsLocation.latitude);
	msd.setPositionLongitude(currentVarMSD.gpsLocation.longitude);
	msd.setPositionTrust(currentVarMSD.PositionTrust);
	msd.setVehicleDirection(currentVarMSD.direction);
	msd.setNumberOfPassengers(passengers);

	string encodedMSDString = msd.encode();
	myMobile.sendSMS(emergencyPhoneNumber, encodedMSDString);
}
