// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file together with these source files. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.


// Copyright (c) 2012 Enrico Di Lorenzo, www.fastfieldsolvers.com
// All rights reserved


#if !defined(AFX_RUNDIALOG_H__E89AAF21_5486_11D5_9282_04F014C10000__INCLUDED_)
#define AFX_RUNDIALOG_H__E89AAF21_5486_11D5_9282_04F014C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RunDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRunDialog dialog

class CRunDialog : public CDialog
{
// Construction
public:
	CRunDialog(CWnd* pParent = NULL);   // standard constructor
	char CRunDialog::generateCmdLine(int *argc, char ***argv);

	CString m_strSamplePath;

// Dialog Data
	//{{AFX_DATA(CRunDialog)
	enum { IDD = IDD_DIALOG_RUN };
	CEdit	m_ctrlInputFileName;
	CString	m_sInputFileName;
	BOOL	m_bListFile;
	float	m_fGlobalPermittivity;
	CString	m_sRemoveFromSolveList;
	CString	m_sRemoveFromInputList;
	UINT	m_uiMultipoleOrder;
	float	m_fIterationTolerance;
	BOOL	m_bLineDrawingMode;
	float	m_fAzimuth;
	float	m_fDistance;
	float	m_fRotation;
	float	m_fScale;
	float	m_fElevation;
	float	m_fLineWidth;
	CString	m_sRemoveConductors;
	BOOL	m_bRemoveDielectric;
	CString	m_sFigureFiles;
	BOOL	m_bMakeTotalChargePics;
	BOOL	m_bPutCommandLineInPS;
	BOOL	m_bRemoveShadingKey;
	BOOL	m_bNumberFaces;
	BOOL	m_bRemoveShowpage;
	BOOL	m_bPrintGraph;
	BOOL	m_bSuppressHiddenLines;
	CString	m_sPartitioningDepth;
	CString	m_sAxesLength;
	CString	m_sUpAxis;
	CString	m_sChargeDistribPics;
	BOOL	m_bChargeDistribPics;
	BOOL	m_bDumpTimeAndMem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRunDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRunDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnReset();
	afx_msg void OnBrowseInputFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNDIALOG_H__E89AAF21_5486_11D5_9282_04F014C10000__INCLUDED_)
