#ifndef VIEWCAMORNTN_H_
#define VIEWCAMORNTN_H_

#include<afxwin.h>
#include<afxcmn.h>
#include "definitions.h"
#include"font.h"

class viewCamOrntn : public CWnd {
public :
	void OnPaint();
	int OnCreate(LPCREATESTRUCT l);
	void OnSize ( UINT type, int w, int h);
	void initialize(TARGETCUBOID inCuboidParams, CAMERA_INFO inCamInfo, 
		FLTPOINT3D inCameraPos, FLTPOINT3D inLookingAt,	DISPLAY_TYPE inDisplayType);
	void updateDisplay(FLTPOINT3D inCameraPos, FLTPOINT3D inLookingAt, 
		DISPLAY_TYPE inDisplayType, BOOL blnRedraw);
	float getViewDistance();
	void setViewDistance(float inFltViewDistance);
	void OnDestroy();

private:
//Variables---------------------------------------------
	font *f;
	//HGLRC m_hGRC;
	HPALETTE m_hpalette;
	//CClientDC *m_d;
	HGLRC m_hRC;
	HDC m_hDC;
	
	void initRendering();

	int w, h;  //Width and height of the window
	
	TARGETCUBOID cuboidParams;
	CAMERARATIO camRatio;
	BOOL blnCameraAtInfinity;
	FLTPOINT3D camPos, lookingAt;

	DISPLAY_TYPE displayType;

	
	FLTPOINT3D arrFarPlane[4];
	FLTPOINT3D arrNearPlane[4];	
	FLTPOINT3D farLeft, farRight, farTop, farBottom;
	FLTPOINT3D nearLeft, nearRight, nearTop, nearBottom;
	BOOL blnPlanesDefined;
	
	float fltViewDistance;	
//--------------------------------------------------------

//Functions------------------------------------------------------
	void setNearFarRectangle();
	void calcViewDistance();
	float valFromPlaneEqn(FLTPOINT3D inP, float inC1,float  inC2, 
									  float inC3,float  inK);
//---------------------------------------------------------------

	DECLARE_MESSAGE_MAP()
};

#endif