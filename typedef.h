#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include "stdio.h"
#include <intrins.h>
#include <string.h>
#include "reg51.h"
//                                        7     6       5      4     3      2      1      0   Reset Value
sfr AUXR  = 0x8E; //Auxiliary Register  T0x12 T1x12 UART_M0x6 BRTR S2SMOD BRTx12 EXTRAM S1BRS  0000,0000
sfr BRT   = 0x9C; //S2 Baud-Rate Timer 

#define FOSC 22118400L      //System frequency

//#define FOSC 24576000L      //System frequency

typedef enum _VALUE
{
    TURE  = 1,
    FALSE = 0
};


typedef bit           boot;
typedef bit           BOOT;
typedef bit           BOOL;
typedef char          CHAR;
typedef unsigned char uint8;
typedef unsigned char UINT8;
typedef unsigned int  uint16;
typedef unsigned int  UINT16;
typedef unsigned long uint32;
typedef unsigned long UINT32;


#endif