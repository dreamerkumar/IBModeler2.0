#include "campositiondlg.h"
#include "resource.h"
#include "math.h"
/*==========================================================================================*/
/********************************camPositionDlg********************************************/
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Initializing functions/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void camPositionDlg::assignDefltValues() {
	camOrientation.camPos = defltCamPos;
	intPosRtnAngle = intPosRtnMin = 0;
	fltPosRtnRadius = 0.0f;

}

camPositionDlg::camPositionDlg(int n, CAM_ORIENTATION inCamOrientation,
							   FLTPOINT3D inCuboidCntr, 
							   FLTPOINT3D inDefltCamPos) : CDialog(n) {
	
	blnUpdtControl = false; //Set to true once all the controls are initialized
	
	camOrientation = inCamOrientation;

	cuboidCntr = inCuboidCntr;
	
	//Get the default value for the camera position
	defltCamPos = inDefltCamPos;
 	
	if(!camOrientation.blnDataInitialized) 
		assignDefltValues();

}

int camPositionDlg::OnInitDialog() {
	
	cmb1.CreateEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | 
		CBS_DROPDOWNLIST | WS_VSCROLL, CRect(67, 107, 183, 750), this, 50);

	cmb1.AddString("Y axis");
	cmb1.AddString("X axis");
	cmb1.AddString("Cube's Y axis ");
	cmb1.AddString("Cube's X axis ");

	cmb1.SetCurSel(0);
	

	//Get the handles to the controls==================
	edit2= (CEdit *)GetDlgItem(IDC_EDIT2);
	edit11= (CEdit *)GetDlgItem(IDC_EDIT11); 
	edit17= (CEdit *)GetDlgItem(IDC_EDIT17);
	edit18= (CEdit *)GetDlgItem(IDC_EDIT18);
	edit19= (CEdit *)GetDlgItem(IDC_EDIT19);
	edit20= (CEdit *)GetDlgItem(IDC_EDIT20);

	st17= (CStatic *)GetDlgItem(IDC_STATIC17);
	st19= (CStatic *)GetDlgItem(IDC_STATIC19);
	st22= (CStatic *)GetDlgItem(IDC_STATIC22);
	st23= (CStatic *)GetDlgItem(IDC_STATIC23); 
	st24= (CStatic *)GetDlgItem(IDC_STATIC24); 
	st25= (CStatic *)GetDlgItem(IDC_STATIC25); 
	st26= (CStatic *)GetDlgItem(IDC_STATIC26);

	spin5 = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN5); 
	spin6 = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN6); 

	//=================================================

	//Make the appropriate selections
	switch(camOrientation.camPosSelType) {
	
	case POS_COORDINATES:
		((CButton *)GetDlgItem(IDC_RADIO4))->SetCheck(TRUE);
		selPos_Cood();
		break;
	
	default:
		((CButton *)GetDlgItem(IDC_RADIO5))->SetCheck(TRUE);
		selPos_Rotate();
		break;
	}

	CDialog::OnInitDialog();
	
	spin5->SetRange(0, 360); 
	spin6->SetRange(0, 60); 

	blnUpdtControl = true;	
	return TRUE;
}

void camPositionDlg::DoDataExchange(CDataExchange *pdx) {

	CDialog::DoDataExchange(pdx);

	if(camOrientation.camPosSelType == POS_COORDINATES) {
		
		DDX_Text(pdx, IDC_EDIT2, camOrientation.camPos.x);
		DDV_MinMaxFloat(pdx, camOrientation.camPos.x, -50000.00f, 50000.00f);

		DDX_Text(pdx, IDC_EDIT11, camOrientation.camPos.y);
		DDV_MinMaxFloat(pdx, camOrientation.camPos.y, -50000.00f, 50000.00f);

		DDX_Text(pdx, IDC_EDIT17, camOrientation.camPos.z);
		DDV_MinMaxFloat(pdx, camOrientation.camPos.z, -50000.00f, 50000.00f);

	} else {
				
		DDX_Text(pdx, IDC_EDIT18, intPosRtnAngle);
		DDV_MinMaxUInt(pdx, intPosRtnAngle, 0, 360);

		DDX_Text(pdx, IDC_EDIT19, intPosRtnMin);
		DDV_MinMaxUInt(pdx, intPosRtnMin, 0, 60);

		DDX_Text(pdx, IDC_EDIT20, fltPosRtnRadius); //Validation for this field done separately
		//in the onok function
	}	

}
//////////////////////////////////Initializing functions///////////////////////////////////////
//******************************//////////////////////////***********************************//


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
///////////////// FUNCTIONS FOR CHANGING THE CAMERA POSITION CO-ORDINATES ////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void camPositionDlg::updateDisplay_E2(){ 
	
	if(!blnUpdtControl)
		return;
	
	CString str;
	edit2->GetWindowText(str);
	camOrientation.camPos.x = (float)atof(str);

	
}

