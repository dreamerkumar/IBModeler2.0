#include "lookingatdlg.h"
#include "resource.h"
#include "math.h"

/*==========================================================================================*/
/********************************lookingAtDlg********************************************/
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Initializing functions/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
lookingAtDlg::lookingAtDlg(int n, CAM_ORIENTATION inCamOrientation, 
	FLTPOINT3D inCuboidCntr) : CDialog(n) {
	
	blnUpdtControl = false; //Set to true once all the controls are initialized
	
	camOrientation = inCamOrientation;
	
	cuboidCntr = inCuboidCntr;

	if(!camOrientation.blnDataInitialized) 
		assignDefltValues();

	intDrtnRtnAngle = intDrtnRtnMin = 0;

}

int lookingAtDlg::OnInitDialog() {
	
	cmb2.CreateEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | 
		CBS_DROPDOWNLIST | WS_VSCROLL, CRect(57, 107, 173, 750), this, 51);

	cmb2.AddString("Y axis");
	cmb2.AddString("X axis");

	cmb2.SetCurSel(0);

	cmb3.CreateEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | 
		CBS_DROPDOWNLIST | WS_VSCROLL, CRect(30, 190, 173, 240), this, 52);
	//Note: The drop down for this combo box has been kept small so that it is always within,
	//the dialog window. This is necessary because if it goes outside, display problems could
	//occur because the display regions outside the dialog box are not updated as long as the
	//focus is on the dialog box.

	cmb3.AddString("Center of the cube");
	cmb3.AddString("Origin");

	cmb3.SetCurSel(0);
	
	//Get the handles to the controls==================
	edit1= (CEdit *)GetDlgItem(IDC_EDIT1);
	edit10= (CEdit *)GetDlgItem(IDC_EDIT10);
	edit14= (CEdit *)GetDlgItem(IDC_EDIT14);
	edit15= (CEdit *)GetDlgItem(IDC_EDIT15);
	edit16= (CEdit *)GetDlgItem(IDC_EDIT16);

	st29= (CStatic *)GetDlgItem(IDC_STATIC29);
	st30= (CStatic *)GetDlgItem(IDC_STATIC30);
	st31= (CStatic *)GetDlgItem(IDC_STATIC31);
	st33= (CStatic *)GetDlgItem(IDC_STATIC33);
	st34= (CStatic *)GetDlgItem(IDC_STATIC34);
	st35= (CStatic *)GetDlgItem(IDC_STATIC35);


	spin2 = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN2); 
	spin4 = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN4);

	

	switch(camOrientation.camDrtnSelType) {
	
	case DRTN_COORDINATES :
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		selDrtn_Cood();
		break;

	case DRTN_ROTN_X_AXIS:
	case DRTN_ROTN_Y_AXIS:
		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);
		selDrtn_Rotate();
		break;
	
	default:
		((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
		selDrtn_Point();
		break;
	} 		
	
	CDialog::OnInitDialog();
	
	spin2->SetRange(0, 360); 
	spin4->SetRange(0, 60);

	blnUpdtControl = true;	
	return TRUE;
}

void lookingAtDlg::DoDataExchange(CDataExchange *pdx) {

	CDialog::DoDataExchange(pdx);


	if(camOrientation.camDrtnSelType == DRTN_COORDINATES) {
		
		DDX_Text(pdx, IDC_EDIT1, camOrientation.lookingAt.x);
		DDV_MinMaxFloat(pdx, camOrientation.lookingAt.x, -50000.00f, 50000.00f);

		DDX_Text(pdx, IDC_EDIT10, camOrientation.lookingAt.y);
		DDV_MinMaxFloat(pdx, camOrientation.lookingAt.y, -50000.00f, 50000.00f);

		DDX_Text(pdx, IDC_EDIT14, camOrientation.lookingAt.z);
		DDV_MinMaxFloat(pdx, camOrientation.lookingAt.z, -50000.00f, 50000.00f);
	
	} else if (camOrientation.camDrtnSelType == DRTN_ROTN_X_AXIS 
		|| camOrientation.camDrtnSelType == DRTN_ROTN_Y_AXIS) {
		
		DDX_Text(pdx, IDC_EDIT15, intDrtnRtnAngle);
		DDV_MinMaxUInt(pdx, intDrtnRtnAngle, 0, 360);

		DDX_Text(pdx, IDC_EDIT16, intDrtnRtnMin);
		DDV_MinMaxUInt(pdx, intDrtnRtnMin, 0, 60);
	}	
}
//////////////////////////////////Initializing functions///////////////////////////////////////
//******************************//////////////////////////***********************************//


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
///////FUNCTIONS TO CHANGE THE COORDINATES OF THE POINT TO WHICH THE CAMERA IS POINTING///////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void lookingAtDlg::updateDisplay_E1(){ 
	if(!blnUpdtControl)
		return;
	CString str;
	edit1->GetWindowText(str);
	camOrientation.lookingAt.x = (float)atof(str);
	 
}

