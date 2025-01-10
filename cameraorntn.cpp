#include "cameraorntn.h"
#include "campositiondlg.h"
#include "lookingatdlg.h"
#include "resource.h"
#include "myapp.h"

/*==========================================================================================*/
/********************************camOrietationDlg********************************************/
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Initializing functions/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
camOrietationDlg::camOrietationDlg(int n, CAM_ORIENTATION inCamOrientation, 
	TARGETCUBOID cuboidParams, CAMERA_INFO camInfo) : CDialog(n) {
	
	camOrientation = inCamOrientation;
	
	//Calculate and store the coordinates of the center of the cuboid
	cuboidCntr.x = (cuboidParams.fltMaxx + cuboidParams.fltMinx)/2.0f;
	cuboidCntr.y = (cuboidParams.fltMaxy + cuboidParams.fltMiny)/2.0f;
	cuboidCntr.z = (cuboidParams.fltMaxz + cuboidParams.fltMinz)/2.0f;

	//Assign the default value for the camera position
	defltCamPos = cuboidCntr;
	defltCamPos.z = 2.0f * cuboidParams.fltMaxz;

	if(!camOrientation.blnDataInitialized) 
		assignDefltValues();
	
	//Initialize the view window's variables
	view.initialize(cuboidParams, camInfo, camOrientation.camPos, camOrientation.lookingAt,
		camOrientation.displayType);

}

int camOrietationDlg::OnInitDialog() {
	
	createOpenGLView();	
	
	//Get the handles to the controls==================

	st2= (CStatic *)GetDlgItem(IDC_STATIC2);
	st3= (CStatic *)GetDlgItem(IDC_STATIC3);
	st4= (CStatic *)GetDlgItem(IDC_STATIC4); 
	st8= (CStatic *)GetDlgItem(IDC_STATIC8); 
	st10= (CStatic *)GetDlgItem(IDC_STATIC10); 
	st12= (CStatic *)GetDlgItem(IDC_STATIC12); 
	st40= (CStatic *)GetDlgItem(IDC_STATIC40);

	btn4 = (CButton *)GetDlgItem(IDC_BUTTON4);
	btn5 = (CButton *)GetDlgItem(IDC_BUTTON5);
	btn6 = (CButton *)GetDlgItem(IDC_BUTTON6);
	btn7 = (CButton *)GetDlgItem(IDC_BUTTON7);
	btn8 = (CButton *)GetDlgItem(IDC_BUTTON8);
	btn9 = (CButton *)GetDlgItem(IDC_BUTTON9);
	
	updtViewBtnStatus(camOrientation.displayType, false); //Disable the button for the already
	//selected view

	//=================================================

	//Display co-ordinates for the center of the cuboid====
	CString str;	
	str.Format("%.3f", cuboidCntr.x);
	((CStatic *)GetDlgItem(IDC_STATIC37))->SetWindowText(str);

	str.Format("%.3f", cuboidCntr.y);
	((CStatic *)GetDlgItem(IDC_STATIC38))->SetWindowText(str);

	str.Format("%.3f", cuboidCntr.z);
	((CStatic *)GetDlgItem(IDC_STATIC39))->SetWindowText(str);
	//======================================================

	//Display the view distance~~~~~~~~~~~~~~~~~~~~
	str.Format("%d", (int)view.getViewDistance());
	st40->SetWindowText(str);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	CDialog::OnInitDialog();
	
	//update the static display of view direction
	updateDisplay();

	return TRUE;
}

void camOrietationDlg::createOpenGLView() {

	WNDCLASS wndClass;

	wndClass.style = CS_OWNDC;
	wndClass.lpfnWndProc = AfxWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = AfxGetInstanceHandle();
	wndClass.hIcon = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "OpenGLClass";

	// Register the window class
	AfxRegisterClass(&wndClass);

	view.Create("OpenGLClass",NULL,WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			CRect(185, 8,535, 380),this, 1,NULL);
	
	view.Invalidate(FALSE);


}

void camOrietationDlg::DoDataExchange(CDataExchange *pdx) {

	CDialog::DoDataExchange(pdx);
}
//////////////////////////////////Initializing functions///////////////////////////////////////
//******************************//////////////////////////***********************************//


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////Functions for updating the display type///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void camOrietationDlg::setView_Top() { 
	updtViewBtnStatus(camOrientation.displayType, true);
	camOrientation.displayType = TOP_VIEW;
	updtViewBtnStatus(camOrientation.displayType, false);

	updateOpenGLDisplay(); 
}

void camOrietationDlg::setView_Bottom() {  
	updtViewBtnStatus(camOrientation.displayType, true);
	camOrientation.displayType = BOTTOM_VIEW;
	updtViewBtnStatus(camOrientation.displayType, false);
	
	updateOpenGLDisplay(); 
}

