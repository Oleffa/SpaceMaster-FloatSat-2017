/*
 * lightsensor.h
 *
 *  Created on: 13.12.2016
 *      Author: Oliver Struckmeier
 *
 *      Old lightsensor h-file for the non thread lightsensor class
 */


#ifndef HEADER_LIGHTSENSOR_H_
#define HEADER_LIGHTSENSOR_H_

#include <rodos.h>
#include "stdint.h"
#include <string>

class LIGHT {
public:
	LIGHT();
	void init();
	void readLight(uint32_t* data);
};


#endif /* HEADER_LIGHTSENSOR_H_ */


