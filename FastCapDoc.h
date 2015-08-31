// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file together with these source files. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.

// FastCapDoc.h : interface of the CFastCapDoc class
//
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_FASTCAPDOC_H__E1385E8B_3037_11D5_9282_74D314C10000__INCLUDED_)
#define AFX_FASTCAPDOC_H__E1385E8B_3037_11D5_9282_74D314C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Unicode string class 
#include "UnicodeString.h"

class CFastCapDoc : public CRichEditDoc
{
protected: // create from serialization only
	CFastCapDoc();
	DECLARE_DYNCREATE(CFastCapDoc)

// Attributes
public:
	void EndCallback();
	void LogCallback(CUnicodeString &text, unsigned long color);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastCapDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

// Implementation
public:
	virtual ~CFastCapDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...);

	IDispatch *m_pDispEndCallback;
	CUnicodeString m_clsEndCallbackName;
	IDispatch *m_pDispLogCallback;
	CUnicodeString m_clsLogCallbackName;

// Generated message map functions
protected:
	//{{AFX_MSG(CFastCapDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFastCapDoc)
	afx_msg BOOL IsRunning();
	afx_msg void ShowWindow();
	afx_msg BOOL Quit();
	afx_msg VARIANT GetCapacitance();
	afx_msg VARIANT GetCondNames();
	afx_msg BOOL Run(LPCTSTR commandLine);
	afx_msg void Stop();
	afx_msg BOOL SetEndCallback(LPDISPATCH callback, LPCTSTR cbName);
	afx_msg BOOL SetLogCallback(LPDISPATCH callback, LPCTSTR cbName);
	afx_msg float GetSolveTime();
	afx_msg long GetSolveMemory();
	afx_msg short GetReturnStatus();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTCAPDOC_H__E1385E8B_3037_11D5_9282_74D314C10000__INCLUDED_)