void lookingAtDlg::updateDisplay_E10(){ 
	if(!blnUpdtControl)
		return;
	CString str;
	edit10->GetWindowText(str);
	camOrientation.lookingAt.y = (float)atof(str);
	 
}

void lookingAtDlg::updateDisplay_E14(){ 
	if(!blnUpdtControl)
		return;
	CString str;
	edit14->GetWindowText(str);
	camOrientation.lookingAt.z = (float)atof(str);
	 
}

void lookingAtDlg::updateDisplay_E15(){ 
	if(!blnUpdtControl)
		return;
	CString str;
	edit15->GetWindowText(str);
	intDrtnRtnAngle = atoi(str);
	
	calculateDrtnCood();
}

void lookingAtDlg::updateDisplay_E16(){ 
	if(!blnUpdtControl)
		return;
	CString str;
	edit16->GetWindowText(str);
	intDrtnRtnMin = atoi(str);

	calculateDrtnCood();
}

void lookingAtDlg::selDrtn_Cood() {
	
	st29->EnableWindow(TRUE);
	edit1->EnableWindow(TRUE);
	st30->EnableWindow(TRUE);
	edit10->EnableWindow(TRUE);
	st31->EnableWindow(TRUE);
	edit14->EnableWindow(TRUE);

	st33->EnableWindow(FALSE);
	cmb2.EnableWindow(FALSE);
	st34->EnableWindow(FALSE);
	edit15->EnableWindow(FALSE);
	spin2->EnableWindow(FALSE);
	st35->EnableWindow(FALSE);
	edit16->EnableWindow(FALSE);
	spin4->EnableWindow(FALSE);
	cmb3.EnableWindow(FALSE);
	
	camOrientation.camDrtnSelType = DRTN_COORDINATES;
	updateControl(IDC_EDIT1, 0, camOrientation.lookingAt.x);
	updateControl(IDC_EDIT10,0, camOrientation.lookingAt.y);
	updateControl(IDC_EDIT14,0, camOrientation.lookingAt.z);
	
	 	
	edit1->SetFocus();
}

void lookingAtDlg::selDrtn_Rotate() {

	st29->EnableWindow(FALSE);
	edit1->EnableWindow(FALSE);
	st30->EnableWindow(FALSE);
	edit10->EnableWindow(FALSE);
	st31->EnableWindow(FALSE);
	edit14->EnableWindow(FALSE);

	st33->EnableWindow(TRUE);
	cmb2.EnableWindow(TRUE);
	st34->EnableWindow(TRUE);
	edit15->EnableWindow(TRUE);
	spin2->EnableWindow(TRUE);
	st35->EnableWindow(TRUE);
	edit16->EnableWindow(TRUE);
	spin4->EnableWindow(TRUE);

	cmb3.EnableWindow(FALSE);
	
	switch(camOrientation.camDrtnSelType) {
	case DRTN_ROTN_Y_AXIS: 
		cmb2.SetCurSel(0);
		break;
	case DRTN_ROTN_X_AXIS :
		cmb2.SetCurSel(1);
		break;
	default:
		camOrientation.camDrtnSelType = DRTN_ROTN_Y_AXIS;
		cmb2.SetCurSel(0);
		break;
	}
	
	selDrtnRtnAxis(); //Calculates the angles and change the display
}

