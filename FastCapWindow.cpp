// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file together with these source files. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.


// Functions linking FastCap with Windows I/O window


#include "stdafx.h"

#include "FastCapWindow.h"
#include <stdio.h>

void FCSetName(char *name)
{
	int val;

	static const UINT UWM_SET_TITLE = RegisterWindowMessage(_T("UWM_SET_TITLE-FastCap-Enrico_Di_Lorenzo"));

	// safe copy of FastCap input file name
	strncpy(g_sTitle, name, MAX_TITLE_LENGHT);
	g_sTitle[MAX_TITLE_LENGHT-1] = 0;

	// ask main process to update window title
	if(IsWindow(FCHwnd))
		val = ::SendMessage(FCHwnd, UWM_SET_TITLE, 0, 0);
}

// function called on FastCap closing (a return to RunFCThread() will follow 
// the call to this routine)
void FCOnClosing(int cause)
{
	if(cause == FC_USER_BREAK) {
		viewprintf(stderr, "\n\nWARNING: Program execution stopped on user request!");
	}
}

// function called to exit FastCap (no any other function call can 
// follow: thread exits here)
void FCExit(int cause)
{
	if(cause == FC_USER_BREAK) {
		viewprintf(stderr, "\nWARNING: Program execution stopped on user request!\n");
	}
	else if(cause == FC_OUT_OF_MEMORY) {
		viewprintf(stderr, "\nERROR: Out of memory, program execution stopped\n");
	}

	// if exiting thread directly, won't pass through RunFCThread() in FastCap2
	// any more. Therefore must clean up here
	OnFastCapExit(cause);

	// close all open files
	_fcloseall();

	AfxEndThread(0);
}

int viewprintf(FILE *out, const char *fmt,...)
{
	int ret;
	unsigned long color;
	int val;

	static const UINT UWM_OUTPUT_TEXT = RegisterWindowMessage(_T("UWM_OUTPUT_TEXT-FastCap-Enrico_Di_Lorenzo"));
	static const UINT UWM_LOG_TEXT = RegisterWindowMessage(_T("UWM_LOG_TEXT-FastCap-Enrico_Di_Lorenzo"));

	va_list arg_ptr;

	va_start(arg_ptr, fmt);

	ret = vsprintf(g_sOutputText, fmt, arg_ptr);

	va_end(arg_ptr);

	if (out == stdout) {
		color = FCV_BLACK;
	}
	else
		color = FCV_RED;

	// ask main process to output text in given color
	if(IsWindow(FCHwnd)) {
		// SendMessage() does not return until the window procedure 
		// has processed the message, so no synchronization issues
		val = ::SendMessage(FCHwnd, UWM_OUTPUT_TEXT, color, 0);
		// However, automation cannot make calls during input-synchronous calls,
		// like SendMessage(); so post a message to the main process to ask
		// for Automation log. The log message is stored during the processing
		// of the previous SendMessage()
		val = ::PostMessage(FCHwnd, UWM_LOG_TEXT, color, 0);
	}

	return ret;
}
