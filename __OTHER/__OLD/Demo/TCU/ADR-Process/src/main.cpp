#include "main.h"

GPSLocation calculateDeadReckoningGPS(GPSLocation initialGPS, double distance, double bearing) {
	double lat1 = initialGPS.latitude * pi / 180.0; // convert to radians
	double lon1 = initialGPS.longitude * pi / 180.0;

	double heading = bearing * pi / 180.0; // convert to radians
	// Calculate new latitude using dead reckoning formula
	double lat2 = asin(sin(lat1) * cos(distance / earthRadius) + cos(lat1) * sin(distance / earthRadius) * cos(heading));
	// Calculate new longitude using dead reckoning formula
	double lon2 = lon1 + atan2(sin(heading) * sin(distance / earthRadius) * cos(lat1), cos(distance / earthRadius) - sin(lat1) * sin(lat2));

	// Convert back to degrees
	GPSLocation newGPS;
	newGPS.latitude = lat2 * 180.0 / pi;
	newGPS.longitude = lon2 * 180.0 / pi;
	return newGPS;
}

double calculateBearingAngle(GPSLocation fromGPS, GPSLocation toGPS) {
	double lat1 = fromGPS.latitude * pi / 180.0;
	double lon1 = fromGPS.longitude * pi / 180.0;
	double lat2 = toGPS.latitude * pi / 180.0;
	double lon2 = toGPS.longitude * pi / 180.0;

	double y = sin(lon2 - lon1) * cos(lat2);
	double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1);
	double bearing = atan2(y, x);

	// Convert bearing to degrees
	return bearing * 180.0 / pi;
}


double calculateDegreeDifference(double start_angle, double end_angle) {
    double diff = fmod(end_angle - start_angle + 540, 360) - 180;
    if (diff < -180) {
        diff += 360;
    }
    return diff;
}


double calculateQueueDifference(queue<double>& angle_queue) {
    double start_angle = angle_queue.front();
    double end_angle = angle_queue.back();

    //Clear Queue
	while (!angle_queue.empty()) {
		angle_queue.pop();
	}

    return calculateDegreeDifference(start_angle, end_angle);
}


double sumQueue(queue<double> &q) {
	double sum = 0.0;

	int count = q.size();

	while (!q.empty()) {
		sum += q.front();
		q.pop();
	}

	if (count == 0) {
		return 0.0;
	}

	return sum;
}






mutex isThreadStarted_mutex;
condition_variable isThreadStarted_conditionVariable;
bool isThreadStarted = false;


mutex isProgramFinished_mutex;
bool isProgramFinished = false;


enum class GPSThreadEvent_t{
		NO_OUTAGE,
		OUTAGE
};

GPSThreadEvent_t GPSThreadEvent = GPSThreadEvent_t::NO_OUTAGE;
mutex GPSThreadEventMutex;


enum class ADRThreadEvent_t{
		HALT,
		START
};
ADRThreadEvent_t ADRThreadEvent = ADRThreadEvent_t::HALT;
mutex ADRThreadEventMutex;



void IPC_thread(){
	string event;
	MessageQueue thisProcessesMessageQueue(ADR_PROCESS_MSGQ, ADR_PROCESS_MSGQ_KEY);
	while(true){

		unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
		if(isProgramFinished == true){
			break;
		}
		isProgramFinished_mutexLock.unlock();

		LOG("WAITING FOR EVENT....");
		thisProcessesMessageQueue.waitForMessage();
		event = thisProcessesMessageQueue.getLatestMessageString();
		LOG("EVENT RECIEVED: " << event);

		if (event == "KILL") {
			unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
			isProgramFinished = true;
			isProgramFinished_mutexLock.unlock();
		}

		else if (event == "OUTAGE") {
			unique_lock<mutex> GPSThreadEventMutexLock(GPSThreadEventMutex);
			GPSThreadEvent = GPSThreadEvent_t::OUTAGE;
			GPSThreadEventMutexLock.unlock();
		}

		else if (event == "NO_OUTAGE") {
			unique_lock<mutex> GPSThreadEventMutexLock(GPSThreadEventMutex);
			GPSThreadEvent = GPSThreadEvent_t::NO_OUTAGE;
			GPSThreadEventMutexLock.unlock();
		}


		//TODO: Make a condition variable for all thread synchronization
		else if (event == "START") {
			unique_lock<mutex> ADRThreadEventQueueMutexLock(ADRThreadEventMutex);
			ADRThreadEvent = ADRThreadEvent_t::START;
			ADRThreadEventMutex.unlock();
		}


	}

}


