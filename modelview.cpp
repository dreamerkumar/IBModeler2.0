#include "modelview.h"
#include "dialogs.h"
#include "myapp.h"
#include "htmlhelp.h"

IMPLEMENT_DYNCREATE(modelView, CView)

BEGIN_MESSAGE_MAP(modelView,CView)
	ON_COMMAND(ID_FILE_NEW, dummyFunction) 
	ON_COMMAND(ID_FILE_OPEN, dummyFunction)	
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()

	ON_COMMAND(MENU_MODEL_MODEL_INFO, showModelInfo)

	ON_COMMAND(MENU_MODEL_MODEL_CLR, useMoldColor)
	ON_COMMAND(MENU_MODEL_CHANGE_CLR, changeColor)
	
	ON_COMMAND(MENU_MODEL_TGL_ROTATE, toggleRotate)
	ON_COMMAND(MENU_MODEL_TGL_AXIS, toggleRtnAxis)
	ON_COMMAND(MENU_MODEL_RTN_SPEED, rtnSpeed)

	ON_UPDATE_COMMAND_UI_RANGE( MENU_MODEL_TGL_AXIS,  MENU_MODEL_RTN_SPEED, showRtnOptions)

	ON_COMMAND(MENU_MODEL_FRONT, setFrontView)
	ON_COMMAND(MENU_MODEL_TOP, setTopView)
	ON_COMMAND(MENU_MODEL_LEFT, setLeftView)
	ON_COMMAND(MENU_MODEL_RIGHT, setRightView)
	ON_COMMAND(MENU_MODEL_BACK, setBackView)
	ON_COMMAND(MENU_MODEL_BOTTOM, setBottomView)

	ON_COMMAND(MENU_MODEL_DISTANCE, resetViewDistance)
	ON_COMMAND(MENU_MODEL_TGL_AXIS_DSPLY, toggleAxisDsply)
	
	ON_COMMAND(MENU_KEYBOARD_HELP, showKeyboard)	

	ON_WM_DESTROY()

	ON_COMMAND(MENU_HELP_ABOUT, showAboutDlg)

END_MESSAGE_MAP()


//If MENU_HELP_ABOUT is clicked, make sure the model is not rotating while displaying,
//the about dialog window (displayed by the myApp class). This will ensure that the 
//dialog wont get hidden~~~~~~~~~~~~~~~~~~~~~~ 
void modelView::showAboutDlg() {
	g_blnDialogLoaded = true;
	((myApp *)AfxGetApp())->showAboutDlg();
	g_blnDialogLoaded = false;
} //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void modelView::OnDestroy() {
	
	CView::OnDestroy();
	
	if(m_d != NULL) {
		//The following lines of code are very important:
		//The first line of code deselects the rendering context
		//The second line of code deletes the rendering context
		//If the rendering context is not deleted, then the system might crash
		//while creating the rendering context again. 
		wglMakeCurrent(m_d -> m_hDC,NULL);
		wglDeleteContext(m_hGRC);
	}

	if(m_hpalette != NULL)
		DeleteObject(m_hpalette);

	
}

void modelView::dummyFunction() {} //Used to nullify the calls of App class through accelerator keys
void modelView::changeColor() {
	// The code below uses CColorDialog::m_cc data member to 
	// customize the settings of CColorDialog. The CColorDialog will
	// be shown as full open and the cuurently selected color will be passed to it
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = RGB(displayColor.red, displayColor.green, displayColor.blue);

	g_blnDialogLoaded = true;
	if(dlg.DoModal()==IDOK) {
		COLORREF clr=dlg.GetColor();

		displayColor.red = GetRValue(clr);
		displayColor.green = GetGValue(clr);
		displayColor.blue = GetBValue(clr);
		drawObjects();
	}
	g_blnDialogLoaded = false;
}

void modelView::useMoldColor() {
		
	modelDoc *doc = (modelDoc *) GetDocument();
	displayColor = doc->getColor();
	drawObjects();
}

void modelView::setFrontView() {
	blnRotate = false;
	blnYAxis = true;
	fltAngle = 0.0f;
	drawObjects();
}

void modelView::setTopView() {
	blnRotate = false;
	blnYAxis = false;
	fltAngle = 270.0f;
	drawObjects();
}

void modelView::setLeftView() {
	blnRotate = false;
	blnYAxis = true;
	fltAngle = 270.0f;
	drawObjects();
}

void modelView::setRightView() {
	blnRotate = false;
	blnYAxis = true;
	fltAngle = 90.0f;
	drawObjects();
}

void modelView::setBackView() {
	blnRotate = false;
	blnYAxis = true;
	fltAngle = 180.0f;
	drawObjects();
}

void modelView::setBottomView() {
	blnRotate = false;
	blnYAxis = false;
	fltAngle = 90.0f;
	drawObjects();
}

