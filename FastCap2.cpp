// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file in the overall package. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.


// FastCap2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include <stdio.h>

#include "FastCap2.h"
#include "FastCapWindow.h"

#include "MainFrm.h"
#include "FastCapView.h"
#include "FCStructs.h"

// imported functions from FastCap
extern "C" fastCapMain(int argc, char *argv[]);
extern "C" void ufree();
extern "C" void dfreeall();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT RunFCThread(LPVOID p);
void OnFastCapExit(int cause);
extern volatile BOOL g_bIsFCRunning = FALSE;
extern "C" volatile float fSolveTime = 0.0;
extern "C" volatile long lSolveMemory = 0;
extern "C" volatile char bFCContinue = TRUE;
extern "C" volatile struct capMatrix strctCapMatrix = {0, NULL, NULL};

extern HWND FCHwnd = NULL;
extern CFastCapApp *mainApp = NULL;;

/////////////////////////////////////////////////////////////////////////////
// CFastCapApp

BEGIN_MESSAGE_MAP(CFastCapApp, CWinApp)
	//{{AFX_MSG_MAP(CFastCapApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FASTCAP_RUN, OnFastCapRun)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastCapApp construction

CFastCapApp::CFastCapApp() 
{
	DWORD numElements[2] = {0, 0};

	// init COleSafeArrays used in Automation
	m_clsCapMatrix.Create(VT_VARIANT, 2, numElements);
	m_clsCondNames.Create(VT_VARIANT, 1, numElements);

	// Place all significant initialization in InitInstance
}

CFastCapApp::~CFastCapApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFastCapApp object

CFastCapApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
// It is used for OLE Automation

// {1ECCFB33-710A-4B5C-9295-A503CA2D10E2}
static const CLSID clsid =
{ 0x1eccfb33, 0x710a, 0x4b5c, { 0x92, 0x95, 0xa5, 0x3, 0xca, 0x2d, 0x10, 0xe2 } };

/////////////////////////////////////////////////////////////////////////////
// CFastCapApp initialization

BOOL CFastCapApp::InitInstance()
{
	HKEY rkey;
	char app_path[_MAX_PATH];
	unsigned long len;
	LONG ret;

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("FastFieldSolvers"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	
	// Load application settings from registry
	//
	
	m_strSamplePath = _T("");

	// Load global (LOCAL_MACHINE) FastCap2 variables from the registry
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\FastCap2\\Settings"), 0,
		KEY_READ, &rkey) != ERROR_SUCCESS) {
		AfxMessageBox("Cannot find FastCap2 'Settings' key in Registy\nPlease try installing FastCap2 again", MB_ICONSTOP);
	}
	else {
		// installation path
		len = _MAX_PATH;
		if ( RegQueryValueEx( rkey, _T("Path"), NULL, NULL, (unsigned char *)app_path, &len) != ERROR_SUCCESS ) {
			AfxMessageBox("Cannot find FastCap2 path in Registy\nPlease try installing FastModel again", MB_ICONSTOP);
		}
		else {
			m_strAppPath = app_path;
		}
		// sample path
		len = _MAX_PATH;
		ret = RegQueryValueEx( rkey, _T("SamplePath"), NULL, NULL, (unsigned char *)app_path, &len);
		if ( ret == ERROR_SUCCESS ) {
			m_strSamplePath = app_path;
		}
	}
	RegCloseKey(rkey);
	
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	m_pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFastCapDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CFastCapView));
	m_pDocTemplate->SetContainerInfo(IDR_CNTR_INPLACE);
	AddDocTemplate(m_pDocTemplate);

	// The following commands have been commented out, because file type must be registred
	// by FastModel only

	// Enable DDE Execute open
	//EnableShellOpen();
	//RegisterShellFileTypes(TRUE);

	// Enable drag/drop open
	//m_pMainWnd->DragAcceptFiles();

	// The following call is taken from CWinApp::ProcessShellCommand function 
	// (see MFC source appui2.cpp) because needed if custom argument parsing
	// is used (otherwise OnFileNew would never be called)
	// 
	// Now it is obsolete since when introducing OLE command arguments
	// are handled through CCmdLineInfo class
	//
	//	if (! (AfxGetApp()->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL)))
	//		OnFileNew();
	//	if (m_pMainWnd == NULL)
	//		return FALSE;

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid, m_pDocTemplate, TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

		// Application was run with /Embedding or /Automation.  Don't show the
		// main window in this case.
	}
	else {
		// When a server application is launched stand-alone, it is a good idea
		//  to update the system registry in case it has been damaged.
		//
		// MS patch KB254957 BUG: Registry update code may fail when an unprivileged user runs an MFC OLE server on Windows 2000
		HKEY hTestKey = NULL;
		LONG lResult = ::RegCreateKeyEx(HKEY_CLASSES_ROOT, 
			"CLSID", 0, "", 
			REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, 
			&hTestKey, NULL);
		if ((ERROR_SUCCESS == lResult) && (hTestKey != NULL))
		{
			// It's ok to call UpdateRegistry
			//
			// remark: patch not working, still getting the message "Failed to update the system registry. Please try using REGEDIT."
			// workaround is to inhibit UpdateRegistry() at all, considering that at installation time the keys should have been
			// installed properly
			//m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
		}

		COleObjectFactory::UpdateRegistryAll();
		
		// Dispatch commands specified on the command line
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
		
		// The one and only window has been initialized, so show and update it.
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
	}

	// Obsolete call; now FastCap cannot be launched from the command line
	// using arguments, to avoid conflict with Windows standard arguments
	// (e.g. /p or -p for printing, /Embedded, ect.)
	// FastCap must be controlled via OLE or via RunDialog
	//
	// Parse command line for FastCap commands
	// and launch FastCap if required
	//	if(parseCmdLine(&argc, &argv, m_lpCmdLine) && argc > 1) {
	//		LaunchFastCap();
	//	} 

	// Save the handler of the main window in global variable,
	// so working threads are able to send back messages
    // (threads are not allowed to access any other structure
	// of the calling process except via its window handler,
	// or an ASSERT will fail)
	//
	// Note that in case of automation, reference is NULL until
	// some messages are processed with ::DispatchMessage();
	// these messages have code 0x400, or WM_USER; it's not clear
	// why they must be processed to assign 'm_pMainWnd', especially
	// because the main window have already been created anyway.
	// Therefore, 'FCHwnd' is now initialized in CMainFrame::OnCreate()
	// but in CMainFrame::OnCreate()
	// FCHwnd = GetMainWnd()->GetSafeHwnd();

	g_bIsFCRunning = FALSE;
	bFCContinue = FALSE;

	return TRUE;
}

