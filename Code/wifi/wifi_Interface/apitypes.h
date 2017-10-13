/*
 * apitypes.h
 *
 *  Created on: Nov 27, 2009
 *      Author: jere
 */

#ifndef APITYPES_H_
#define APITYPES_H_

#include <stdint.h>

typedef unsigned char  uint8;
typedef signed char   int8;
typedef unsigned short uint16;
typedef signed short   int16;
typedef unsigned long  uint32;
typedef signed long    int32;

typedef struct bd_addr_t
{
    unsigned char addr[6];

}bd_addr;

typedef bd_addr hw_addr;
typedef struct
{
    unsigned char len;
    unsigned char data[];
}uint8array;
typedef struct
{
    unsigned short len;
    unsigned char data[];
}uint16array;

typedef struct
{
    unsigned char len;
    signed char data[];
}string1;

typedef union
{
        uint32_t u;
        uint8_t  a[4];
}ipv4;
#endif /* APITYPES_H_ */