void modelView::showKeyboard() {
	dlgKeyboardMap dlg(IDD_DIALOG15, MODEL);
	g_blnDialogLoaded = true;
	dlg.DoModal();
	g_blnDialogLoaded = false;
	delete dlg;
	drawObjects(); //Always redraw after closing a dialog window
}


void modelView::showModelInfo() {
	modelDoc *doc = (modelDoc *) GetDocument();

	MODEL_POINT_RANGE pointRange = doc->getPointRange();
	
	MODEL_INFO modelInfo;
	
	modelInfo.strFilePath  = doc->getFilePath();
	modelInfo.intFileSize = doc->getFileSize();

	modelInfo.fltMinx = pointRange.fltMinx;
	modelInfo.fltMaxx = pointRange.fltMaxx;
	modelInfo.fltMiny = pointRange.fltMiny;
	modelInfo.fltMaxy = pointRange.fltMaxy;
	modelInfo.fltMinz = pointRange.fltMinz;
	modelInfo.fltMaxz = pointRange.fltMaxz;

	modelInfo.fltViewDistance = fltViewDistance;
	modelInfo.intTotTriangles = doc->getVertexCount()/3;
	modelInfo.strCurrentView = "";
	if(blnRotate) {
		CString str;
		str.Format("%d", btSeconds*100);
		modelInfo.strCurrentView = "Rotating by 5 degrees every " + 
			str + " milliseconds around the ";
		if(blnYAxis) 
			modelInfo.strCurrentView+= "vertical axis.";
		else
			modelInfo.strCurrentView+= "horizontal axis.";
	} else {
		if(blnYAxis && fltAngle == 0.0f)
			modelInfo.strCurrentView = "Front View";
		else if(!blnYAxis && fltAngle == 270.0f)
			modelInfo.strCurrentView = "Top View";
		else if(blnYAxis && fltAngle == 270.0f)		
			modelInfo.strCurrentView = "Left View";
		else if(blnYAxis && fltAngle == 90.0f)		
			modelInfo.strCurrentView = "Right View";
		else if(blnYAxis && fltAngle == 180.0f)		
			modelInfo.strCurrentView = "Back View";
		else if(!blnYAxis && fltAngle == 90.0f)		
			modelInfo.strCurrentView = "Bottom View";	
		else if(blnYAxis)
			modelInfo.strCurrentView = "Rotation around the Y axis stopped";	
		else
			modelInfo.strCurrentView = "Rotation around the X axis stopped";	
	}

	dlgModelInfo dlg(IDD_DIALOG14, modelInfo);
	g_blnDialogLoaded = true;
	dlg.DoModal();
	g_blnDialogLoaded = false;
	delete dlg;
	drawObjects(); //Always redraw after closing a dialog window

}

void modelView::toggleRotate() {
	blnRotate = !blnRotate;	
	
}

void modelView::toggleRtnAxis()	{
	if(blnRotate) {
		blnYAxis = !blnYAxis;	
		fltAngle = 0.0f; //Reset angle
	}
}

void modelView::rtnSpeed() {
	dlgModelRtnSpeed dlg(IDD_DIALOG12, btSeconds );
	g_blnDialogLoaded = true;
	
	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_RTN_SPEED);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MODEL);
	
	if(intRtnVal == IDOK) 
		setRotationTimer(dlg.getSeconds());
	g_blnDialogLoaded = false;
	delete dlg;
	drawObjects(); //Always redraw after closing a dialog window
}

void modelView::showRtnOptions(CCmdUI *item) {
	if(blnRotate)
		item->Enable(TRUE);
	else
		item->Enable(FALSE);
}

void modelView::setRotationTimer(BYTE inBtSeconds) {
	
	VERIFY(inBtSeconds >=1 && inBtSeconds <=5);
	
	if(inBtSeconds == btSeconds) //No need to reset the timer
		return;

	KillTimer(1); //Stop the old timer	

	btSeconds = inBtSeconds;

	UINT m_nTimer = SetTimer(1, btSeconds * 100, NULL); //Set the new timer
	//Note although the variable used is btSeconds the value being stored here is 1/10th of a second
}

void modelView::resetViewDistance()	{
	dlgModelViewDt dlg(IDD_DIALOG13, fltViewDistance, fltMaxCoodVal);
	
	g_blnDialogLoaded = true;
	
	
	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MODEL_VIEW_DISTANCE);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MODEL);
	
	if(intRtnVal == IDOK) {
		fltViewDistance = dlg.getViewDt();
		resetPerspectiveView();
	}

	g_blnDialogLoaded = false;

	delete dlg;
	drawObjects(); //Always redraw after closing a dialog window
}

