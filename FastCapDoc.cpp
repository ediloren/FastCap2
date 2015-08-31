// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file together with these source files. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.

// FastCapDoc.cpp : implementation of the CFastCapDoc class
//

#include "stdafx.h"

// include for _bstrt_t class
#include <comdef.h>

#include "FastCap2.h"
#include "FastCapDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern volatile BOOL g_bIsFCRunning;
extern "C" volatile char bFCContinue;
extern HWND FCHwnd;

/////////////////////////////////////////////////////////////////////////////
// CFastCapDoc

IMPLEMENT_DYNCREATE(CFastCapDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CFastCapDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CFastCapDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, CRichEditDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFastCapDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CFastCapDoc)
	DISP_FUNCTION(CFastCapDoc, "IsRunning", IsRunning, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "ShowWindow", ShowWindow, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "Quit", Quit, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "GetCapacitance", GetCapacitance, VT_VARIANT, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "GetCondNames", GetCondNames, VT_VARIANT, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "Run", Run, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CFastCapDoc, "Stop", Stop, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "SetEndCallback", SetEndCallback, VT_BOOL, VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CFastCapDoc, "SetLogCallback", SetLogCallback, VT_BOOL, VTS_DISPATCH VTS_BSTR)
	DISP_FUNCTION(CFastCapDoc, "GetSolveTime", GetSolveTime, VT_R4, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "GetSolveMemory", GetSolveMemory, VT_I4, VTS_NONE)
	DISP_FUNCTION(CFastCapDoc, "GetReturnStatus", GetReturnStatus, VT_I2, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFastCap to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {4B8E95F2-2399-4D1D-BAB8-C18A09223D5E}
static const IID IID_IFastCap =
{ 0x4b8e95f2, 0x2399, 0x4d1d, { 0xba, 0xb8, 0xc1, 0x8a, 0x9, 0x22, 0x3d, 0x5e } };

BEGIN_INTERFACE_MAP(CFastCapDoc, CDocument)
	INTERFACE_PART(CFastCapDoc, IID_IFastCap, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastCapDoc construction/destruction

CFastCapDoc::CFastCapDoc()
{
	// OLE init
	EnableAutomation();
	// OLE
	AfxOleLockApp();
	// IDispatch pointers
	m_pDispEndCallback = NULL;
	m_pDispLogCallback = NULL;
}

CFastCapDoc::~CFastCapDoc()
{
	if(m_pDispEndCallback != NULL) {
		// tell Automation to remove the reference to this object,
		// so memory can be released
		m_pDispEndCallback->Release();
	}
	if(m_pDispLogCallback != NULL) {
		// tell Automation to remove the reference to this object,
		// so memory can be released
		m_pDispLogCallback->Release();
	}

	// Close the OLE Library
	AfxOleUnlockApp();
}

BOOL CFastCapDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

CRichEditCntrItem* CFastCapDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return NULL; //return new CFastCap2CntrItem(preo, (CFastCapDoc*) this);
}

/////////////////////////////////////////////////////////////////////////////
// CFastCapDoc serialization

void CFastCapDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CRichEditDoc enables serialization
	//  of the container document's COleClientItem objects.
	CRichEditDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CFastCapDoc diagnostics

#ifdef _DEBUG
void CFastCapDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CFastCapDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFastCapDoc commands

// overridden to inhibit document to prompt user for saving changes
BOOL CFastCapDoc::SaveModified() 
{
	// returning TRUE means that the document can be closed 
	return TRUE;
}

// AutoWrap helper function to implement a client Automation interface
//
// It is used here to call callback functions by our Automation server
// 
// For information about callbacks, see online help under:
//
// - HOWTO: Automate Excel From VC++ Without Using MFC, KB216686
// - b2c.exe VisualBasic to VC++ Automation client converter by Microsoft (web site)
// - Building OLE Automation Service Components in Visual Basic and Visual C++
//   (Hotel Manager Example) and related example project in VBasic
// - Locating Resources To Study Automation, KB152023
// - Office Automation Using VC++, KB196776
// - Platform SDK -> COM and ActiveX -> Automation -> Overview of Automation ->
//   How Do Clients and Objects Interact?
// - Platform SDK -> COM and ActiveX -> Automation -> Accessing ActiveX Objects ->
//   Creating Applications and Tools That Access Objects -> 
//   Accessing Members Through IDispatch / Accessing Members Through VTBLs
// - Inside OLE 2dn Edition
HRESULT CFastCapDoc::AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...)
{
    // Begin variable-argument list...
    va_list marker;
    va_start(marker, cArgs);

    if(!pDisp) {
        MessageBox(NULL, "NULL IDispatch passed to AutoWrap()", "Error", 0x10010);
        _exit(0);
    }

    // Variables used...
    DISPPARAMS dp = { NULL, NULL, 0, 0 };
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPID dispID;
    HRESULT hr;
    char buf[200];
    char szName[200];
    
    // Convert down to ANSI
    WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);
    
    // Get DISPID for name passed...
    hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx", szName, hr);
        MessageBox(NULL, buf, "AutoWrap()", 0x10010);
        _exit(0);
        return hr;
    }
    
    // Allocate memory for arguments...
    VARIANT *pArgs = new VARIANT[cArgs+1];
    // Extract arguments...
    for(int i=0; i<cArgs; i++) {
        pArgs[i] = va_arg(marker, VARIANT);
    }
    
    // Build DISPPARAMS
    dp.cArgs = cArgs;
    dp.rgvarg = pArgs;
    
    // Handle special-case for property-puts!
    if(autoType & DISPATCH_PROPERTYPUT) {
        dp.cNamedArgs = 1;
        dp.rgdispidNamedArgs = &dispidNamed;
    }
    
    // Make the call!
    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx", szName, dispID, hr);
        MessageBox(NULL, buf, "AutoWrap()", 0x10010);
        _exit(0);
        return hr;
    }
    // End variable-argument section...
    va_end(marker);
    
    delete [] pArgs;
    
    return hr;
}

