#ifndef CAMORIETATION_H_
#define CAMORIETATION_H_

#include<afxwin.h>
#include<afxcmn.h>
#include "definitions.h"
#include "viewcamorntn.h"

/****************************Main dialog class***********************************************/

class camOrietationDlg : public CDialog {
public:
	CAM_ORIENTATION camOrientation;

	camOrietationDlg(int n, CAM_ORIENTATION inCamOrientation, TARGETCUBOID cuboidParams,
		CAMERA_INFO camInfo);

	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	void OnOK();
	void OnCancel();
private:
	
	CStatic *st2, *st3, *st4, *st8, *st10, *st12, *st40;
	
	CButton *btn4, *btn5,  *btn6,  *btn7,  *btn8,  *btn9;
	
	void updtViewBtnStatus(DISPLAY_TYPE inDisplayType, BOOL blnEnable); 
	
	viewCamOrntn view; //Open GL view  

	FLTPOINT3D cuboidCntr, defltCamPos;
	
	void assignDefltValues(); 

	void updateDisplay();

	void updateOpenGLDisplay(); 

		
	void createOpenGLView(); //To display the Open GL view
	void destroyOpenGLView();//Called before displaying any message in a messagebox
	
	void setView_Top();
	void setView_Bottom();
	void setView_Left();
	void setView_Right();
	void setView_Front();
	void setView_Back();
	void showLegend();


	void changeViewDistance();
	
	void changeCamPosition(); //Calls the dialog for changing the camera position
	void changeCamDirection(); //Calls the dialog for changing the camera view direction
	
	DECLARE_MESSAGE_MAP();
};

/*****************************Dialog class for view distance*********************************/

class viewDistanceDlg : public CDialog {
public:
	viewDistanceDlg(int n, float inFltViewDistance);
	int OnInitDialog();
	void OnOK();
	float fltViewDistance;
private:
	CListBox *list1;
};


#endif