void modelView::toggleAxisDsply() {
	blnDsplyAxis = !blnDsplyAxis;
	
	if(blnDsplyAxis) { //Inform the user of the position of the axis
		
		CString strMsg;
		strMsg.Format("The axes are positioned to the\ncenter of the model:\nX = %.2f\nY = %.2f\nZ = %.2f", 
			axisPosition.x,axisPosition.y,axisPosition.z);
		g_blnDialogLoaded = true;
		MessageBox(strMsg, "Axis position", MB_OK | MB_ICONINFORMATION);
		g_blnDialogLoaded = false;
	}
	drawObjects();
}

modelView::modelView() { 
	g_blnDialogLoaded = false; //No dialog is loaded initially
	blnInitialized = false;
	blnRotate = true ; //enable rotation of objects 
	blnYAxis = true;
	fltAngle = 0.0f; //set the view angle
	btSeconds = 1; //Rotate by 5 degrees every one second
	blnDsplyAxis = false; //Don't display the axis as default
	g_blnLoadingModel = false; //We haven't started loading the model
	m_d = NULL;

}

void modelView::calcPointRanges(modelDoc *doc)	{
	
	MODEL_POINT_RANGE pointRange = doc->getPointRange();
	
	float fltXRadius = (pointRange.fltMaxx - pointRange.fltMinx)/2.0f;
	float fltYRadius = (pointRange.fltMaxy - pointRange.fltMiny)/2.0f;
	float fltZRadius = (pointRange.fltMaxz - pointRange.fltMinz)/2.0f;

	//The axis position should always be the center of the model
	axisPosition.x = pointRange.fltMinx + fltXRadius;
	axisPosition.y = pointRange.fltMiny + fltYRadius;
	axisPosition.z = pointRange.fltMinz + fltZRadius;
	//----------------------------------------------------------

	//The fltMaxCoodVal will help in determining the optimum distance from which the model
	//is to be viewed
	fltMaxCoodVal = __max(__max(fltXRadius, fltYRadius), fltZRadius) ;
}

void modelView::setDefaultDistance() {
	fltViewDistance = MODEL_VIEW_DISTANCE_MULTIPLIER * fltMaxCoodVal;
	//Round off to two decimal places
	fltViewDistance = ((float)(int)(fltViewDistance * 100.0f))/100.0f;

}


//Function called whenever the display has to be updated
void modelView::OnDraw(CDC *p) {
	modelDoc *doc = (modelDoc *) GetDocument();
	if(doc == NULL )
		return;
	
	if(!blnInitialized) {
		
		CString str = "Loading..";
		p->TextOut(10, 10, str, strlen(str));
		
		if(doc->isModelLoaded()) {
			UINT m_nTimer = SetTimer(1, btSeconds * 100, NULL); //set the timer function 
			blnInitialized = true;
			calcPointRanges(doc);
			setDefaultDistance();
			displayColor = doc->getColor();
			init_rendering();
		} 
		
	} 
	drawObjects();	
}


void modelView::OnSize ( UINT type, int inWidth, int inHeight ) {
	
	intWindowWidth= inWidth; intWindowHeight = inHeight;
	resetView(); //reset to the new viewport
}

void modelView::OnTimer(UINT nIDEvent) {
	if(blnRotate 
		&& !g_blnDialogLoaded) { //Don't alter display if any dialog is loaded.
		fltAngle+= 5.0f; //change the view angle
		if(fltAngle >= 360.0f)
			fltAngle = 0.0f;
		drawObjects(); //draw 
	}
   // Call base class handler.
   CView::OnTimer(nIDEvent);
}

void modelView::OnKeyDown(UINT ascii, UINT repeat, UINT scan) {
	
	//if(!blnInitialized)
		//return;
	
	scan = (scan<<24)>>24;
//	CString cst; cst.Format("%d", scan); MessageBox(cst);	
	if(scan == 19 ) //R
		toggleRotate();
	else if(scan == 30)//A
		toggleRtnAxis();
	else if(scan == 32) {//D
		toggleAxisDsply();
	} else if(scan == 72) { //Up Arrow
		if(fltViewDistance >= fltMaxCoodVal + 1) {
			fltViewDistance--;
			resetPerspectiveView();
			drawObjects();
		}
	} else if(scan == 80) { //Down Arrow
		if(fltViewDistance <= MODEL_FLT_MAX_VIEW_DISTANCE - 1) {
			fltViewDistance++;
			resetPerspectiveView();
			drawObjects();
		}
	}
}