queue<double> speedometerSensorReadings_queue;
mutex speedometerSensorReadings_queueMutex;
constexpr int speedometerSampleRate = 4;	//40;
constexpr int maxSpeedSampleChange = 3;
void SpeedometerSensor_thread() {

	string fileName = string(DATA_DIR) + string("Speedx.csv");
	SpeedometerSensor speedometer(fileName);
	int sampleNumber = 0;
	const int maxSampleNumber = speedometer.getNumberOfSamples() - 1;
	double currentSpeedSample;
	double previousSpeedSample = speedometer.getSpeed(sampleNumber);

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	while (!isThreadStarted) {
		isThreadStarted_conditionVariable.wait(isThreadStarted_mutexLock);
	}
	isThreadStarted_mutexLock.unlock();

	// Define the start time of the loop
	auto startTime = high_resolution_clock::now();

	// Define the next time the loop should start
	auto nextLoopTime = startTime + milliseconds(speedometerSampleRate);

	while (true) {

		unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
		if (isProgramFinished == true) {
			break;
		}
		isProgramFinished_mutexLock.unlock();

		// Data Acquisition
		currentSpeedSample = speedometer.getSpeed(sampleNumber);

		// Data Cleaning
		if (abs(currentSpeedSample - previousSpeedSample) > maxSpeedSampleChange) {
			currentSpeedSample = previousSpeedSample;
		}

		previousSpeedSample = currentSpeedSample;

		unique_lock<mutex> speedometerSensorReadingsQueueMutexLock(speedometerSensorReadings_queueMutex);
		speedometerSensorReadings_queue.push(currentSpeedSample);
		speedometerSensorReadingsQueueMutexLock.unlock();

		sampleNumber++;

		if (sampleNumber >= maxSampleNumber) {
//			unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
//			isProgramFinished = true;
//			isProgramFinished_mutexLock.unlock();

			LOG("Speedometer Thread Ended");
			break;
		}

		// Sleep until the next loop period
		this_thread::sleep_until(nextLoopTime);

		// Calculate the next loop time
		nextLoopTime += milliseconds(speedometerSampleRate);
	}
}

queue<GPSReading> gpsSensorReadings_queue;
mutex gpsSensorReadings_queueMutex;
constexpr int gpsSampleRate = 100;	//1000;
void GPSSensor_thread() {

	string fileName = string(DATA_DIR) + string("Positionx.csv");
	GPSSensor gps(fileName);
	int sampleNumber = 0;
	const int maxSampleNumber = gps.getNumberOfSamples() - 1;
	GPSLocation currentGPSSample;
	GPSStatus currentGPSStatus = GPSStatus::AVAILABLE;
	GPSReading currentGPSReading;

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	while (!isThreadStarted) {
		isThreadStarted_conditionVariable.wait(isThreadStarted_mutexLock);
	}
	isThreadStarted_mutexLock.unlock();

	string event;

	GPSThreadEvent_t currentGPSThreadEvent;
	// Define the start time of the loop
	auto startTime = high_resolution_clock::now();

	// Define the next time the loop should start
	auto nextLoopTime = startTime + milliseconds(gpsSampleRate);

	while (true) {

		unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
		if (isProgramFinished == true) {
			break;
		}
		isProgramFinished_mutexLock.unlock();


		unique_lock<mutex> GPSThreadEventMutexLock(GPSThreadEventMutex);
		currentGPSThreadEvent = GPSThreadEvent;
		GPSThreadEventMutexLock.unlock();

		switch (currentGPSThreadEvent) {
			case (GPSThreadEvent_t::OUTAGE): {
				currentGPSStatus = GPSStatus::UNAVALIABLE;
				break;
			}
			case (GPSThreadEvent_t::NO_OUTAGE): {
				currentGPSStatus = GPSStatus::AVAILABLE;
				break;
			}
		}

		// Data Acquisition
		currentGPSSample = gps.getGPS(sampleNumber);
		currentGPSReading.Location = currentGPSSample;
		currentGPSReading.Status = currentGPSStatus;

		unique_lock<mutex> gpsSensorReadingsQueueMutexLock(gpsSensorReadings_queueMutex);
		gpsSensorReadings_queue.push(currentGPSReading);
		gpsSensorReadingsQueueMutexLock.unlock();

		sampleNumber++;

		if (sampleNumber >= maxSampleNumber) {
//			unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
//			isProgramFinished = true;
//			isProgramFinished_mutexLock.unlock();
			LOG("GPS Thread Ended");
			break;
		}

		// Sleep until the next loop period
		this_thread::sleep_until(nextLoopTime);

		// Calculate the next loop time
		nextLoopTime += milliseconds(gpsSampleRate);
	}
}

