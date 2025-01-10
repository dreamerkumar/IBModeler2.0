#ifndef DIALOGS_H_
#define DIALOGS_H_

#include<afxwin.h>
#include<afxcmn.h>
#include"resource.h"
#include "definitions.h"
#include "hsl.h"

//View window for the splash screen
class splashView : public CWnd {
private:
	HBITMAP m_hImage;
	CBitmap m_bitmap;
	CDC m_memDC;
	int m_intWidth, m_intHeight;
	BOOL m_blnShowImage;

public :
	void OnPaint();
	int OnCreate(LPCREATESTRUCT l);
	void OnDestroy();
	void OnTimer() ;
	DECLARE_MESSAGE_MAP()
};
//--------------------------------


//Dialog window for the initial splash screen---------------------
class dlgSplash : public CDialog
{
private:
	splashView m_view;
public:
	dlgSplash(int n);
	int OnInitDialog();
	void OnTimer();
	
	DECLARE_MESSAGE_MAP()
};
//---------------------------------------------------------------

//Dialog window for the about box--------------------------------
class dlgAbout : public CDialog
{
public:
	dlgAbout(int n);
	int OnInitDialog();	
};
//---------------------------------------------------------------

class dlgCreateMold : public CDialog
{
private:
	CSpinButtonCtrl *spin1, *spin2, *spin3;
	CEdit *edit1, *edit2, *edit3, *edit4, *edit5, *edit6, *edit7, *edit8, *edit9;
	CStatic *st25, *st27;
	BOOL flgSt27Created;

public:

	dlgCreateMold(int n);
	
	BOOL flgOKBtn;
	BOOL flgUsingSnaps;
	
	UINT ptDensityX, ptDensityY, ptDensityZ;//Points per 10 cm( Range: 1-200)
	float minx, maxx, miny, maxy, minz, maxz;//Ranges for the mold

	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	void OnOK();
	void OnCancel();

	void updtStatusForSnaps();
	void updtStatusForSlides();

	void updtFileSizeInfo();

	CString updtVariables();
	
	DECLARE_MESSAGE_MAP()
};
/*********************************Yes/No dialog box******************************************/
class dlgYesNo : public CDialog {
private:
	CString message;
	CStatic *st;
	BOOL flgCreated;
	BOOL blnYesBtn;
void OnPaint();
public:
	dlgYesNo(int n, CString m, BOOL inBlnYesBtn);
	int OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

/***************************Class used by dlgChangeMarkerSize********************************/
class previewWindow : public CStatic {
private :

public :
	int markerSize;
	void OnPaint();

	DECLARE_MESSAGE_MAP()
};


/*************************Dialog box to alter the size of marker*****************************/
class dlgChangeMarkerSize : public CDialog {
private:
	
	previewWindow prWin;
	CScrollBar scr;
	CStatic *stPixelSize;
	UINT size;
	void updateDisplay();
public:
	dlgChangeMarkerSize(int n, UINT mkSize);
	int OnInitDialog();
	void OnHScroll(UINT code, UINT pos, CScrollBar *scroll);
	UINT getMarkerSize();
	DECLARE_MESSAGE_MAP()
};


/***************************Class used by dlgSelectBkColors********************************/
class previewWindow1 : public CWnd {
private :

public :
	BYTE btRed, btGreen, btBlue;
	BOOL blnShowEnabled;
	BOOL blnShowColor;
	void OnPaint();

	DECLARE_MESSAGE_MAP()
};


//***************Class used to select a range of colors to set as background*****************

class dlgSelectBkColors : public CDialog {

private:
	previewWindow1 prWin1, prWin2;
	BYTE btRed1, btGreen1, btBlue1, btHue, btSat1, btSat2, btLum1, btLum2;
	CScrollBar scrSat, scrLum;
	CButton *rdoSelClr;
	CButton *rdoBrightClrs;
	CButton *rdoDarkClrs;
	COLOR_SELECTION_TYPE selectionType;
	CHSL myHsl;
	BOOL blnColor2Selected;

public:
	dlgSelectBkColors(int n, BYTE inBtRed, BYTE inBtGreen, BYTE inBtBlue);
	int OnInitDialog();
	void OnVScroll(UINT code, UINT pos, CScrollBar *scroll);
	void getSelectedColors(BYTE &outBtRed1,BYTE &outBtGreen1,BYTE &outBtBlue1, 
		COLOR_SELECTION_TYPE &outSelectionType, BYTE &outBtSatLumVal);
	void updateColor2Display();
	void defineColorRange();
	void selectLum();
	void selectSat();
	void initializeColor2();
	void changeColor1();

	void selectClr()		{ selectionType = NONE; }
	void selectBrightClrs() { selectionType = CUR_AND_BRIGHTER; }
	void selectDarkClrs()	{ selectionType = CUR_AND_DARKER; }

	DECLARE_MESSAGE_MAP()
};

/****************************Dialog to specify camera parameters*****************************/

class dlgGetCameraInfo : public CDialog {

private:
	CStatic *stInfinityHt,*stInfinityWt;
	CStatic *stHtFrame, *stHtText1, *stHtText2, *stHtText3, *stHtText4;
	CStatic *stWtFrame, *stWtText1, *stWtText2, *stWtText3, *stWtText4;
	CEdit *editHtD1, *editHt1, *editHtD2, *editHt2, *editWtD1, *editWt1, *editWtD2, *editWt2;
	CEdit *editInfinityHt,*editInfinityWt;
	CButton *chkAtInfinity, *chkWtSameAsHt;

