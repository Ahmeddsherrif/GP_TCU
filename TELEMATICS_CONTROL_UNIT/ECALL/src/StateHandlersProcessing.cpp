#include "main.h"

void state_sys_dead_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void state_sys_idle_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void state_sys_sos_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void state_sys_call_handler_processing() {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void sendMSD() {
	MSDEncoder msd(CONFIG_FILE_DIR);
	string emergencyPhoneNumber = msd.getEmergencyPhoneNumber();

	// TODO: change them
	currentVarMSD.direction = 50;
	currentVarMSD.passengers = 2;

	msd.setActivation(currentVarMSD.activation);
	msd.setThreatDetected(currentVarMSD.threatDetected);
	msd.setPositionLatitude(currentVarMSD.gpsLocation.latitude);
	msd.setPositionLongitude(currentVarMSD.gpsLocation.longitude);
	msd.setPositionTrust(currentVarMSD.PositionTrust);
	msd.setVehicleDirection(currentVarMSD.direction);
	msd.setNumberOfPassengers(currentVarMSD.passengers);

	string encodedMSDString = msd.encode();
	myMobile.sendSMS(emergencyPhoneNumber, encodedMSDString);
}