BOOL CFastCapApp::parseCmdLine(int *argc, char ***argv, const char *commandStr)
{
	char argStr[256], *pointers[256], *cmdStr;
	int res, skip, i;
	
	// copy pointer, not to change 'commandStr'
	// (required when function is called by automation passing a 'const')
	cmdStr = (char *)commandStr;

	// initalize first argument with program name
	pointers[0] = new char[10];
	strcpy(pointers[0], "fastcap");

	*argc = 1;
	
	// then parse all arguments
	//
	res = getSubstring(cmdStr, argStr, &skip);
	// first argument should be input file name, so store it in run dialog
	// for future runs
	if( res != EOF ) 
		m_clsRunDlg.m_sInputFileName = argStr;

	while (res != 0 && res != EOF) {
		cmdStr += skip;
		pointers[*argc] = new char[strlen(argStr)+1];
		strcpy(pointers[*argc], argStr);
		(*argc)++;
		res = getSubstring(cmdStr, argStr, &skip);
	}

	if (*argc == 1) 
		return FALSE;
	else {
		*argv = new char*[*argc];
		
		for (i=0; i<*argc; i++)
			(*argv)[i] = pointers[i];

		return TRUE;
	}
}

// Special version of sscanf which handles also file names with spaces inside,
// provided they are surrounded by '"'.
// Note that this routine is specialized to retrieve only strings
int CFastCapApp::getSubstring(const char *buffer, char *substr, int *skip)
{
	int res, openPos, deltaClosePos, startPos;
	char *openPosPtr, *closePosPtr, tmpStr[256];

	substr[0] = '\0';

	// read a piece of string
	res = sscanf(buffer, "%s%n", tmpStr, skip);
	// if not finished
	if( res != EOF ) {
		// find if it does contain any '"'
		openPosPtr = strchr(buffer, '"');
        // if found and if within tmpStr (before any space)
		openPos = (int)(openPosPtr - buffer);
		if( openPosPtr != NULL && openPos < strlen(tmpStr) ) {
			// search for the closing '"'
			closePosPtr = strchr(buffer + openPos + 1, '"');
			// if no closing '"', assume the end is the end of the
			// string in the buffer, regardless of any spaces or tabs
			if(closePosPtr == NULL) {
				// build 'substr' string by collating the different pieces,
				// skipping the '"'
				strncpy(tmpStr, buffer, openPos);
				tmpStr[openPos] = '\0';
				strcat(tmpStr, buffer + openPos + 1);
				// remove any trailing space left
				startPos = strspn(tmpStr, " \t\n");
				strcpy(substr, tmpStr + startPos);
				*skip = strlen(buffer);
			}
			else {
				// build 'substr' string by collating the different pieces,
				// skipping the '"'s
				deltaClosePos = (int)(closePosPtr - openPosPtr);
				strncpy(tmpStr, buffer, openPos);
				tmpStr[openPos] = '\0';
				strncat(tmpStr, buffer + openPos + 1, deltaClosePos - 1);
				// remove any trailing space left
				startPos = strspn(tmpStr, " \t\n");
				strcpy(substr, tmpStr + startPos);
				*skip = openPos + deltaClosePos + 1;
			}
		}
		// if no '"' at all, simply copy scanned string
		else {
			strcpy(substr, tmpStr);
		}
	}

	return res;
}