BOOL CFastCapDoc::IsRunning() 
{
	return g_bIsFCRunning;
}

void CFastCapDoc::ShowWindow() 
{
	POSITION pos;
	CView* pView;
	CFrameWnd* pFrameWnd;

	pos = GetFirstViewPosition();
	pView = GetNextView(pos);
	if (pView != NULL)
	{
		pFrameWnd = pView->GetParentFrame();
		pFrameWnd->ActivateFrame(SW_SHOW);
		pFrameWnd = pFrameWnd->GetParentFrame();
		if (pFrameWnd != NULL)
			pFrameWnd->ActivateFrame(SW_SHOW);
	}  
	
	// Save the handler of the main window in global variable,
	// so working threads are able to send back messages
    // (threads are not allowed to access any other structure
	// of the calling process except via its window handler,
	// or an ASSERT will fail)
	//
	// No more used here; now assignment is done in CMainFrame::OnCreate(),
	// see comments in CFastCapApp::InitInstance()
	//FCHwnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	//ASSERT(FCHwnd);

	// perform view initial update
	pView->OnInitialUpdate();
}

BOOL CFastCapDoc::Quit() 
{
	CWnd *pWnd;

	// cannot quit while FastCap is running, must stop
	// thread before
	if(g_bIsFCRunning == TRUE)
		return FALSE;

	pWnd = AfxGetApp()->GetMainWnd();

	pWnd->PostMessage(WM_CLOSE);

	return TRUE;
}

VARIANT CFastCapDoc::GetCapacitance() 
{
	CFastCapApp *theApp;
	
	theApp = (CFastCapApp *)AfxGetApp();

	// build safe-array from capacitance matrix safe array
	COleSafeArray capMatrix(theApp->m_clsCapMatrix);

	// return the safe-array encapsulated in a VARIANT
	return capMatrix.Detach();
}

