#ifndef CAMPOSITIONDLG_H_
#define CAMPOSITIONDLG_H_

#include<afxwin.h>
#include<afxcmn.h>
#include "definitions.h"




/****************************Dialog class to change the camera position**********************/
//IDD_DIALOG6
class camPositionDlg : public CDialog {
public:
	

	camPositionDlg(int n, CAM_ORIENTATION inCamOrientation,
							   FLTPOINT3D inCuboidCntr, 
							   FLTPOINT3D inDefltCamPos);

	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	void OnOK();
	void OnCancel();
	CAM_ORIENTATION getCamOrientation();
private:
	
	//Variables-----------------------------------------------------------------------------
	CComboBox cmb1;

	CSpinButtonCtrl *spin5, *spin6;

	CEdit *edit2, *edit11, *edit17, *edit18, *edit19, *edit20;
	
	CStatic *st17, *st19, *st22, *st23, *st24, *st25, 	*st26;

	CAM_ORIENTATION camOrientation;

	FLTPOINT3D cuboidCntr, defltCamPos;
	
	int intPosRtnAngle;
	int intPosRtnMin;
	float fltPosRtnRadius;
	BOOL blnUpdtControl; //checked by all the update functions before making any updations
	//---------------------------------------------------------------------------------------
	
	void assignDefltValues(); 

	void calculatePosCood();
		
	void getAngleRadius(FLTPOINT3D cntrOfRtn, FLTPOINT3D freeEnd, int &intAngle, 
		int &intAngleMin, float &fltRadius, BOOL blnXAxis);	
	
	void getCoordinates(FLTPOINT3D cntrOfRtn, FLTPOINT3D &freeEnd, int intAngle, 
		int intAngleMin, BOOL blnXAxis);
	
	void selPosRtnAxis();
	void selPos_Cood();
	void selPos_Rotate();
							  
	void updateDisplay_E2(); 
	void updateDisplay_E11(); 
	void updateDisplay_E17(); 
	void updateDisplay_E18(); 
	void updateDisplay_E19(); 
	void updateDisplay_E20(); 

	void updateControl(int intCntrlID,int intVal = 0, float fltVal = 0.0f); 
	
	void vectorToCood(VECTOR r, FLTPOINT3D &p);


	DECLARE_MESSAGE_MAP();
};

#endif