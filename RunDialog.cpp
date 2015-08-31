// Copyright (c) 2013 FastFieldSolvers S.r.l.
// http://www.fastfieldsolvers.com
// All Rights reserved
//
// Usage is subject to the License that you should have received
// as a text file together with these source files. The License text is also available
// in the relevant source code distribution from http://www.fastfieldsolvers.com
// or contacting FastFieldSolvers S.r.l.


// RunDialog.cpp : implementation file
//


#include "stdafx.h"
#include "FastCap2.h"
#include "RunDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRunDialog dialog


CRunDialog::CRunDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRunDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRunDialog)
	m_sInputFileName = _T("");
	m_bListFile = TRUE;
	m_fGlobalPermittivity = 1.0f;
	m_sRemoveFromSolveList = _T("");
	m_sRemoveFromInputList = _T("");
	m_uiMultipoleOrder = 2;
	m_fIterationTolerance = 0.01f;
	m_bLineDrawingMode = FALSE;
	m_fAzimuth = 50.0f;
	m_fDistance = 2.0f;
	m_fRotation = 0.0f;
	m_fScale = 1.0f;
	m_fElevation = 50.0f;
	m_fLineWidth = 1.0f;
	m_sRemoveConductors = _T("");
	m_bRemoveDielectric = FALSE;
	m_sFigureFiles = _T("");
	m_bMakeTotalChargePics = FALSE;
	m_bPutCommandLineInPS = FALSE;
	m_bRemoveShadingKey = FALSE;
	m_bNumberFaces = FALSE;
	m_bRemoveShowpage = FALSE;
	m_bPrintGraph = FALSE;
	m_bSuppressHiddenLines = FALSE;
	m_sPartitioningDepth = _T("auto");
	m_sAxesLength = _T("none");
	m_sUpAxis = _T("z");
	m_sChargeDistribPics = _T("");
	m_bChargeDistribPics = FALSE;
	m_bDumpTimeAndMem = FALSE;
	//}}AFX_DATA_INIT
}


void CRunDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRunDialog)
	DDX_Control(pDX, IDC_INPUT_FILE_NAME, m_ctrlInputFileName);
	DDX_Text(pDX, IDC_INPUT_FILE_NAME, m_sInputFileName);
	DDX_Check(pDX, IDC_LIST_FILE, m_bListFile);
	DDX_Text(pDX, IDC_GLOBAL_PERMITTIVITY, m_fGlobalPermittivity);
	DDX_Text(pDX, IDC_REMOVE_SOLVE_LIST, m_sRemoveFromSolveList);
	DDX_Text(pDX, IDC_REMOVE_INPUT_LIST, m_sRemoveFromInputList);
	DDX_Text(pDX, IDC_MULTIPOLE_ORDER, m_uiMultipoleOrder);
	DDX_Text(pDX, IDC_ITERATION_TOLERANCE, m_fIterationTolerance);
	DDX_Check(pDX, IDC_LINE_DRAWING_MODE, m_bLineDrawingMode);
	DDX_Text(pDX, IDC_AZIMUTH, m_fAzimuth);
	DDX_Text(pDX, IDC_DISTANCE, m_fDistance);
	DDX_Text(pDX, IDC_ROTATION, m_fRotation);
	DDX_Text(pDX, IDC_SCALE, m_fScale);
	DDX_Text(pDX, IDC_ELEVATION, m_fElevation);
	DDX_Text(pDX, IDC_LINE_WIDTH, m_fLineWidth);
	DDX_Text(pDX, IDC_REMOVE_CONDUCTORS, m_sRemoveConductors);
	DDX_Check(pDX, IDC_REMOVE_DIELECTRIC, m_bRemoveDielectric);
	DDX_Text(pDX, IDC_FIGURE_FILES, m_sFigureFiles);
	DDX_Check(pDX, IDC_TOTAL_CHARGE, m_bMakeTotalChargePics);
	DDX_Check(pDX, IDC_PUT_COMMANDS_PS, m_bPutCommandLineInPS);
	DDX_Check(pDX, IDC_REMOVE_SHADING_KEY, m_bRemoveShadingKey);
	DDX_Check(pDX, IDC_NUMBER_FACES, m_bNumberFaces);
	DDX_Check(pDX, IDC_REMOVE_SHOWPAGE, m_bRemoveShowpage);
	DDX_Check(pDX, IDC_PRINT_GRAPHS, m_bPrintGraph);
	DDX_Check(pDX, IDC_NO_HIDDEN_LINES, m_bSuppressHiddenLines);
	DDX_CBString(pDX, IDC_PARTITIONING_DEPTH, m_sPartitioningDepth);
	DDX_CBString(pDX, IDC_AXES_LENGTH, m_sAxesLength);
	DDX_CBString(pDX, IDC_UPAXIS, m_sUpAxis);
	DDX_Text(pDX, IDC_CHARGE_DISTR_FILES, m_sChargeDistribPics);
	DDX_Check(pDX, IDC_CHARGE_DISTRIBUTION, m_bChargeDistribPics);
	DDX_Check(pDX, IDC_DUMP_TIME_AND_MEM, m_bDumpTimeAndMem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRunDialog, CDialog)
	//{{AFX_MSG_MAP(CRunDialog)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_BROWSE_INPUT_FILE, OnBrowseInputFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRunDialog message handlers

