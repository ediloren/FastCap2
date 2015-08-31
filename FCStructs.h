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

struct capMatrix {
    long   m_lDim;
	double **m_daCapMatrix;
	char   **m_sCondNames;
};

#endif