void modelView::drawObjects() {
	
	if(!blnInitialized)
		return;
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations	
	glPushMatrix();		
	

	if(fltAngle!=0.0) {
		if(blnYAxis)
			glRotatef((-fltAngle),0.0f, 1.0f, 0.0f);		
		else
			glRotatef((-fltAngle),1.0f, 0.0f, 0.0f);		
	}

	
	//*************COORDINATE AXES****************
	if(blnDsplyAxis) {
		glColor3f(0.0f,0.0f,255.0f);
		glBegin(GL_LINES);
			glVertex3f(-fltViewDistance,0.0f,0.0f);
			glVertex3f(fltViewDistance,0.0f,0.0f);		
			glVertex3f(0.0f,-fltViewDistance,0.0f);
			glVertex3f(0.0f,fltViewDistance,0.0f);		
			glVertex3f(0.0f,0.0f,-fltViewDistance);
			glVertex3f(0.0f,0.0f,fltViewDistance);		
		glEnd();	
	}

	//Translate the system to the center of the model
	glTranslatef(-axisPosition.x, -axisPosition.y, -axisPosition.z); 

	//****************DO ALL THE DRAWING HERE*********

	glColor3ub(displayColor.red,displayColor.green,displayColor.blue);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	modelDoc *doc = (modelDoc *) GetDocument();

	glVertexPointer(3, GL_FLOAT, 0, doc->getPoints());
	glNormalPointer(GL_FLOAT, 0, doc->getNormals());
	glDrawArrays(GL_TRIANGLES, 0, doc->getVertexCount());

	//*******************END DRAWING*****************
	
	// Restore the matrix state
	glPopMatrix();	// Modelview matrix
	
	// Flush drawing commands
    //glFlush();
	
	SwapBuffers(m_d -> m_hDC);

}

void modelView::init_rendering() {

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof ( PIXELFORMATDESCRIPTOR ), 
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 
		32,
		0, 0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	} ;

	m_d = new CClientDC ( this ) ;

	int pixformat = ChoosePixelFormat ( m_d -> m_hDC, &pfd ) ;
	SetPixelFormat ( m_d -> m_hDC, pixformat, &pfd ) ;

	DescribePixelFormat ( m_d -> m_hDC, pixformat, sizeof ( pfd ), &pfd ) ;

	if ( pfd.dwFlags & PFD_NEED_PALETTE ) {
		//setuplogicalpalette
		struct
		{
			WORD ver ;
			WORD num ;
			PALETTEENTRY entries[256];
		} logicalpalette = { 0x300, 256 } ;

		BYTE reds[ ] = { 0, 20, 40, 120, 140, 180, 220, 255 } ;
		BYTE greens[ ] = { 0, 20, 40, 120, 140, 180, 220, 255 } ;
		BYTE blues[ ] = { 0, 20, 40, 120, 140, 180, 220, 255 } ;

		for ( int cn = 0 ; cn < 256 ; ++cn )
		{	
			logicalpalette.entries[cn].peRed = reds[cn & 0x07] ;
			logicalpalette.entries[cn].peGreen = greens[( cn >> 0x03 ) & 0x07] ;
			logicalpalette.entries[cn].peBlue = blues[( cn >> 0x06 ) & 0x03] ;
			logicalpalette.entries[cn].peFlags = 0 ;
		}

		m_hpalette = CreatePalette ( ( LOGPALETTE* ) &logicalpalette ) ;

		if ( m_hpalette )
		{
			SelectPalette ( m_d -> m_hDC, m_hpalette, FALSE ) ;
			RealizePalette ( m_d -> m_hDC ) ;
		}
	}//end of setuplogicalpalette

	m_hGRC = wglCreateContext ( m_d -> m_hDC ) ;
	wglMakeCurrent ( m_d -> m_hDC, m_hGRC ) ;	

	//**************OPENGL STATE CALLS**********************
	
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of 	
	glEnable(GL_LIGHTING);		// Enable lighting
	
	/*************CODE FOR LIGHTING*************************/

	// Setup and enable light 0
	float  whiteLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };	
	float  sourceLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float  lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);
	//********END OF CODE FOR LIGHTING**********************

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	
	//**************END OF OPENGL STATE CALLS***************
	
	resetView();	
}



void modelView::resetView() {
	
	if(!blnInitialized)
		return;
   
	if(intWindowHeight == 0)
		intWindowHeight = 1;
		
	// Set Viewport to window dimensions
    glViewport(0, 0, intWindowWidth, intWindowHeight);

	resetPerspectiveView();
}

void modelView::resetPerspectiveView() {

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//******************FOR PERSPECTIVE PROJECTION ********************
	// field of view of 45 degrees, near and far planes 1.0 and 1000
	gluPerspective(45.0f, (float)intWindowWidth/(float)intWindowHeight, 1,
		(int)MODEL_FLT_MAX_VIEW_DISTANCE + 10);
	
	// Modelview matrix reset
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//	 Translate the whole scene out and into view	
	glTranslatef(0.0f, 0.0f, -fltViewDistance);

}