// free memory allocated for command line parameters
void CFastCapApp::freeCmdLine()
{
	int i;

	for (i=0; i<argc; i++)
		delete argv[i];

	delete argv;
}

// Remark: no synchronization object is needed, since it's always
// the same process which launchs FastCap, both in case of user's
// request or OLE request through Windows messages, so access is 
// never concurrent
void CFastCapApp::LaunchFastCap()
{
	g_bIsFCRunning = TRUE;
	bFCContinue = TRUE;
	AfxBeginThread(RunFCThread, this);
}

CFastCapDoc* CFastCapApp::GetDocument()
{
	POSITION pos;
	CFastCapDoc* fastcapdoc;

	pos = m_pDocTemplate->GetFirstDocPosition();
	fastcapdoc = (CFastCapDoc*) m_pDocTemplate->GetNextDoc(pos); 
	
	// only for debug
	if( pos != NULL)
		ASSERT("More than one document type!");

	return fastcapdoc;
}

//////////////////////////////////////////////////////////
// Working Thread


UINT RunFCThread(LPVOID p)
{
	DWORD numElements[2];
	long index[2], i;
	COleVariant data;

	CFastCapApp *theApp = mainApp = (CFastCapApp *)p;
	
	theApp->m_iRetStatus = FC_STILL_RUNNING;

	fastCapMain(theApp->argc, theApp->argv);

	//
	// copy capacitance matrix into safe-array
	//

	// Remark: safe array contains VARIANTs which are of type VT_R8 (double);
	// safe array does not contains VT_R8 any more, since VBScript is not able
	// to deal with arrays containing anything but VARIANTs;
	// See MS KB174576, HOWTO: Return Arrays from Server-Side Objects in ASP

	// destroy previous matrix
	theApp->m_clsCapMatrix.Clear();
	// init capacitance matrix dimensions
	numElements[0] = strctCapMatrix.m_lDim;
	numElements[1] = strctCapMatrix.m_lDim;
	// create the safe-array...
	theApp->m_clsCapMatrix.Create(VT_VARIANT, 2, numElements);
	// copy values
	for(index[0]=0; index[0]<strctCapMatrix.m_lDim; index[0]++) {
		for(index[1]=0; index[1]<strctCapMatrix.m_lDim; index[1]++) {
			data = strctCapMatrix.m_daCapMatrix[index[0]][index[1]];
			theApp->m_clsCapMatrix.PutElement(index, &data);
		}
	}

	//
	// copy conductor names into safe-array
	//

	// Remark: safe array contains VARIANTs which are of type VT_BSTR;
	// See MS KB174576, HOWTO: Return Arrays from Server-Side Objects in ASP
	// (before this change, the class CComBSTR, which encapsulates VT_BSTR,
	// was used in the safe array)

	// destroy previous matrix
	theApp->m_clsCondNames.Clear();
	// init conductor names array dimensions
	numElements[0] = strctCapMatrix.m_lDim;
	// create the safe-array...
	theApp->m_clsCondNames.Create(VT_VARIANT, 1, numElements);
	// copy values
	for(index[0]=0; index[0]<strctCapMatrix.m_lDim; index[0]++) {
			data = strctCapMatrix.m_sCondNames[index[0]];
			theApp->m_clsCondNames.PutElement(index, &data);
	}

	//
	// copy solve time and memory
	//

	theApp->m_fSolveTime = fSolveTime;
	theApp->m_lSolveMemory = lSolveMemory;

	// Clear info in the 'strctCapMatrix' structure
	//
	// free memory used by FastCap to store the matrix
	for(i=0; i < strctCapMatrix.m_lDim; i++)  {
		free((void*)strctCapMatrix.m_daCapMatrix[i]);
	}
	free((void*)strctCapMatrix.m_daCapMatrix);
	// free memory used by FastCap to store the list of names
	for(i=0; i < strctCapMatrix.m_lDim; i++)  {
		free((void*)strctCapMatrix.m_sCondNames[i]);
	}
	free((void*)strctCapMatrix.m_sCondNames);
	// reset the matrix dimension
	strctCapMatrix.m_lDim = 0;

	OnFastCapExit(FC_NORMAL_END);

	return 0;
}