BOOL CRunDialog::OnInitDialog() 
{
	int len;

	CDialog::OnInitDialog();

	// position caret to end of line
	UpdateData(TRUE);
	len = m_ctrlInputFileName.LineLength();
	m_ctrlInputFileName.SetSel(len, len);
	UpdateData(FALSE);

	// set controls to default values
//	OnReset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// reset dialog box controls to defaults, except input file name
void CRunDialog::OnReset() 
{

	m_bListFile = TRUE;
	m_fGlobalPermittivity = 1.0f;
	m_sRemoveFromSolveList = _T("");
	m_sRemoveFromInputList = _T("");
	m_uiMultipoleOrder = 2;
	m_fIterationTolerance = 0.01f;
	m_bLineDrawingMode = FALSE;
	m_fAzimuth = 50.0f;
	m_fDistance = 2.0f;
	m_fRotation = 0.0f;
	m_fScale = 1.0f;
	m_fElevation = 50.0f;
	m_fLineWidth = 1.0f;
	m_sRemoveConductors = _T("");
	m_bRemoveDielectric = FALSE;
	m_sFigureFiles = _T("");
	m_bMakeTotalChargePics = FALSE;
	m_bPutCommandLineInPS = FALSE;
	m_bRemoveShadingKey = FALSE;
	m_bNumberFaces = FALSE;
	m_bRemoveShowpage = FALSE;
	m_bPrintGraph = FALSE;
	m_bSuppressHiddenLines = FALSE;
	m_sPartitioningDepth = _T("auto");
	m_sAxesLength = _T("none");
	m_sUpAxis = _T("z");
	m_sChargeDistribPics = _T("");
	m_bChargeDistribPics = FALSE;
	m_bDumpTimeAndMem = FALSE;
	
	UpdateData(FALSE);
}

void CRunDialog::OnBrowseInputFile() 
{
	int len;
	DWORD firstTime, reglen;
	LONG ret;
	bool isFirstTime;
	HKEY rkey;


	CFileDialog	dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY,
		"FastCap List files (*.lst)|*.lst|FastCap Geometry files (*.neu, *.qui)|*.neu;*.qui|All files (*.*)|*.*|");

	isFirstTime = false;

	// Load current (CURRENT_USER) FasterCap variables from the registry
	ret = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\FastFieldSolvers\\FastCap2\\Settings"), 0, KEY_READ | KEY_SET_VALUE, &rkey);
	if ( ret == ERROR_SUCCESS) {
		// get the value to understand if this is the first time FasterCap is launched;
		// if the key does not exist (for the current user CU), then this is the first time
		reglen = sizeof(firstTime);
		if ( RegQueryValueEx( rkey, _T("FirstLaunch"), NULL, NULL, (unsigned char *)&firstTime, &reglen) != ERROR_SUCCESS ) {
			isFirstTime = true;
			// and record in the registry that the user already launched FasterCap once
			firstTime = 1;
			if ( RegSetValueEx( rkey, _T("FirstLaunch"), NULL, REG_DWORD,  (unsigned char *)&firstTime, sizeof(firstTime)) != ERROR_SUCCESS ) {
				AfxMessageBox("Cannot write to the Registry\nPlease check user privileges", MB_ICONSTOP);
			}
		}
	}
	RegCloseKey(rkey);

	if(isFirstTime == true ) { 
		dlgFile.m_ofn.lpstrInitialDir = m_strSamplePath;
	}

	if( dlgFile.DoModal() != IDCANCEL ) {
		// must copy into class vars the values of control,
		// not to loose their status when UpdateData(FALSE)
		UpdateData(TRUE);
		m_sInputFileName = dlgFile.GetPathName();
		UpdateData(FALSE);

		// position caret to end of line
		UpdateData(TRUE);
		len = m_ctrlInputFileName.LineLength();
		m_ctrlInputFileName.SetSel(len, len);
		UpdateData(FALSE);
	}

	
}

