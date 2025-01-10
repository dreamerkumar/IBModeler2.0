#ifndef LOOKINGATDLG_H_
#define LOOKINGATDLG_H_

#include<afxwin.h>
#include<afxcmn.h>
#include "definitions.h"

/****************************Dialog class to change the camera position**********************/
//IDD_DIALOG17 
class lookingAtDlg : public CDialog {
public:
	CAM_ORIENTATION camOrientation;

	lookingAtDlg(int n, CAM_ORIENTATION inCamOrientation,FLTPOINT3D inCuboidCntr);

	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	void OnOK();
	void OnCancel();
	CAM_ORIENTATION getCamOrientation();
private:
	//Variables ---------------------------------------------------------------------------
	CEdit *edit1, *edit10, *edit14, *edit15, *edit16;
	
	CStatic *st29, *st30, *st31, *st33, *st34, *st35, *st40;
	
	CComboBox cmb2, cmb3;

	CSpinButtonCtrl *spin2, *spin4;

	FLTPOINT3D cuboidCntr;
	
	int intDrtnRtnAngle;
	int intDrtnRtnMin;
	//-------------------------------------------------------------------------------------
	void assignDefltValues(); 

	void calculateDrtnCood();
		
	void getAngleRadius(FLTPOINT3D cntrOfRtn, FLTPOINT3D freeEnd, int &intAngle, 
		int &intAngleMin, float &fltRadius, BOOL blnXAxis);	
	
	void getCoordinates(FLTPOINT3D cntrOfRtn, FLTPOINT3D &freeEnd, int intAngle, 
		int intAngleMin, BOOL blnXAxis);
		
	void selDrtn_Cood();
	void selDrtn_Rotate();
	void selDrtn_Point();
	void selDrtnRtnAxis();
	void selDrtnPointTo();	
	
	BOOL blnUpdtControl; //checked by all the update functions before making any updations
						  
	void updateDisplay_E1(); 
	void updateDisplay_E10(); 
	void updateDisplay_E14(); 
	void updateDisplay_E15(); 
	void updateDisplay_E16(); 

	void updateControl(int intCntrlID,int intVal = 0, float fltVal = 0.0f); 
	
	void vectorToCood(VECTOR r, FLTPOINT3D &p);

	DECLARE_MESSAGE_MAP();
};

#endif