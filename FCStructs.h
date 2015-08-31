// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file together with these source files. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.


// C Header file with definitions common to both FastCap and Windows
// I/O window


#ifndef FCSTRUCTS_H
#define FCSTRUCTS_H

#define FCV_BLACK	RGB(0,0,0)
#define FCV_RED		RGB(255,0,0)

// defines used with FCOnClosing function defined in FastCapWindow.h/.cpp
#define FC_NORMAL_END			0
#define FC_USER_BREAK			-1
#define FC_OUT_OF_MEMORY		-2
#define FC_CANNOT_OPEN_FILE		-4
#define FC_FILE_ERROR			-8
#define FC_GENERIC_ERROR		-8192
#define FC_STILL_RUNNING		-16384

struct capMatrix {
    long   m_lDim;
	double **m_daCapMatrix;
	char   **m_sCondNames;
};

#endif