queue<double> gyroSensorReadings_queue;
mutex gyroSensorReadings_queueMutex;
constexpr int gyroSampleRate = 1;	//10;
constexpr double gyroStaticBias = 0;
void GyroSensor_thread() {

	string fileName = string(DATA_DIR) + string("Orientationx.csv");
	GyroscopeSensor gyro(fileName);
	int sampleNumber = 0;
	const int maxSampleNumber = gyro.getNumberOfSamples() - 1;
	double currentGyroSample;

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	while (!isThreadStarted) {
		isThreadStarted_conditionVariable.wait(isThreadStarted_mutexLock);
	}
	isThreadStarted_mutexLock.unlock();

	// Define the start time of the loop
	auto startTime = high_resolution_clock::now();

	// Define the next time the loop should start
	auto nextLoopTime = startTime + milliseconds(gyroSampleRate);

	while (true) {

		unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
		if (isProgramFinished == true) {
			break;
		}
		isProgramFinished_mutexLock.unlock();

		// Data Acquisition
		currentGyroSample = gyro.getYawRate(sampleNumber);

		unique_lock<mutex> gyroSensorReadingsQueueMutexLock(gyroSensorReadings_queueMutex);
		gyroSensorReadings_queue.push(currentGyroSample);
		gyroSensorReadingsQueueMutexLock.unlock();

		sampleNumber++;

		if (sampleNumber >= maxSampleNumber) {
//			unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
//			isProgramFinished = true;
//			isProgramFinished_mutexLock.unlock();
			LOG("Gyro Thread Ended");
			break;
		}

		// Sleep until the next loop period
		this_thread::sleep_until(nextLoopTime);

		// Calculate the next loop time
		nextLoopTime += milliseconds(gyroSampleRate);
	}
}

enum class State {
	GPS_ON, GPS_OFF
};