void camPositionDlg::updateDisplay_E11(){ 
	
	if(!blnUpdtControl)
		return;
	
	CString str;
	edit11->GetWindowText(str);
	camOrientation.camPos.y = (float)atof(str);

	
}

void camPositionDlg::updateDisplay_E17(){ 

	if(!blnUpdtControl)
		return;
	
	CString str;
	edit17->GetWindowText(str);
	camOrientation.camPos.z = (float)atof(str);
	
	
}

void camPositionDlg::updateDisplay_E18() { 		
		
	if(!blnUpdtControl)
		return;
	
	CString str;
	edit18->GetWindowText(str);
	int intTemp = atoi(str);
	
	if(intTemp < 0 || intTemp > 360) {
		MessageBox("Angle should be between 0 and 360.", "Error:");
		return;
	}
	
	intPosRtnAngle = intTemp;

	calculatePosCood();
}

void camPositionDlg::updateDisplay_E19(){ 
	
	if(!blnUpdtControl)
		return;

	CString str;
	edit19->GetWindowText(str);
	intPosRtnMin = atoi(str);
	
	calculatePosCood();
}

void camPositionDlg::updateDisplay_E20() { //Called when the value of the radius is changed
	
	if(!blnUpdtControl)
		return;
	
	CString str;
	edit20->GetWindowText(str);
	float fltNewRadius = (float)atof(str);
	
	if(fltNewRadius <= 0.0f) {
		MessageBox("Radius should be greater than zero.", "Error:");			
		return;
	} else if(fltNewRadius > 49500.0f) {
		MessageBox("Radius should be less than or equal to 49500.", "Error:");
		return;
	}
	
	VERIFY(fltPosRtnRadius > 0.0f);		

	//Increase the radius value using ratio proportion

	VECTOR r1;
	
	if(camOrientation.camPosSelType == POS_ROTN_CUBE_X_AXIS 
		|| camOrientation.camPosSelType == POS_ROTN_CUBE_Y_AXIS)
		r1 = VECTOR(cuboidCntr);
	else
		r1 = VECTOR(0.0f, 0.0f, 0.0f); //Origin

	VECTOR r = VECTOR(camOrientation.camPos) - r1;
	
	r = r * fltNewRadius / fltPosRtnRadius;

	VECTOR r2 = r + r1;

	vectorToCood(r2, camOrientation.camPos); //Update the camera position

	fltPosRtnRadius = fltNewRadius; //Store the new radius


}

void camPositionDlg::selPos_Cood() {

	st17->EnableWindow(TRUE);
	edit2->EnableWindow(TRUE);
	st19->EnableWindow(TRUE);
	edit11->EnableWindow(TRUE);
	st22->EnableWindow(TRUE);
	edit17->EnableWindow(TRUE);

	st23->EnableWindow(FALSE);
	cmb1.EnableWindow(FALSE);
	st24->EnableWindow(FALSE);
	edit18->EnableWindow(FALSE);
	spin5->EnableWindow(FALSE);
	st25->EnableWindow(FALSE);
	edit19->EnableWindow(FALSE);
	spin6->EnableWindow(FALSE);
	st26->EnableWindow(FALSE);
	edit20->EnableWindow(FALSE);
	
	edit2->SetFocus();

	camOrientation.camPosSelType = POS_COORDINATES;	
	
	updateControl(IDC_EDIT2,0, camOrientation.camPos.x);
	updateControl(IDC_EDIT11,0, camOrientation.camPos.y);
	updateControl(IDC_EDIT17,0, camOrientation.camPos.z);

}