void camOrietationDlg::setView_Left() {  
	updtViewBtnStatus(camOrientation.displayType, true);
	camOrientation.displayType = LEFT_VIEW;
	updtViewBtnStatus(camOrientation.displayType, false);
	
	updateOpenGLDisplay(); 
}

void camOrietationDlg::setView_Right() {  
	updtViewBtnStatus(camOrientation.displayType, true);
	camOrientation.displayType = RIGHT_VIEW;
	updtViewBtnStatus(camOrientation.displayType, false);
	
	updateOpenGLDisplay(); 
}

void camOrietationDlg::setView_Front() { 
	updtViewBtnStatus(camOrientation.displayType, true);
	camOrientation.displayType = FRONT_VIEW;
	updtViewBtnStatus(camOrientation.displayType, false);

	updateOpenGLDisplay(); 
}

void camOrietationDlg::setView_Back() { 
	updtViewBtnStatus(camOrientation.displayType, true);
	camOrientation.displayType = BACK_VIEW;
	updtViewBtnStatus(camOrientation.displayType, false);

	updateOpenGLDisplay(); 
}

void camOrietationDlg::changeViewDistance() {
	
	//Update the view window with the values of other fields
	view.updateDisplay(camOrientation.camPos, camOrientation.lookingAt, 
		camOrientation.displayType, false); //false: Don't draw now as we are about to destroy
	
	destroyOpenGLView(); 	
	
	viewDistanceDlg dlg(IDD_DIALOG9, view.getViewDistance());
	if(dlg.DoModal() == IDOK  && view.getViewDistance() != dlg.fltViewDistance) {
		view.setViewDistance(dlg.fltViewDistance);
		//Display the view distance~~~~~~~~~~~~~~~~~~~~
		CString str;
		str.Format("%d", (int)dlg.fltViewDistance);
		st40->SetWindowText(str);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}

	createOpenGLView();	
}
void camOrietationDlg::showLegend() {
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_LEGENDS);
	((myApp*)AfxGetApp())->showHelp();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_ORIENTATION);	
}

void camOrietationDlg::updtViewBtnStatus(DISPLAY_TYPE inDisplayType, BOOL blnEnable) {
	switch(inDisplayType) {
	case TOP_VIEW:
		btn4->EnableWindow(blnEnable);
		break;
	case BOTTOM_VIEW:
		btn5->EnableWindow(blnEnable);
		break;
	case LEFT_VIEW:
		btn6->EnableWindow(blnEnable);
		break;
	case RIGHT_VIEW:
		btn7->EnableWindow(blnEnable);
		break;
	case FRONT_VIEW:
		btn8->EnableWindow(blnEnable);
		break;
	case BACK_VIEW:
		btn9->EnableWindow(blnEnable);
		break;
	}
}
//////////////////////End of Functions for updating the display type///////////////////////////
//******************************//////////////////////////***********************************//




///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////General functions//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
void camOrietationDlg::assignDefltValues() {
	camOrientation.camPos = defltCamPos;
	camOrientation.lookingAt = cuboidCntr;
}

void camOrietationDlg::updateDisplay() {
	
	CString str;
	
	str.Format("X: %f", camOrientation.camPos.x);
	st2->SetWindowText(str);

	str.Format("Y: %f", camOrientation.camPos.y);
	st3->SetWindowText(str);

	str.Format("Z: %f", camOrientation.camPos.z);
	st4->SetWindowText(str);

	str.Format("X: %f", camOrientation.lookingAt.x);
	st8->SetWindowText(str);

	str.Format("Y: %f", camOrientation.lookingAt.y);
	st10->SetWindowText(str);

	str.Format("Z: %f", camOrientation.lookingAt.z);
	st12->SetWindowText(str);	
}

void camOrietationDlg::updateOpenGLDisplay() {
	
	view.updateDisplay(camOrientation.camPos, camOrientation.lookingAt, 
		camOrientation.displayType, true);
}

//////////////////////////////////End of general functions/////////////////////////////////////
//******************************//////////////////////////***********************************//


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////Functions called for closing the dialog box///////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void camOrietationDlg::destroyOpenGLView() {
		view.DestroyWindow();	

}

void camOrietationDlg::OnCancel() {
	destroyOpenGLView();
	CDialog::OnCancel();
}

void camOrietationDlg::OnOK() {
	destroyOpenGLView();
	camOrientation.blnDataInitialized = true;
	CDialog::OnOK();
}

////////////////////////End of functions for closing the dialog box////////////////////////////
//******************************//////////////////////////***********************************//
void camOrietationDlg::changeCamPosition() {
	
	//A new window does not show up if the OpenGLView is on, so destroy it first
	destroyOpenGLView();

	//Create an instance of the camPositionDlg Dialog
	camPositionDlg dlg(IDD_DIALOG6, camOrientation, cuboidCntr, defltCamPos);
	
	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_LOCATION);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_ORIENTATION);
	
	if(intRtnVal == IDOK) {
		//update the camOreintation parameter
		camOrientation = dlg.getCamOrientation();	
		//update the static display of camera position
		updateDisplay();
		//create the OpenGL view again
		createOpenGLView();
		//update the OpenGL view with the new camera pos
		updateOpenGLDisplay();
		
	} else
		createOpenGLView(); //Just create the new OpenGL view

	delete dlg;
}