void lookingAtDlg::selDrtnRtnAxis() {
	
	int intIndex = cmb2.GetCurSel();
	
	switch(intIndex) {
	case 0:
		camOrientation.camDrtnSelType = DRTN_ROTN_Y_AXIS;
		break;
	case 1:
		camOrientation.camDrtnSelType = DRTN_ROTN_X_AXIS;
		break;
	}
	
	BOOL blnXAxis;

	if(intIndex == 1)
		blnXAxis = true;
	else
		blnXAxis = false;

	float fltRadius;
	
	getAngleRadius(camOrientation.camPos, camOrientation.lookingAt, intDrtnRtnAngle, 
		intDrtnRtnMin, fltRadius, blnXAxis);
	
	//update the controls with the angle values
	updateControl(IDC_EDIT15, intDrtnRtnAngle);
	updateControl(IDC_EDIT16, intDrtnRtnMin);
	
	 
	//spin2->SetFocus(); The function is also called on update of cam pos values, so should not
	//set focus here 
}

void lookingAtDlg::calculateDrtnCood() {
	
	BOOL blnXAxis;
	
	if(camOrientation.camDrtnSelType == DRTN_ROTN_X_AXIS)
		blnXAxis = true;
	else
		blnXAxis = false;

	getCoordinates(camOrientation.camPos, camOrientation.lookingAt, 
		intDrtnRtnAngle, intDrtnRtnMin, blnXAxis);
	
	 
}

//selDrtn_Point and selDrtnPointTo functions :Used when the option of  pointing to is selected.
void lookingAtDlg::selDrtn_Point() {

	st29->EnableWindow(FALSE);
	edit1->EnableWindow(FALSE);
	st30->EnableWindow(FALSE);
	edit10->EnableWindow(FALSE);
	st31->EnableWindow(FALSE);
	edit14->EnableWindow(FALSE);

	st33->EnableWindow(FALSE);
	cmb2.EnableWindow(FALSE);
	st34->EnableWindow(FALSE);
	edit15->EnableWindow(FALSE);
	spin2->EnableWindow(FALSE);
	st35->EnableWindow(FALSE);
	edit16->EnableWindow(FALSE);
	spin4->EnableWindow(FALSE);

	cmb3.EnableWindow(TRUE);
	
	switch(camOrientation.camDrtnSelType) { 

	case DRTN_CUBE_CNTR:
		cmb3.SetCurSel(0);
		break;
	case DRTN_ORGN:
		cmb3.SetCurSel(1);
		break;
	default:
		cmb3.SetCurSel(0);		
		break;
	}

	selDrtnPointTo();
	cmb3.SetFocus();
}

void lookingAtDlg::selDrtnPointTo() {
	
	int intSelection = cmb3.GetCurSel();
	switch(intSelection) {
	case 0: //point cuboid center
		camOrientation.camDrtnSelType = DRTN_CUBE_CNTR;
		camOrientation.lookingAt.x = cuboidCntr.x;
		camOrientation.lookingAt.y = cuboidCntr.y;
		camOrientation.lookingAt.z = cuboidCntr.z;
		break;
	case 1: //point to origin
		camOrientation.camDrtnSelType = DRTN_ORGN;
		camOrientation.lookingAt.x =  camOrientation.lookingAt.y =  
			camOrientation.lookingAt.z =  0.0f;
		break;
	}	
	 
}

// END OF FUNCTIONS TO CHANGE THE COORDINATES OF THE POINT TO WHICH THE CAMERA IS POINTING ////
//******************************//////////////////////////***********************************//



