#include <afxwin.h>
#include<gl\gl.h>
#include<gl\glu.h>
#include "definitions.h"
#include "modeldoc.h"

class modelView : public CView {
	DECLARE_DYNCREATE(modelView)

public:
	modelView();


private:
	FLTPOINT3D axisPosition; //The axis is always positioned at the center of the model

	void dummyFunction(); //Used to nullify the calls of App class through accelerator keys
	int intWindowWidth, intWindowHeight; //Window dimensions
	float fltAngle;
	BOOL blnRotate;
	BOOL blnYAxis;
	float fltMaxCoodVal;
	float fltViewDistance;
	BYTE btSeconds; //The seconds after which the model rotates by  5 degrees
	RGBCOLOR displayColor;
	BOOL blnDsplyAxis;
	
	BOOL g_blnDialogLoaded; //If the dialog window is loaded, then
	//this flag will stop the rotation of the model.
	//This has been done, because on clicking on a menu/toolbar item while the 
	//model is rotating, the corresponding dialog was not getting displayed 
	//unless we press the Alt key.

	BOOL g_blnLoadingModel; //This flag is used in the ondraw function. It ensures, that
	//repeated calls to load the model is not made if there are repeated calls to OnDraw
	
	BOOL blnInitialized; //tells whether the dialog has been loaded
	HGLRC m_hGRC; //Rendering context
	HPALETTE m_hpalette;
	CClientDC *m_d;

	void OnDraw(CDC *p);
	void OnSize ( UINT type, int cx, int cy ) ;	
	void OnTimer(UINT nIDEvent) ;
	void OnKeyDown(UINT ascii, UINT repeat, UINT scan);

	void showModelInfo();

	void useMoldColor();
	void changeColor();	
		
	void toggleRotate();
	void toggleRtnAxis();
	void rtnSpeed();
	void showRtnOptions(CCmdUI *item);

	void setFrontView();
	void setTopView();
	void setLeftView();
	void setRightView();
	void setBackView();
	void setBottomView();
	
	void resetViewDistance();
	void toggleAxisDsply();

	void showKeyboard();
	
	void calcPointRanges(modelDoc *doc);
	void setDefaultDistance();
	void setRotationTimer(BYTE inBtSeconds);
	void resetPerspectiveView() ;
	///////////////////////////////////////////////////
	void drawObjects();   //OpenGl 
	void init_rendering();	//Functions
	void resetView();
	//////////////////////////////////////////////////
	
	void OnDestroy(); 

	void showAboutDlg();

	DECLARE_MESSAGE_MAP()
};
