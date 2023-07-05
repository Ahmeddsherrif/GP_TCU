/*
 * Utilities.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

class Utilities {

	public:
		static void parseCommaSepratedString(string &inputString,
				vector<string> &outputStringValues);

		static void deleteCharFromString(char c, string& str);
};

#endif /* UTILITIES_H_ */