void camOrietationDlg::changeCamDirection() {
	//A new window does not show up if the OpenGLView is on, so destroy it first
	destroyOpenGLView();

	//Create an instance of the camPositionDlg Dialog
	lookingAtDlg dlg(IDD_DIALOG17, camOrientation, cuboidCntr);

	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_LOOKING_AT);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_ORIENTATION);

	
	if(intRtnVal == IDOK) {
		//update the camOreintation parameter
		camOrientation = dlg.getCamOrientation();	
		//update the static display of view direction
		updateDisplay();
		//create the OpenGL view again
		createOpenGLView();
		//update the OpenGL view with the new camera pos
		updateOpenGLDisplay();
		
	} else
		createOpenGLView(); //Just create the new OpenGL view

	delete dlg;

}

BEGIN_MESSAGE_MAP(camOrietationDlg, CDialog)
	
	ON_COMMAND(IDC_BUTTON4, setView_Top)
	ON_COMMAND(IDC_BUTTON5, setView_Bottom)
	ON_COMMAND(IDC_BUTTON6, setView_Left)
	ON_COMMAND(IDC_BUTTON7, setView_Right)
	ON_COMMAND(IDC_BUTTON8, setView_Front)
	ON_COMMAND(IDC_BUTTON9, setView_Back)	

	ON_COMMAND(IDC_BUTTON2,  changeViewDistance)
	ON_COMMAND(IDC_BUTTON10, updateOpenGLDisplay)
	ON_COMMAND(IDC_BUTTON11, showLegend)
	ON_COMMAND(IDC_BUTTON13, changeCamPosition)
	ON_COMMAND(IDC_BUTTON14, changeCamDirection)
	

END_MESSAGE_MAP()


/*****************************Dialog class for view distance*********************************/

	
viewDistanceDlg::viewDistanceDlg(int n, float inFltViewDistance)  : CDialog(n) {

	fltViewDistance = inFltViewDistance;

}
int viewDistanceDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	list1 = (CListBox *)GetDlgItem(IDC_LIST1);

	list1->AddString("10");
	list1->AddString("20");
	list1->AddString("50");
	list1->AddString("100");
	list1->AddString("200");
	list1->AddString("500");
	list1->AddString("1000");
	list1->AddString("2000");
	list1->AddString("5000");
	list1->AddString("10000");
	list1->AddString("20000");
	list1->AddString("50000");
	list1->AddString("100000");
	list1->AddString("200000");

	int intIndex;
	switch((int)fltViewDistance) {
	case 10:
		intIndex = 0;
		break;
	case 20:
		intIndex = 1;
		break;
	case 50:
		intIndex = 2;
		break;
	case 100:
		intIndex = 3;
		break;
	case 200:
		intIndex = 4;
		break;
	case 500:
		intIndex = 5;
		break;
	case 1000:
		intIndex = 6;
		break;
	case 2000:
		intIndex = 7;
		break;
	case 5000:
		intIndex = 8;
		break;
	case 10000:
		intIndex = 9;
		break;
	case 20000:
		intIndex = 10;
		break;
	case 50000:
		intIndex = 11;
		break;
	case 100000:
		intIndex = 12;
		break;
	case 200000:
		intIndex = 13;
		break;
	}
	list1->SetCurSel(intIndex);
	
	return TRUE;
}
void viewDistanceDlg::OnOK() {
	int intIndex = list1->GetCurSel();
	switch(intIndex) {
	case 0:
		fltViewDistance = 10.0f;
		break;
	case 1:
		fltViewDistance = 20.0f;
		break;
	case 2:
		fltViewDistance = 50.0f;
		break;
	case 3:
		fltViewDistance = 100.0f;
		break;
	case 4:
		fltViewDistance = 200.0f;
		break;
	case 5:
		fltViewDistance = 500.0f;
		break;
	case 6:
		fltViewDistance = 1000.0f;
		break;
	case 7:
		fltViewDistance = 2000.0f;
		break;
	case 8:
		fltViewDistance = 5000.0f;
		break;
	case 9:
		fltViewDistance = 10000.0f;
		break;
	case 10:
		fltViewDistance = 20000.0f;
		break;
	case 11:
		fltViewDistance = 50000.0f;
		break;
	case 12:
		fltViewDistance = 100000.0f;
		break;
	case 13:
		fltViewDistance = 200000.0f;
		break;

	}
	CDialog::OnOK();
}

