/*
 * voltagesensor.h
 *
 *  Created on: 28.01.2017
 *      Author: Oliver Struckmeier
 */

#ifndef HEADER_VOLTAGESENSOR_H_
#define HEADER_VOLTAGESENSOR_H_

#include "rodos.h"
#include "HAL.h"

class voltagesensor: public Thread{
public:
	voltagesensor();
	void init();
	void run();
	void myInit();
};


#endif /* HEADER_VOLTAGESENSOR_H_ */
