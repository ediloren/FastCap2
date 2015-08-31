// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file together with these source files. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.


// C Header file with definitions for linking FastCap with Windows
// I/O window


#ifndef FCWINDOW_H
#define FCWINDOW_H

#include "FCStructs.h"

// defines used with FCOnClosing function defined in FastCapWindow.h/.cpp
#define FC_NORMAL_END			0
#define FC_USER_BREAK			-1
#define FC_OUT_OF_MEMORY		-2
#define FC_CANNOT_OPEN_FILE		-4
#define FC_FILE_ERROR			-8
#define FC_GENERIC_ERROR		-8192
#define FC_STILL_RUNNING		-16384

// maximum length of a conductor name
#define FC_MAX_NAME_LEN		64

extern volatile char bFCContinue;
extern volatile struct capMatrix strctCapMatrix;
extern volatile float fSolveTime;
extern volatile long lSolveMemory;

extern void FCSetName(char *name);


// prototypes for var initialization
void InitGlobAndStatVars(void);

void InitCalcpVars(void);
void InitCapsolveVars(void);
void InitDirectVars(void);
void InitMulDoVars(void);
void InitMulGlobalVars(void);
void InitMulMatsVars(void);
void InitPatranVars(void);
void InitPsMatDisplayVars(void);
void InitUglierAllocVars(void);
void InitZbufSortVars(void);


#endif