	BOOL blnSetParamsOnly;
	
public:
	CAMERA_INFO outCameraInfo;
	dlgGetCameraInfo(int n,CAMERA_INFO inCameraInfo, BOOL inBlnSetParamsOnly );	
	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);	
	void OnCancel();
	void OnOK();
	void loadFromFile();
	void updtStatus_Infinity();
	void updtStatus_DiffWidth();
	
	DECLARE_MESSAGE_MAP()
};


/******************Generic dialog to call worker thread**************************************/

class dlgCallThread : public CDialog {

private:
	void displayElapsedTime();
	clock_t clkStart;
	CStatic *stPercent, *stTime, *stHeading;
	CProgressCtrl *pgr;
	int percentLast;

	UINT m_nTimer;
	CWinThread *t;
	PROCESS_INFO *ptr;
	THREAD_OPERATION_TYPE oprtnType;
	unsigned long g_lngExitCode;
//	BOOL blnCancelled;
	BOOL g_blnCancelClicked;
public:	
	BOOL blnSuccess;
	
	dlgCallThread(int n,THREAD_OPERATION_TYPE inOprtnType, PROCESS_INFO *info);	
	int OnInitDialog();
	void OnCancel();
	void OnTimer(UINT nIDEvent);

	unsigned long getExitCode() { return g_lngExitCode;}

	BOOL isCancelled() { 
		if(g_lngExitCode == 0 ) //Even if user clicked on cancel, the operation was complete
			return false;
		else		
			return ptr->isCancelled();
	}

	DECLARE_MESSAGE_MAP()
};

/*****************************Dialog class to specify the range of mold points for the model*/
class pointRangeDlg : public CDialog {
public:
	pointRangeDlg(int n, UINT inIntXPts, UINT inIntYPts, UINT inIntZPts);
	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	void getPointRange(UINT &outIntMinx, UINT &outIntMaxx, UINT &outIntMiny, 
		UINT &outIntMaxy, UINT &outIntMinz, UINT &outIntMaxz);

	
private:
	CStatic *st4, *st5, *st30, *st29, *st42, *st41;
	CEdit *edit1, *edit10, *edit15, *edit16, *edit17, *edit18;	
	
	UINT intXPts, intYPts, intZPts;
	UINT intMinx, intMaxx, intMiny, intMaxy, intMinz, intMaxz;
	BOOL blnProcessAllX, blnProcessAllY, blnProcessAllZ;

	void setProcessAllX();
	void setPointRangeX();
	void setProcessAllY();
	void setPointRangeY();
	void setProcessAllZ();
	void setPointRangeZ();

	void setControlsX(BOOL blnStatus);
	void setControlsY(BOOL blnStatus);
	void setControlsZ(BOOL blnStatus);
	void OnOK();
	BOOL validateRange();
	DECLARE_MESSAGE_MAP()
	
};

//***************Class used to select the model color*****************************************

class dlgModelColor : public CDialog {

private:
	previewWindow1 prWin;
	RGBCOLOR clr;
public:
	dlgModelColor(int n, RGBCOLOR inClr);
	int OnInitDialog();
	void getColorValues(RGBCOLOR &outClr);
	void chooseColor();

	DECLARE_MESSAGE_MAP()
};

//***************Dialog class to change the rotation speed of model***************************
class dlgModelRtnSpeed : public CDialog {

private:
	BYTE btSeconds;
	CListBox *list;
public:
	dlgModelRtnSpeed(int n, BYTE inBtSeconds);
	int OnInitDialog();
	BYTE getSeconds();
	void OnOK();
};

//***************Dialog class to change the view distance while viewing the model************
class dlgModelViewDt: public CDialog {

private:
	float fltViewDt;
	float fltMaxCoodVal;
	void setDefault();
public:
	dlgModelViewDt(int n, float inFltViewDt, float inFltMaxCoodVal);
	//int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	float getViewDt();


	
	DECLARE_MESSAGE_MAP()
};

//***************************Dialog class to show model info**********************************
class dlgModelInfo: public CDialog {

private:
	MODEL_INFO modelInfo;
public:
	dlgModelInfo(int n, MODEL_INFO inModelInfo);
	int OnInitDialog();
	
};

//***************************Dialog class to show keyboard shortcuts**************************
class dlgKeyboardMap : public CDialog {

private:
	KEYBOARD_MAP_TYPE mapType;
	CListCtrl list;
public:
	dlgKeyboardMap(int n, KEYBOARD_MAP_TYPE inMapType);
	int OnInitDialog();	
};
/**************Dialog class to alter the number of points along the X and Y ****************/
/***************************** axes for the slides option **********************************/
enum SELECTION_TYPE {
	FIRST, 
	SECOND, 
	THIRD
};

class alterPointsDlg : public CDialog {

//Variables
private:
	CStatic *st4, *st7, *st8, *st9, *st10, *st11, *st13, *st14;
	CEdit *edit1, *edit2;
	CComboBox combo;
	
	UINT c_uintCurrentX, c_uintCurrentY, c_uintImgWidth, c_uintImgHeight;

	UINT c_uintNewX, c_uintNewY;

	SELECTION_TYPE selectionType;

//Functions
public:
	alterPointsDlg(int n, UINT inUintCurrentX, UINT inUintCurrentY, 
		UINT inUintImgWidth,UINT inUintImgHeight);

	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	void getNewPoints(UINT &outUintX, UINT  &outUintY);

private:
	void setControls(BYTE inBtOption);
	void setControls_1();
	void setControls_2();
	void setControls_3();
	void onChangeOfN();

	DECLARE_MESSAGE_MAP()	
};

#endif 