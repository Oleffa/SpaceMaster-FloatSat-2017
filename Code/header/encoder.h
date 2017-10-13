/*
 * encoder.h
 *
 *  Created on: 05.02.2017
 *      Author: Oliver Struckmeier
 */

#ifndef HEADER_ENCODER_H_
#define HEADER_ENCODER_H_

#include "rodos.h"
#include "stdint.h"
#include <string>


class encoder{
public:
	encoder();
	void encoder_init();
	float encoder_read(float dt);
};
#endif /* HEADER_ENCODER_H_ */