int main() {

	thread t1(IPC_thread);

	LOG("WAITING FOR START EVENT");
	while (true) {
		unique_lock<mutex> ADRThreadEventMutexLock(ADRThreadEventMutex);
		if(ADRThreadEvent == ADRThreadEvent_t::START){
			break;
		}
		ADRThreadEventMutexLock.unlock();

		this_thread::sleep_for(seconds(1));
	}
	LOG("PROGRAM STARTED ...");



	// iomanip
	cout << fixed << setprecision(6);
	cout << "Status,latitude,Longitude,SecondsSinceOutage" << endl;

	GPSReading currentADRReading;

	double averageSpeed;
	double averageYawRate;

	//Estimated Variables
	double currentEstimatedDistance = 0.0;
	double currentEstimatedBearingAngle = 0.0;
	GPSLocation currentEstimatedGPSLocation = { 0.0, 0.0 };

	//True Variables
	GPSReading currentGPSReading;
	GPSStatus currenGPSStatus = GPSStatus::AVAILABLE;
	GPSLocation currentTrueGPSSample = { 0, 0 };
	GPSLocation previousTrueGPSSample = { 0, 0 };
	bool newGPSReadingFlag = false;
	double currentTrueBearingAngle = 0.0;



	MessageQueue communicationMangerMsgQ(COMM_PROCESS_MSGQ, COMM_PROCESS_MSGQ_KEY);

	State state = State::GPS_ON;


	thread t2(SpeedometerSensor_thread);
	thread t3(GyroSensor_thread);
	thread t4(GPSSensor_thread);

	//Wait until all threads initialize and block
	this_thread::sleep_for(seconds(1));

	unique_lock<mutex> isThreadStarted_mutexLock(isThreadStarted_mutex);
	isThreadStarted = true;
	isThreadStarted_conditionVariable.notify_all();
	isThreadStarted_mutexLock.unlock();

	int secondsSinceOutage = 0;
	while (true) {

		unique_lock<mutex> isProgramFinished_mutexLock(isProgramFinished_mutex);
		if (isProgramFinished == true) {
			Message message;

			memset(&message, 0, sizeof(message));

			message.id = 11;
			strcpy(message.data, getGPSString(currentADRReading).c_str());
			communicationMangerMsgQ.sendMessage(message);
			this_thread::sleep_for(seconds(1000));
			break;
		}
		isProgramFinished_mutexLock.unlock();

		unique_lock<mutex> gpsSensorReadingsQueue_MutexLock(gpsSensorReadings_queueMutex);
		if (gpsSensorReadings_queue.empty() == false) {
			currentGPSReading = gpsSensorReadings_queue.front();
			currentTrueGPSSample = currentGPSReading.Location;
			currenGPSStatus = currentGPSReading.Status;
			gpsSensorReadings_queue.pop();
			newGPSReadingFlag = true;

			if (currenGPSStatus == GPSStatus::AVAILABLE) {
				state = State::GPS_ON;
			} else {
				state = State::GPS_OFF;
			}

			currentADRReading.Status = currenGPSStatus;
		}
		gpsSensorReadingsQueue_MutexLock.unlock();

		if (newGPSReadingFlag == true) {
			newGPSReadingFlag = false;

			unique_lock<mutex> speedometerSensorReadingsQueueMutexLock(speedometerSensorReadings_queueMutex);
			if (speedometerSensorReadings_queue.empty() == false) {
				averageSpeed = sumQueue(speedometerSensorReadings_queue);
			}
			speedometerSensorReadingsQueueMutexLock.unlock();

			unique_lock<mutex> gyroSensorReadingsQueueMutexLock(gyroSensorReadings_queueMutex);
			if (gyroSensorReadings_queue.empty() == false) {
				averageYawRate = calculateQueueDifference(gyroSensorReadings_queue);
			}
			gyroSensorReadingsQueueMutexLock.unlock();

			//Always Calculate Bearing -- FOr Demo Perposes only
			currentTrueBearingAngle = calculateBearingAngle(previousTrueGPSSample, currentTrueGPSSample);
			previousTrueGPSSample = currentTrueGPSSample;



			switch (state) {
				case (State::GPS_ON): {
					secondsSinceOutage = 0;
					currentEstimatedBearingAngle = currentTrueBearingAngle;
					currentEstimatedGPSLocation = currentTrueGPSSample;
					currentADRReading.Location = currentTrueGPSSample;


					break;
				}
				case (State::GPS_OFF): {
					secondsSinceOutage++;
					currentEstimatedDistance = KMH_TO_MS(averageSpeed) * MILLISECONDS_TO_SECONDS((double )40);
					currentEstimatedBearingAngle += averageYawRate;
					currentEstimatedGPSLocation = calculateDeadReckoningGPS(currentEstimatedGPSLocation, currentEstimatedDistance,
					currentEstimatedBearingAngle);


					currentADRReading.Location = currentEstimatedGPSLocation;

					break;
				}
			}



			cout << getGPSString(currentADRReading) <<"," << secondsSinceOutage << " seconds"<< endl;


		}
	}

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	return 0;
}