VARIANT CFastCapDoc::GetCondNames() 
{
	CFastCapApp *theApp;
	
	theApp = (CFastCapApp *)AfxGetApp();

	// build safe-array from conductor names safe array
	COleSafeArray nameArray(theApp->m_clsCondNames);

	// return the safe-array encapsulated in a VARIANT
	return nameArray.Detach();
}

BOOL CFastCapDoc::Run(LPCTSTR commandLine) 
{
	CFastCapApp *theApp;

	theApp = (CFastCapApp *)AfxGetApp();

	if( g_bIsFCRunning == TRUE) {
		return FALSE;
	} 

	theApp->parseCmdLine(&(theApp->argc), &(theApp->argv), commandLine);

	theApp->LaunchFastCap();

	return TRUE;
}

void CFastCapDoc::Stop() 
{
	// signal to the working thread to stop execution
	bFCContinue	= FALSE;
}

// set an automation callback upon FastCap working thread end
BOOL CFastCapDoc::SetEndCallback(LPDISPATCH callback, LPCTSTR cbName) 
{
	if(callback != NULL) {
		m_pDispEndCallback = callback;
		m_clsEndCallbackName = cbName;
		// tell Automation that there is a new reference to this object
		// so that it does not prematurely release it;
		// otherwise, the reference to the 'callback' object would go
		// out of scope once the SetEndCallback() method is complete,
		// and Automation therefore would dereference and releases the memory. 
		m_pDispEndCallback->AddRef();
		return TRUE;
	}
	else {
		// no callback anymore
		m_pDispEndCallback->Release();
		m_pDispEndCallback = NULL;
		return FALSE;
	}
}

// set an automation callback upon FastCap working thread end
void CFastCapDoc::EndCallback()
{
	if(m_pDispEndCallback != NULL) {
		AutoWrap(DISPATCH_METHOD, NULL, m_pDispEndCallback, m_clsEndCallbackName.GetPointer(), 0);
	}
}

BOOL CFastCapDoc::SetLogCallback(LPDISPATCH callback, LPCTSTR cbName)
{
	if(callback != NULL) {
		m_pDispLogCallback = callback;
		m_clsLogCallbackName = cbName;
		// tell Automation that there is a new reference to this object
		// so that it does not prematurely release it;
		// otherwise, the reference to the 'callback' object would go
		// out of scope once the SetEndCallback() method is complete,
		// and Automation therefore would dereference and releases the memory. 
		m_pDispLogCallback->AddRef();
		return TRUE;
	}
	else {
		// no callback anymore
		m_pDispLogCallback->Release();
		m_pDispLogCallback = NULL;
		return FALSE;
	}
}

// set an automation callback upon FastCap working thread log messages (viewprintf()..)
void CFastCapDoc::LogCallback(CUnicodeString &text, unsigned long color)
{
	VARIANT parm[2];

 	if(m_pDispLogCallback != NULL) {

		parm[0].vt = VT_BSTR;
		parm[0].bstrVal = ::SysAllocString(text.GetPointer());
		parm[1].vt = VT_I4;
		parm[1].lVal = color;
		
		AutoWrap(DISPATCH_METHOD, NULL, m_pDispLogCallback, m_clsLogCallbackName.GetPointer(), 2, parm[1], parm[0]);

		VariantClear(&parm[0]);
		VariantClear(&parm[1]);
	}
}

float CFastCapDoc::GetSolveTime() 
{
	CFastCapApp *theApp;
	
	theApp = (CFastCapApp *)AfxGetApp();

	return theApp->m_fSolveTime;
}

long CFastCapDoc::GetSolveMemory() 
{
	CFastCapApp *theApp;
	
	theApp = (CFastCapApp *)AfxGetApp();

	return theApp->m_lSolveMemory;
}

short CFastCapDoc::GetReturnStatus() 
{
	CFastCapApp *theApp;
	
	theApp = (CFastCapApp *)AfxGetApp();

	return theApp->m_iRetStatus;
}