///////////////////////////////////////////////////////////////////////////////////////////////
///// getAngleRadius: Called whenever the option of rotation of the camera ////////////////////
///// position around any of the axis is chosen. Gives back the angle, and ////////////////////
/////               radius around the center of rotation                   ////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void lookingAtDlg::getAngleRadius(FLTPOINT3D cntrOfRtn, FLTPOINT3D freeEnd, 
								int &intAngle, int &intAngleMin, float &fltRadius, 
								BOOL blnXAxis) {
	
	double PI = 22.0/7.0;
	
	double dblAngle;
	
	VECTOR r = VECTOR (freeEnd) - VECTOR (cntrOfRtn);
	
	fltRadius = r.modulus();
	
	VECTOR r1;

	if(blnXAxis) 
		r1 = VECTOR(0.0f, r.j, r.k); //Vector in the y, z plane
	else
		r1 = VECTOR (r.i, 0.0f, r.k); //Vector in the x, z plane
	
	float fltR1Mod = r1.modulus();
	
	//Check for zero radius
	if(fltR1Mod == 0.0f) {
		intAngle = intAngleMin = 0;
		fltRadius = 0.0f;
		return;
	}

	/////////////////////////////////////NOTES//////////////////////////////////////////
	/*  1. The rotation angle has to be measured always with respect to the z axis        
	    2. We calculate the angle with respect to this axis by first getting the sine  
		   of the angle.                                              
	    3. To get the sine of the angle, we should divide the cross product of k and r1 
		   by the modulus of r1 vector
	    4. Then, we use asin to get the angle in radians.
		5. Asin returns values between -pi/2 to + pi/2. We then need to convert to a 
		   value between 0 and 2PI. */
	
	if(blnXAxis) {
		//Rotation is about the X axis

		double dblSinVal = (double)((VECTOR(0.0f, 0.0f, 1.0f) * r1).i/fltR1Mod);	
				
		if(dblSinVal <= 0.0 ) {
			//0 < Angle <= pi
			dblAngle = asin( - dblSinVal); 	
			if(r1.k < 0 ) //pi/2 <= Angle <= pi
				dblAngle = PI - dblAngle;

		} else {
			//pi < Angle < 2pi
			dblAngle = asin( dblSinVal); 	
			if(r1.k <= 0) 
				dblAngle = PI + dblAngle;
			else 
				dblAngle = 2*PI - dblAngle;
		}

	} else {
		
		double dblSinVal = (double)((VECTOR(0.0f, 0.0f, 1.0f) * r1).j/fltR1Mod);

		//Rotation is about the Y axis	
		
		if(dblSinVal >= 0.0 ) {
			//0 < Angle <= pi
			dblAngle = asin(dblSinVal); 	
			if(r1.k < 0 ) //pi/2 < Angle < pi
				dblAngle = PI - dblAngle;
		} else {
			//pi < Angle < 2pi
			dblAngle = asin( - dblSinVal); 	
			if(r1.k <= 0) 
				dblAngle = PI + dblAngle;
			else 
				dblAngle = 2*PI - dblAngle;
		}	
	}

	///////////////////////////////////////////////////////////////////////////////////////
	
	//Convert to degrees and minutes (PI <=> 180 degree)
	double dblDegrees = (180.0 * dblAngle)/PI;

	intAngle = (int)dblDegrees; //There won't be any rounding off
	
	intAngleMin = (int) ((dblDegrees - (double)intAngle) * 60.0);

}

///////////////////////////////////End of getAngleRadius///////////////////////////////////////
//******************************//////////////////////////***********************************//
	

///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////getCoordinates////////////////////////////////////////////////////
/* Note: The passed value of freeEnd would be used to calculate the radius. Hence, we 
   should ensure that we are passing the right value through freeEnd. freeEnd is 
   therefore acting as a in as well as an out parameter.*/
