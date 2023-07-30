/*
 * debug.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ahmed
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG 1

#if DEBUG == 1

	#define LOG(LOG_MESSEGE)			cout << ">>>>>>> (LOG) " << LOG_MESSEGE << endl
	#define ERROR(ERROR_MESSEGE)		cerr << ">>>>>>> (ERROR) "<< ERROR_MESSEGE << endl
#else
	#define LOG(LOG_MESSEGE)
	#define ERROR(ERROR_MESSEGE)
#endif

#endif /* DEBUG_H_ */