char CRunDialog::generateCmdLine(int *argc, char ***argv)
{
	char argStr[256], *pointers[256], *strPnt;
	size_t len;
	int skip, i;
	BOOL iscomma;
	
	// initalize first argument with program name
	pointers[0] = new char[10];
	strcpy(pointers[0], "fastcap");
	*argc = 1;
	
	// then generate all arguments
	
	// filename
	//
	// check that file name is not void or composed only of spaces
	len = sscanf((LPCTSTR)m_sInputFileName, "%s", argStr);
	if(sscanf((LPCTSTR)m_sInputFileName, "%s", argStr) != EOF) {

		// must specify +3 instead of +1 because there could be the '-l' chars
		// in front of file name
		pointers[*argc] = new char[ m_sInputFileName.GetLength() + 3];

		// blanks first char
		pointers[*argc][0] = '\0';
		// if it is a list file, prefix the '-l' to the file name
		if( m_bListFile == TRUE ) 
			strcpy(pointers[*argc], "-l");

		strcat(pointers[*argc], (LPCTSTR)m_sInputFileName);
		(*argc)++;
	}
	else {
		MessageBox( "No file name given!", "Warning", MB_ICONEXCLAMATION);
		delete pointers[0];
		return 0;
	}


	// other arguments
	//
	
	if( m_fGlobalPermittivity != 1.0f) {

		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-p%f", m_fGlobalPermittivity);
		(*argc)++;
	}

	if( m_sRemoveFromSolveList != _T("")  ) {

		pointers[*argc] = new char[m_sRemoveFromSolveList.GetLength() + 5];
		strcpy(pointers[*argc], "-rs");

		len = 0;
		strPnt = (char *) ((LPCTSTR)m_sRemoveFromSolveList);
		while (sscanf(strPnt, "%s%n", argStr, &skip) != EOF) {

			strcat(pointers[*argc], argStr);
			strcat(pointers[*argc], ",");

			strPnt += skip;
		}

		// remove last ','
		len = strlen(pointers[*argc]);
		(pointers[*argc])[len-1] = '\0'; 

		(*argc)++;
	}

	if( m_sRemoveFromInputList != _T("")  ) {

		pointers[*argc] = new char[m_sRemoveFromInputList.GetLength() + 5];
		strcpy(pointers[*argc], "-ri");

		len = 0;
		strPnt = (char *) ((LPCTSTR)m_sRemoveFromInputList);
		while (sscanf(strPnt, "%s%n", argStr, &skip) != EOF) {

			strcat(pointers[*argc], argStr);
			strcat(pointers[*argc], ",");

			strPnt += skip;
		}

		// remove last ','
		len = strlen(pointers[*argc]);
		(pointers[*argc])[len-1] = '\0'; 

		(*argc)++;
	}


	if( m_sPartitioningDepth != "auto") {
		pointers[*argc] = new char[23];
		strcpy(pointers[*argc], "-d");
		strcat(pointers[*argc], (LPCTSTR)m_sPartitioningDepth);
		(*argc)++;
	}

	if( m_uiMultipoleOrder != 2) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-o%u", m_uiMultipoleOrder);
		(*argc)++;
	}

	if( m_fIterationTolerance != 0.01f) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-t%f", m_fIterationTolerance);
		(*argc)++;
	}

	if( m_bLineDrawingMode != FALSE) {
		pointers[*argc] = new char[3];
		strcpy(pointers[*argc], "-m");
		(*argc)++;
	}

	if( m_fAzimuth != 50.0f) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-a%f", m_fAzimuth);
		(*argc)++;
	}

	if( m_fDistance != 2.0f) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-h%f", m_fDistance);
		(*argc)++;
	}

	if( m_sAxesLength != "none") {
		pointers[*argc] = new char[13];
		strcpy(pointers[*argc], "-x");
		strcat(pointers[*argc], (LPCTSTR)m_sAxesLength);
		(*argc)++;
	}

	if( m_fRotation != 0.0f) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-r%f", m_fRotation);
		(*argc)++;
	}

	if( m_fScale != 1.0f) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-s%f", m_fScale);
		(*argc)++;
	}

	if( m_sUpAxis != "z") {
		pointers[*argc] = new char[13];
		strcpy(pointers[*argc], "-u");
		strcat(pointers[*argc], (LPCTSTR)m_sUpAxis);
		(*argc)++;
	}

	if( m_fElevation != 50.0f) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-e%f", m_fElevation);
		(*argc)++;
	}

	if( m_fLineWidth != 1.0f) {
		pointers[*argc] = new char[13];
		sprintf(pointers[*argc], "-w%f", m_fLineWidth);
		(*argc)++;
	}

	if( m_sRemoveConductors != _T("")  ) {

		pointers[*argc] = new char[m_sRemoveConductors.GetLength() + 5];
		strcpy(pointers[*argc], "-rc");

		len = 0;
		strPnt = (char *) ((LPCTSTR)m_sRemoveConductors);
		while (sscanf(strPnt, "%s%n", argStr, &skip) != EOF) {

			strcat(pointers[*argc], argStr);
			strcat(pointers[*argc], ",");

			strPnt += skip;
		}

		// remove last ','
		len = strlen(pointers[*argc]);
		(pointers[*argc])[len-1] = '\0'; 

		(*argc)++;
	}

	if( m_bRemoveDielectric != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-rd");
		(*argc)++;
	}

	// warinig: due to an initial misunderstanding, the var name has still
	// the plural 's', but the figure file is unique!
	if( m_sFigureFiles != _T("")  ) {

		pointers[*argc] = new char[m_sFigureFiles.GetLength()+3];
		strcpy(pointers[*argc], "-b");
		strcat(pointers[*argc], (LPCTSTR)m_sFigureFiles);
		(*argc)++;
	}

	if( m_bChargeDistribPics != FALSE  ) {

		pointers[*argc] = new char[m_sChargeDistribPics.GetLength() + 5];
		strcpy(pointers[*argc], "-q");

		len = 0;
		iscomma = FALSE;
		strPnt = (char *) ((LPCTSTR)m_sChargeDistribPics);
		while (sscanf(strPnt, "%s%n", argStr, &skip) != EOF) {

			strcat(pointers[*argc], argStr);
			strcat(pointers[*argc], ",");
			iscomma = TRUE;

			strPnt += skip;
		}

		// remove last ',', if any
		if(iscomma == TRUE) {
			len = strlen(pointers[*argc]);
			(pointers[*argc])[len-1] = '\0'; 
		}

		(*argc)++;
	}

	if( m_bMakeTotalChargePics != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-dc");
		(*argc)++;
	}

	if( m_bPutCommandLineInPS != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-c");
		(*argc)++;
	}

	if( m_bRemoveShadingKey != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-rk");
		(*argc)++;
	}

	if( m_bNumberFaces != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-n");
		(*argc)++;
	}

	if( m_bRemoveShowpage != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-v");
		(*argc)++;
	}

	if( m_bPrintGraph != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-g");
		(*argc)++;
	}

	if( m_bSuppressHiddenLines != FALSE) {
		pointers[*argc] = new char[4];
		strcpy(pointers[*argc], "-f");
		(*argc)++;
	}

	if( m_bDumpTimeAndMem != FALSE) {
		pointers[*argc] = new char[3];
		strcpy(pointers[*argc], "-z");
		(*argc)++;
	}

	// in the end, copy all the options to the program arguments
	*argv = new char*[*argc];
		
	for (i=0; i<*argc; i++)
		(*argv)[i] = pointers[i];

	return 1;
}