///////////////////////////////////////////////////////////////////////////////////////////////
void lookingAtDlg::getCoordinates(FLTPOINT3D cntrOfRtn, FLTPOINT3D &freeEnd, 
									  int intAngle, int intAngleMin, BOOL blnXAxis) {
	
	double PI = 22.0/7.0;
	
	VECTOR vecCntrOfRtn = VECTOR (cntrOfRtn);
	
	VECTOR r = VECTOR (freeEnd) - vecCntrOfRtn;
	
	VECTOR r1;

	if(blnXAxis) 
		r1 = VECTOR(0.0f, r.j, r.k); //Vector in the y, z plane		
	else
		r1 = VECTOR (r.i, 0.0f, r.k); //Vector in the x, z plane

	float fltR1Mod = r1.modulus();
	
	double dblAngle =( ((double)intAngle  + (double)intAngleMin/60.0) * PI) /180.0;

	if(blnXAxis) {
		r.j =  (float)(fltR1Mod * sin(dblAngle));		
		r.k =  (float)(fltR1Mod * cos(dblAngle));
	
	} else { 
		r.i =  (float)(fltR1Mod * sin(dblAngle));		
		r.k =  (float)(fltR1Mod * cos(dblAngle));		
	}

	VECTOR r2 = r + vecCntrOfRtn;
	
	vectorToCood(r2, freeEnd);	
}
//////////////////////////////////End of getCoordinates////////////////////////////////////////
//******************************//////////////////////////***********************************//



//////////////////////End of Functions for updating the display type///////////////////////////
//******************************//////////////////////////***********************************//




///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////General functions//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void lookingAtDlg::assignDefltValues() {
	camOrientation.lookingAt = cuboidCntr;
}

void lookingAtDlg::vectorToCood(VECTOR r, FLTPOINT3D &p) {
	p.x = r.i;
	p.y = r.j;
	p.z = r.k;
}
void lookingAtDlg::updateControl(int intCntrlID, int intVal, float fltVal) {
	
	//While updating the values, the update function for other controls should not be called
	blnUpdtControl = false;
	
	CString str;
	
	switch(intCntrlID) {

	case IDC_EDIT1:
		str.Format("%f", fltVal);
		edit1->SetWindowText(str);
		break;
	case IDC_EDIT10:
		str.Format("%f", fltVal);
		edit10->SetWindowText(str);
		break;
	case IDC_EDIT14:
		str.Format("%f", fltVal);
		edit14->SetWindowText(str);
		break;
	case IDC_EDIT15:
		str.Format("%d", intVal);
		edit15->SetWindowText(str);
		break;
	case IDC_EDIT16:
		str.Format("%d", intVal);
		edit16->SetWindowText(str);
		break;
	}

	blnUpdtControl = true;
}

//////////////////////////////////End of general functions/////////////////////////////////////
//******************************//////////////////////////***********************************//


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////Functions called for closing the dialog box///////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void lookingAtDlg::OnCancel() {
	CDialog::OnCancel();
}

void lookingAtDlg::OnOK() {
	
	
	CDialog::OnOK();

	
	//If the validations have not failed and the window is not visible (EndDialog does not 
	//destroy the window, but makes it invisible)
	if(!IsWindowVisible()) 
		camOrientation.blnDataInitialized = true;

}

////////////////////////End of functions for closing the dialog box////////////////////////////
//******************************//////////////////////////***********************************//
CAM_ORIENTATION lookingAtDlg::getCamOrientation() {
	return camOrientation;
}

BEGIN_MESSAGE_MAP(lookingAtDlg, CDialog)
	
	ON_COMMAND(IDC_RADIO1, selDrtn_Cood)
	ON_COMMAND(IDC_RADIO2, selDrtn_Rotate)
	ON_COMMAND(IDC_RADIO3, selDrtn_Point)

	ON_CBN_SELCHANGE(51, selDrtnRtnAxis)
	ON_CBN_SELCHANGE(52, selDrtnPointTo)
	
	ON_EN_CHANGE(IDC_EDIT1,  updateDisplay_E1)
	ON_EN_CHANGE(IDC_EDIT10, updateDisplay_E10)
	ON_EN_CHANGE(IDC_EDIT14, updateDisplay_E14)
	ON_EN_CHANGE(IDC_EDIT15, updateDisplay_E15)
	ON_EN_CHANGE(IDC_EDIT16, updateDisplay_E16)

	ON_COMMAND(IDC_SPIN2, updateDisplay_E15)
	ON_COMMAND(IDC_SPIN4, updateDisplay_E16)
END_MESSAGE_MAP()


