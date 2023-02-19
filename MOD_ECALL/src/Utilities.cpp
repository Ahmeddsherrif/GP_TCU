/*
 * Utilities.cpp
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#include <string>
#include <sstream>
#include <vector>

using namespace std;

#include "Utilities.h"

void Utilities::parseCommaSepratedString(string &inputString, vector<string>& outputStringValues) {
	istringstream iss(inputString);
	string token;
	int i = 0;
	while (getline(iss, token, ',')) {
		outputStringValues.push_back(token);
		i++;
	}
}

void Utilities::deleteCharFromString(char c, string &str) {
	size_t index = 0;
	while (true) {
		index = str.find(c, index);

		if (index != string::npos) {
			str.erase(index, 1);
		} else {
			break;
		}
	}
}