void camPositionDlg::selPos_Rotate() {

	st17->EnableWindow(FALSE);
	edit2->EnableWindow(FALSE);
	st19->EnableWindow(FALSE);
	edit11->EnableWindow(FALSE);
	st22->EnableWindow(FALSE);
	edit17->EnableWindow(FALSE);

	st23->EnableWindow(TRUE);
	cmb1.EnableWindow(TRUE);
	st24->EnableWindow(TRUE);
	edit18->EnableWindow(TRUE);
	spin5->EnableWindow(TRUE);
	st25->EnableWindow(TRUE);
	edit19->EnableWindow(TRUE);
	spin6->EnableWindow(TRUE);
	st26->EnableWindow(TRUE);
	edit20->EnableWindow(TRUE);

	switch(camOrientation.camPosSelType) {
	case POS_ROTN_Y_AXIS: 
		cmb1.SetCurSel(0);
		break;
	case POS_ROTN_X_AXIS :
		cmb1.SetCurSel(1);
		break;
	case POS_ROTN_CUBE_Y_AXIS  :
		cmb1.SetCurSel(2);
		break;
	case POS_ROTN_CUBE_X_AXIS:
		cmb1.SetCurSel(3);
		break;
	default:
		//Among the axes along which the rotation is possible, try to choose an option which 
		//is feasible
		if(camOrientation.camPos.x == 0.0f && camOrientation.camPos.y == 0.0f  
			&& camOrientation.camPos.z == 0.0f 
			&& (cuboidCntr.x != 0.0f || cuboidCntr.y != 0.0f  || cuboidCntr.z != 0.0f)) 
			//rotation about the origin is not possible, but rotation about the cuboid center
			//is possible
			cmb1.SetCurSel(2); //Rotation around the Y axis of the cuboid
		else 
			cmb1.SetCurSel(0); //Rotation around the Y axis 
		break;
	}
	
	selPosRtnAxis(); //Calculates the angles and change the display
}


void camPositionDlg::selPosRtnAxis() {
	
	int intIndex = cmb1.GetCurSel();
	
	if( ((intIndex == 0 || intIndex == 1) //Rotation about the X or Y axis
		&& camOrientation.camPos.x == 0.0f && camOrientation.camPos.y == 0.0f 
		&& camOrientation.camPos.z == 0.0f) //Camera located at origin
		|| //Rotation about the cuboid axes
		( intIndex > 1  && (camOrientation.camPos.x == cuboidCntr.x 
		&& camOrientation.camPos.y == cuboidCntr.y 
		&& camOrientation.camPos.z == cuboidCntr.z)) ) { //Camera at the cuboid center

		CString str = "The camera cannot be rotated from the current location\n" ;
		str = str +	"as it is located at the center of rotation.\n\n";
		str = str + "The position will be reset to the default value to facilitate rotation.";
							
		MessageBox(str, "Error:");
		assignDefltValues();		
	}
	
	switch(intIndex) {
	case 0:
		camOrientation.camPosSelType = POS_ROTN_Y_AXIS;
		break;
	case 1:
		camOrientation.camPosSelType = POS_ROTN_X_AXIS;
		break;
	case 2:
		camOrientation.camPosSelType = POS_ROTN_CUBE_Y_AXIS;
		break;
	case 3:
		camOrientation.camPosSelType = POS_ROTN_CUBE_X_AXIS;
		break;
	}
	
	FLTPOINT3D cntrOfRtn; //Initialized to the origin
	
	if(intIndex > 1) //rotation is about the cuboid center
		cntrOfRtn = cuboidCntr;

	BOOL blnXAxis;

	if(intIndex == 1 || intIndex == 3 )
		blnXAxis = true;
	else
		blnXAxis = false;

	getAngleRadius(cntrOfRtn, camOrientation.camPos, intPosRtnAngle, intPosRtnMin, 
		fltPosRtnRadius, blnXAxis);
	
	//update the controls with the angle and radius values
	updateControl(IDC_EDIT18, intPosRtnAngle);
	
	updateControl(IDC_EDIT19, intPosRtnMin);
	
	updateControl(IDC_EDIT20,0, fltPosRtnRadius);
	
	
	spin5->SetFocus();
}		

void camPositionDlg::calculatePosCood() {
	
	FLTPOINT3D cntrOfRtn; //Initialized to the origin

	if(camOrientation.camPosSelType == POS_ROTN_CUBE_Y_AXIS || 
		camOrientation.camPosSelType == POS_ROTN_CUBE_X_AXIS) 
		cntrOfRtn = cuboidCntr;

	BOOL blnXAxis;
	
	if(camOrientation.camPosSelType == POS_ROTN_X_AXIS || 
		camOrientation.camPosSelType == POS_ROTN_CUBE_X_AXIS)
		blnXAxis = true;
	else
		blnXAxis = false;

	getCoordinates(cntrOfRtn, camOrientation.camPos, intPosRtnAngle, intPosRtnMin, blnXAxis);


}