void OnFastCapExit(int cause)
{
	int val;

	static const UINT UWM_THREAD_END = RegisterWindowMessage(_T("UWM_THREAD_END-FastCap-Enrico_Di_Lorenzo"));

	//
	// copy reason for exiting
	//

	mainApp->m_iRetStatus = cause;

	// free all allocated memory
	dfreeall();
	ufree();

	// free memory allocated for passing command line parameters
	mainApp->freeCmdLine();

	// signal end of task
	g_bIsFCRunning = FALSE;

	// inform main process that thread has stopped
	if(IsWindow(FCHwnd))
		val = ::PostMessage(FCHwnd, UWM_THREAD_END, 0, 0);
}

//////////////////////////////////////////////////////////
// Message Handlers

void CFastCapApp::OnFastCapRun() 
{	
	CFastCapView *view;
	CMainFrame *mainFrame;

	if( g_bIsFCRunning == TRUE ) {
		mainFrame = (CMainFrame *) GetMainWnd();
		view = (CFastCapView *) mainFrame->GetActiveView();
		view->OutputText("Error on user 'Run' request: cannot run another thread, FastCap is already running\n", FCV_RED);
	} 
	else {
		RunDialogLaunch();
	}
}

void CFastCapApp::RunDialogLaunch()
{
	// Set FastCap state as running as long as the user 
	// keeps the run dialog open, even if it is not true;
	// This is to prevent OLE messages (which are pumped
	// together with other window messages while waiting
	// for user input, see DoModal() ) to start FastCap
	// thread in the meanwhile 	
	g_bIsFCRunning = true;
	
	// set the samples installation directory
	// as the first directory to look at
	m_clsRunDlg.m_strSamplePath = m_strSamplePath;
			
	if( m_clsRunDlg.DoModal() != IDCANCEL ) {
		// only of no error from run dialog, run FastCap
		if(m_clsRunDlg.generateCmdLine(&argc, &argv) != 0) {
			LaunchFastCap();
		}
		else {
			// If FastCap thread is not run, reset run state
			g_bIsFCRunning = FALSE;
		}
	}
	else {
		// If FastCap thread is not run, reset run state
		g_bIsFCRunning = false;
	}
}

BOOL CFastCapApp::OnCopyData(CWnd* pWnd, COPYDATASTRUCT *pCopyDataStruct)
{
	CFastCapView *view;
	CMainFrame *mainFrame;
	char command[12];

	const char *rxCopyData = (LPCSTR) (pCopyDataStruct->lpData);

	// the first 10 chars are the sent command; the others are the arguments
	strncpy(command, (LPCSTR) rxCopyData, 10);

	if (g_bIsFCRunning == TRUE) {
		mainFrame = (CMainFrame *) GetMainWnd();
		view = (CFastCapView *) mainFrame->GetActiveView();
		view->OutputText("Error on user 'Run' request: cannot run another thread, FastCap is already running\n", FCV_RED);
		return TRUE;
		
	
// TBC warning: should implement the possibility to stop FastCap from here, but 
// following code has bugs
 /*		if( MessageBox(m_pMainWnd->GetSafeHwnd(), "FastCap is already running\nDo you want to stop it?", 
				"Warning", MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL) {
			return TRUE;
		}

		// signal to stop 
		bFCContinue = FALSE;
		// then wait for the event; TBC warning: should use some os event,
		// not to keep the processor busy; also should use a timer, to
		// avoid infinite loop in case of error
		while (g_bIsFCRunning == TRUE)
			; */
	}

	if (strncmp( rxCopyData, "command ln", 10) == 0) {
		// the argument is a valid FastCap command line

		if(parseCmdLine(&argc, &argv, (char *) (rxCopyData + 10) )) {
			LaunchFastCap();
		} 

	}
	else if (strncmp( rxCopyData, "path name ", 10) == 0) {
		// the argument is only the path and file name of the 
		// FastCap input file. The options have to be chosen
		// locally.

		m_clsRunDlg.m_sInputFileName =  (rxCopyData + 10);

		RunDialogLaunch();
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFastCapApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFastCapApp commands