//////////////////// END OF FUNCTIONS FOR CHANGING THE CAMERA POSITION CO-ORDINATES ///////////
//******************************//////////////////////////***********************************//





///////////////////////////////////////////////////////////////////////////////////////////////
///// getAngleRadius: Called whenever the option of rotation of the camera ////////////////////
///// position around any of the axis is chosen. Gives back the angle, and ////////////////////
/////               radius around the center of rotation                   ////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void camPositionDlg::getAngleRadius(FLTPOINT3D cntrOfRtn, FLTPOINT3D freeEnd, 
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
void camPositionDlg::getCoordinates(FLTPOINT3D cntrOfRtn, FLTPOINT3D &freeEnd, 
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






///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////General functions//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


void camPositionDlg::vectorToCood(VECTOR r, FLTPOINT3D &p) {
	p.x = r.i;
	p.y = r.j;
	p.z = r.k;
}
void camPositionDlg::updateControl(int intCntrlID, int intVal, float fltVal) {
	
	//While updating the values, the update function for other controls should not be called
	blnUpdtControl = false;
	
	CString str;
	
	switch(intCntrlID) {

	case IDC_EDIT2:
		str.Format("%f", fltVal);
		edit2->SetWindowText(str);
		break;
	case IDC_EDIT11:
		str.Format("%f", fltVal);
		edit11->SetWindowText(str);
		break;
	case IDC_EDIT17:
		str.Format("%f", fltVal);
		edit17->SetWindowText(str);
		break;
	case IDC_EDIT18:
		str.Format("%d", intVal);
		edit18->SetWindowText(str);
		break;
	case IDC_EDIT19:
		str.Format("%d", intVal);
		edit19->SetWindowText(str);
		break;
	case IDC_EDIT20:
		str.Format("%f", fltVal);
		edit20->SetWindowText(str);
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
void camPositionDlg::OnCancel() {
	CDialog::OnCancel();
}

void camPositionDlg::OnOK() {
	
	//Validate the fields other than those done by DoDataExchange===========================
	
	//Radius if the selection type for camera position is rotation
	if(camOrientation.camPosSelType != POS_COORDINATES) { //Rotation option selected
	
		//Note: The DDX_Text routine in dodataexchange validates the radius for a number		
		
		CString str;
		edit20->GetWindowText(str);
		float fltNewRadius = (float)atof(str);
		
		if(fltNewRadius <= 0.0f ) {
			MessageBox("Radius should be greater than zero.", "Error:");
			edit20->SetSel(0, str.GetLength());
			edit20->SetFocus();
			return;
		}

		if(fltNewRadius > 49500.0f ) {
		//Even if the cube is not located at the origin, the coordinates should not exceed 
		//the maximum value of 50000 specified for all coordinates
	
			MessageBox("Radius should be less than or equal to 49500", "Error:");
			edit20->SetSel(0, str.GetLength());
			edit20->SetFocus();
			return;
		}	
	}
	
	CDialog::OnOK();

	
	if(!IsWindowVisible()) 
		//Dialog box has closed without any errors
		camOrientation.blnDataInitialized = true;

}

CAM_ORIENTATION camPositionDlg::getCamOrientation() {
	return camOrientation;
}

////////////////////////End of functions for closing the dialog box////////////////////////////
//******************************//////////////////////////***********************************//


BEGIN_MESSAGE_MAP(camPositionDlg, CDialog)
	
	ON_COMMAND(IDC_RADIO4, selPos_Cood)
	ON_COMMAND(IDC_RADIO5, selPos_Rotate)	

	ON_CBN_SELCHANGE(50, selPosRtnAxis)
	
	ON_EN_CHANGE(IDC_EDIT2,  updateDisplay_E2)
	ON_EN_CHANGE(IDC_EDIT11, updateDisplay_E11)
	ON_EN_CHANGE(IDC_EDIT17, updateDisplay_E17)
	ON_EN_CHANGE(IDC_EDIT18, updateDisplay_E18)
	ON_EN_CHANGE(IDC_EDIT19, updateDisplay_E19)
	ON_EN_CHANGE(IDC_EDIT20, updateDisplay_E20)
	
	ON_COMMAND(IDC_SPIN5, updateDisplay_E18)
	ON_COMMAND(IDC_SPIN6, updateDisplay_E19)

END_MESSAGE_MAP()
