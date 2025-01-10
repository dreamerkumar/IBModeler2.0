#include "viewcamorntn.h"
#include<gl\gl.h>
#include<gl\glu.h>
/////////////////////////////////////////////////////////////////////////////////////////////////

float viewCamOrntn::valFromPlaneEqn(FLTPOINT3D inP, float inC1,float  inC2, 
									  float inC3,float  inK) {

	return inP.x*inC1 + inP.y*inC2 + inP.z*inC3 + inK;

}
/////////////////////////////////////////////////////////////////////////////////////////////////
float viewCamOrntn::getViewDistance() { 
	return fltViewDistance;
 }

/////////////////////////////////////////////////////////////////////////////////////////////////
void viewCamOrntn::setViewDistance(float inFltViewDistance) {
	fltViewDistance = inFltViewDistance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void viewCamOrntn::calcViewDistance() {
		//Set the view distance to an appropriate value
	float fltMaxVal;
	fltMaxVal = modulus(cuboidParams.fltMinx);
	fltMaxVal = __max(fltMaxVal, modulus(cuboidParams.fltMaxx));
	fltMaxVal = __max(fltMaxVal, modulus(cuboidParams.fltMiny));
	fltMaxVal = __max(fltMaxVal, modulus(cuboidParams.fltMaxy));
	fltMaxVal = __max(fltMaxVal, modulus(cuboidParams.fltMinz));
	fltMaxVal = __max(fltMaxVal, modulus(cuboidParams.fltMaxz));
	fltMaxVal = __max(fltMaxVal, modulus(camPos.x));
	fltMaxVal = __max(fltMaxVal, modulus(camPos.y));
	fltMaxVal = __max(fltMaxVal, modulus(camPos.z));
	fltMaxVal = __max(fltMaxVal, modulus(lookingAt.x));
	fltMaxVal = __max(fltMaxVal, modulus(lookingAt.y));
	fltMaxVal = __max(fltMaxVal, modulus(lookingAt.z));
	
	if(fltMaxVal > 40000.0f)
		fltViewDistance = 200000.0f;
	else if(fltMaxVal > 20000.0f)
		fltViewDistance = 100000.0f;
	else if(fltMaxVal > 8000.0f)
		fltViewDistance = 50000.0f;
	else if(fltMaxVal > 4000.0f)
		fltViewDistance = 20000.0f;
	else if(fltMaxVal > 2000.0f)
		fltViewDistance = 10000.0f;
	else if(fltMaxVal > 800.0f)
		fltViewDistance = 5000.0f;
	else if(fltMaxVal > 400.0f)
		fltViewDistance = 2000.0f;
	else if(fltMaxVal > 200.0f)
		fltViewDistance = 1000.0f;
	else if(fltMaxVal > 80.0f)
		fltViewDistance = 500.0f;
	else if(fltMaxVal > 40.0f)
		fltViewDistance = 200.0f;
	else if(fltMaxVal > 20.0f)
		fltViewDistance = 100.0f;
	else if(fltMaxVal > 8.0f)
		fltViewDistance = 50.0f;
	else if(fltMaxVal > 4.0f)
		fltViewDistance = 20.0f;
	else
		fltViewDistance = 10.0f;
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void viewCamOrntn::setNearFarRectangle() {
	
	//////////////////Variable Declaration////////////////////////////////////////////////////
	//constants for the plane equation
	float l;
	float m;
	float n;
	float kFarPlane; //constant in the far plane equation
	float kNearPlane ; //constant in the near plane equation
	VECTOR upDirection, rightDirection;//directions on the far plane
	float xScope, yScope; //half lengths of the area covered by the camera on Far rectangle
	/////////////////////////////////////////////////////////////////////////////////////////
		
	blnPlanesDefined = false; //Default is unsuccessful

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~step 1~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* Find the equations of the plane passing through cameraPos and lookingAt and          */
	/* perpendicular to the line joining them. Equation is in								*/
	/* the form x l + y m + z n + k = 0                                                     */
	
	//get the direction of view
	VECTOR vDirection = VECTOR(lookingAt) - VECTOR(camPos);

	float fltModulus = vDirection.modulus()  ;
	if(fltModulus == 0.0f)
		return;
	
	//divide by modulus to get the unit vector
	vDirection = vDirection/ fltModulus;
	
	//directional cosines of the plane
	l = vDirection.i ;
	m = vDirection.j ;
	n = vDirection.k ;
	
	//constants for the equation of plane 
	kNearPlane = - (camPos.x*l + camPos.y*m + camPos.z*n);

	kFarPlane = - (lookingAt.x*l + lookingAt.y*m + lookingAt.z*n);


	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ step 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* Make the bounding rectangle on the far plane. We need to find out the four        */
	/* directions from the central point on the plane along which the four corner points */
	/* lie. If the view direction would have been along the -ve z axis, then the upward  */
	/* direction would have been the positive direction of y axis and moving towards     */
	/* right would mean moving in the positive direction of x axis.                      */
	

	//check whether the vector is along the x, y or z axis
	if(vDirection.i != 0 && vDirection.j == 0 && vDirection.k == 0 ) { 
		//aligned along the x axis
		upDirection = VECTOR(0.0f,1.0f,0.0f) ;		
	
		if(vDirection.i > 0 ) 
			rightDirection = VECTOR(0.0f,0.0f,1.0f);
		else
			rightDirection = VECTOR(0.0f,0.0f,-1.0f);
	
	} else if(vDirection.j != 0 && vDirection.i == 0 && vDirection.k == 0 ) { 
		//aligned along the y axis
		if(vDirection.j > 0 ) 
			upDirection = VECTOR(0.0f,0.0f,1.0f);
		else
			upDirection = VECTOR(0.0f,0.0f,-1.0f);
		
		rightDirection  = VECTOR(1.0f,0.0f,0.0f);
			
	
	} else if(vDirection.k != 0 && vDirection.j == 0 && vDirection.i == 0 ) { 
		//aligned along the z axis
		upDirection = VECTOR(0.0f,1.0f,0.0f);
					
		if(vDirection.k > 0 ) 
			rightDirection = VECTOR(-1.0f,0.0f,0.0f);
		else
			rightDirection = VECTOR(1.0f,0.0f,0.0f);		
		
	
	} else if(vDirection.i == 0 && vDirection.j != 0 && vDirection.k != 0 ) {
		//In the yz plane
		if(vDirection.k < 0 ) 
			rightDirection = VECTOR( 1.0f, 0.0f, 0.0f) ; 
		else
			rightDirection = VECTOR(-1.0f, 0.0f, 0.0f) ; 
		
		upDirection = rightDirection * vDirection ; 
	
	} else if (vDirection.i !=0 && vDirection.j == 0 && vDirection.k != 0 ) {
		//In the xz plane
		upDirection = VECTOR( 0.0f , 1.0f , 0.0f ) ; 
		
		rightDirection = vDirection * upDirection ; 

	}  else if (vDirection.i !=0 && vDirection.j != 0 && vDirection.k == 0 ) {
		//In the xy plane
		if(vDirection.i > 0 ) 
			rightDirection = VECTOR( 0.0f, 0.0f, 1.0f ) ; 
		else 
			rightDirection = VECTOR( 0.0f, 0.0f,-1.0f ) ; 
			
		upDirection = rightDirection * vDirection ; 

	} else if( vDirection.i !=0 && vDirection.j != 0 && vDirection.k != 0 ) {
		//Not along the axis, neither along the three planes
		
		if( (vDirection.k < 0 && vDirection.i > 0) 
				||  ( vDirection.k > 0 && vDirection.i < 0 ) ) 
			upDirection = VECTOR(vDirection.i, 0.0f, 0.0f) * vDirection ;
		else 
			upDirection = vDirection * VECTOR(vDirection.i, 0.0f, 0.0f) ;
			
		rightDirection = vDirection * upDirection;
	}	
		
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Step 3~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*~Adjust the magnitude of the vectors so that they cover the required area in        ~*/
	/*~the far plane																      ~*/
	
	if(blnCameraAtInfinity) {
		
		xScope = camRatio.xRangeAtInfinity/2.0f;
		yScope = camRatio.yRangeAtInfinity/2.0f;

	} else {
		xScope    = (fltModulus * camRatio.xRatio)/2.0f ;
		yScope    = (fltModulus * camRatio.yRatio)/2.0f ; 
	}
	
	
	float fltUpModulus = upDirection.modulus();
	float fltRtModulus = rightDirection.modulus();
	
	if(fltUpModulus == 0.0f)
		return;
	if(fltRtModulus == 0.0f)
		return;
	
	upDirection	   = upDirection    * ( yScope/fltUpModulus) ;
	rightDirection = rightDirection * ( xScope/fltRtModulus) ;
	
	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Step 4~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* Using the vector equations and this point, find the corners of the far plane        */
	
	//top left
	arrFarPlane[0].x =  lookingAt.x + upDirection.i  - rightDirection.i  ;
	arrFarPlane[0].y =  lookingAt.y + upDirection.j  - rightDirection.j  ;
	arrFarPlane[0].z =  lookingAt.z + upDirection.k  - rightDirection.k  ;
	//bottom left
	arrFarPlane[1].x =  lookingAt.x - upDirection.i  - rightDirection.i  ;
	arrFarPlane[1].y =  lookingAt.y - upDirection.j  - rightDirection.j  ;
	arrFarPlane[1].z =  lookingAt.z - upDirection.k  - rightDirection.k  ;
	//bottom right
	arrFarPlane[2].x =  lookingAt.x - upDirection.i  + rightDirection.i  ;
	arrFarPlane[2].y =  lookingAt.y - upDirection.j  + rightDirection.j  ;
	arrFarPlane[2].z =  lookingAt.z - upDirection.k  + rightDirection.k  ;
	//top right
	arrFarPlane[3].x =  lookingAt.x + upDirection.i  + rightDirection.i  ;
	arrFarPlane[3].y =  lookingAt.y + upDirection.j  + rightDirection.j  ;
	arrFarPlane[3].z =  lookingAt.z + upDirection.k  + rightDirection.k  ;


	farLeft = (arrFarPlane[0] + arrFarPlane[1]) / 2.0f;
	farRight = (arrFarPlane[2] + arrFarPlane[3]) / 2.0f;
	farTop = (arrFarPlane[0] + arrFarPlane[3]) / 2.0f;
	farBottom = (arrFarPlane[1] + arrFarPlane[2]) / 2.0f;
	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Step 5~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*~~~~~ If the object is at infinity then define the corners of the near plane ~~~~~~~*/
	if(blnCameraAtInfinity) {		
		/* Using the vector equations and cameraPos, find the corners of the near plane   */
		//top left
		arrNearPlane[0].x =  camPos.x + upDirection.i  - rightDirection.i  ;
		arrNearPlane[0].y =  camPos.y + upDirection.j  - rightDirection.j  ;
		arrNearPlane[0].z =  camPos.z + upDirection.k  - rightDirection.k  ;
		//bottom left
		arrNearPlane[1].x =  camPos.x - upDirection.i  - rightDirection.i  ;
		arrNearPlane[1].y =  camPos.y - upDirection.j  - rightDirection.j  ;
		arrNearPlane[1].z =  camPos.z - upDirection.k  - rightDirection.k  ;
		//bottom right
		arrNearPlane[2].x =  camPos.x - upDirection.i  + rightDirection.i  ;
		arrNearPlane[2].y =  camPos.y - upDirection.j  + rightDirection.j  ;
		arrNearPlane[2].z =  camPos.z - upDirection.k  + rightDirection.k  ;
		//top right
		arrNearPlane[3].x =  camPos.x + upDirection.i  + rightDirection.i  ;
		arrNearPlane[3].y =  camPos.y + upDirection.j  + rightDirection.j  ;
		arrNearPlane[3].z =  camPos.z + upDirection.k  + rightDirection.k  ;

		nearLeft = (arrNearPlane[0] + arrNearPlane[1]) / 2.0f;
		nearRight = (arrNearPlane[2] + arrNearPlane[3]) / 2.0f;
		nearTop = (arrNearPlane[0] + arrNearPlane[3]) / 2.0f;
		nearBottom = (arrNearPlane[1] + arrNearPlane[2]) / 2.0f;
	}

	blnPlanesDefined = true;
}

void viewCamOrntn::initialize(TARGETCUBOID inCuboidParams, CAMERA_INFO inCamInfo, 
	FLTPOINT3D inCameraPos, FLTPOINT3D inLookingAt,	DISPLAY_TYPE inDisplayType) {
	
	cuboidParams = inCuboidParams;

	displayType	= inDisplayType;

	blnCameraAtInfinity = inCamInfo.blnCameraAtInfinity;
	
	if(blnCameraAtInfinity) {
		camRatio.xRangeAtInfinity = inCamInfo.fltInfinityWt;
		camRatio.yRangeAtInfinity = inCamInfo.fltInfinityHt;
	} else {
		//Calculate the y Ratio
		
		float x; //x here stands for an extra variable whose value has to be computed before
		//we get the value of the y ratio
		
		if(inCamInfo.fltHeight1 == inCamInfo.fltHeight2)
			x = 0.0f;
		else
			x = (inCamInfo.fltHeight2 * inCamInfo.fltHtDistance1 - 
				inCamInfo.fltHtDistance2 * inCamInfo.fltHeight1)/ 
				(inCamInfo.fltHeight1 - inCamInfo.fltHeight2);
			
		camRatio.yRatio = inCamInfo.fltHeight1/(x + inCamInfo.fltHtDistance1);

		//Calculate the x Ratio
		if(inCamInfo.blnWtSameAsHt)
			camRatio.xRatio = camRatio.yRatio;
		else {
			if(inCamInfo.fltWidth1 == inCamInfo.fltWidth2)
				x = 0.0f;
			else
				x = (inCamInfo.fltWidth2 * inCamInfo.fltWtDistance1 - 
					inCamInfo.fltWtDistance2 * inCamInfo.fltWidth1)/ 
					(inCamInfo.fltWidth1 - inCamInfo.fltWidth2);
				
			camRatio.xRatio = inCamInfo.fltWidth1/(x + inCamInfo.fltWtDistance1);
		}
	}

	camPos = inCameraPos;
	lookingAt = inLookingAt;

	//Set an appropriate view distance
	calcViewDistance();	

	//Calculate points on the planes perpendicular to the line joining camPos and lookingAt and
	//passing through them
	setNearFarRectangle();

}

int viewCamOrntn::OnCreate ( LPCREATESTRUCT l ) {
	initRendering(); //prepare the screen for OpenGL rendering
	return 0;
}

void viewCamOrntn::initRendering() {

	// Get the device context
	m_hDC = ::GetDC(this->m_hWnd);
	
	
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

	//m_d = new CClientDC ( this ) ;

	int pixformat = ChoosePixelFormat ( m_hDC, &pfd ) ;
	//SetPixelFormat ( m_d -> m_hDC, pixformat, &pfd ) ;

	// Set the Pixel Format
	SetPixelFormat(m_hDC, pixformat, NULL);
	
	
	
	
	DescribePixelFormat ( m_hDC, pixformat, sizeof ( pfd ), &pfd ) ;

	if ( pfd.dwFlags & PFD_NEED_PALETTE ) {
		//setuplogicalpalette( ) ;
		struct
		{
			WORD ver ;
			WORD num ;
			PALETTEENTRY entries[256];
		} logicalpalette = { 0x300, 256 } ;

		BYTE reds[ ] = { 0, 20, 40, 120, 140, 180, 220, 255 } ;
		BYTE greens[ ] = { 0, 20, 40, 120, 140, 180, 220, 255 } ;
		BYTE blues[ ] = { 0, 20, 40, 120, 140, 180, 220, 255 } ;

		for ( int cn = 0 ; cn < 256 ; ++cn ) {	

			logicalpalette.entries[cn].peRed = reds[cn & 0x07] ;
			logicalpalette.entries[cn].peGreen = greens[( cn >> 0x03 ) & 0x07] ;
			logicalpalette.entries[cn].peBlue = blues[( cn >> 0x06 ) & 0x03] ;
			logicalpalette.entries[cn].peFlags = 0 ;
		}

		m_hpalette = CreatePalette ( ( LOGPALETTE* ) &logicalpalette ) ;

		if ( m_hpalette )
		{
			SelectPalette ( m_hDC, m_hpalette, FALSE ) ;
			RealizePalette (m_hDC ) ;
		}
	}//end of setuplogicalpalette( ) ;

	m_hRC = wglCreateContext (m_hDC ) ;
	if(m_hRC != NULL)
		wglMakeCurrent (m_hDC, m_hRC ) ;	
	
	//**************OPENGL STATE CALLS**********************
	
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside of 	
	/*
	/*************CODE FOR LIGHTING*************************
	Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	float  whiteLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };	
	float  sourceLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float  lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);*/
	//********END OF CODE FOR LIGHTING**********************

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Black background
	glClearColor(230.0f/FLT_MAX_CLR, 240.0f/FLT_MAX_CLR, 1.0f, 1.0f );	
	//**************END OF OPENGL STATE CALLS***************

	//***************Set the lighting properties*****************//
	//glEnable(GL_LIGHTING);

//	float ambientLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
//	glLightfv(GL_LIGHT0, GL_LIGHT_MODEL_AMBIENT, ambientLight);
//	glEnable(GL_COLOR_MATERIAL);

//glColorMaterial(GL_FRONT, GL_AMBIENT);

	//**********End of setting the lighting properties***********//
	
	f = new font(); //This object should be created only after the
	//initial set up of OpenGL

}

void viewCamOrntn::OnSize ( UINT type, int inWt, int inHt) {
	
	w = inWt; 
	h = inHt;
	
	if(h == 0)
		h = 1;
	
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);	
}

void viewCamOrntn::OnPaint() {
	
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

		// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//SET FOR PERSPECTIVE PROJECTION
	gluPerspective(45.0f, (float)w/(float)h, 1.0f, 210000.0f);
	//Note: The near and far plane(given by the last two parameters ) should always be +ve
	//*************

	// Modelview matrix reset
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	
	//Translate the whole scene out and into view	
	glTranslatef(0.0f, 0.0f, -fltViewDistance);

	// Save the matrix state and do the rotations	
	glPushMatrix();		

	switch(displayType) {
	case FRONT_VIEW:
		//Need not rotate
		break;
	case BACK_VIEW:
		glRotatef(180.0f,  0.0f, 1.0f, 0.0f );		
		break;
	case TOP_VIEW:
		glRotatef(90.0f,  1.0f, 0.0f, 0.0f );		
		break;
	case BOTTOM_VIEW:
		glRotatef(-90.0f,  1.0f, 0.0f, 0.0f );				
		break;
	case LEFT_VIEW:
		glRotatef(90.0f,  0.0f, 1.0f, 0.0f );		
		break;
	case RIGHT_VIEW:
		glRotatef(-90.0f,  0.0f, 1.0f, 0.0f );		
		break;
	}
	
	//****************BEGIN DRAWING*************************************************

	if(blnPlanesDefined) {
		//Draw the camera location ***********************************************
		if(blnCameraAtInfinity) {
			glColor3f(0.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
				glVertex3f(nearLeft.x, nearLeft.y, nearLeft.z);
				glVertex3f(nearRight.x, nearRight.y, nearRight.z);		

				glVertex3f(nearTop.x, nearTop.y, nearTop.z);
				glVertex3f(nearBottom.x, nearBottom.y, nearBottom.z);		

				glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);		
				glVertex3f(arrNearPlane[1].x, arrNearPlane[1].y, arrNearPlane[1].z);		

				glVertex3f(arrNearPlane[1].x, arrNearPlane[1].y, arrNearPlane[1].z);		
				glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);		

				glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);		
				glVertex3f(arrNearPlane[3].x, arrNearPlane[3].y, arrNearPlane[3].z);		

				glVertex3f(arrNearPlane[3].x, arrNearPlane[3].y, arrNearPlane[3].z);		
				glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);		

			glEnd();		
		}
		//End of drawing the camera location*************************************

		
		//Draw the point at which the camera is looking ************************
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
			glVertex3f(farLeft.x, farLeft.y, farLeft.z);
			glVertex3f(farRight.x, farRight.y, farRight.z);		

			glVertex3f(farTop.x, farTop.y, farTop.z);
			glVertex3f(farBottom.x, farBottom.y, farBottom.z);		

			glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);		
			glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);		

			glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);		
			glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);		

			glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);		
			glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);		

			glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);		
			glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);		
		glEnd();		
		//End of draw the point at which the camera is looking/

		
		//Draw the lines from the camera location to the view direction
		glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);	
			glVertex3f(camPos.x, camPos.y, camPos.z);
			glVertex3f(lookingAt.x, lookingAt.y, lookingAt.z);
					
			if(!blnCameraAtInfinity) { //frustum
				
				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(camPos.x, camPos.y, camPos.z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);				

				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(camPos.x, camPos.y, camPos.z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);				

				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(camPos.x, camPos.y, camPos.z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);				

				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(camPos.x, camPos.y, camPos.z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);				
			
			} else { //cuboid 
				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);				

				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(arrNearPlane[1].x, arrNearPlane[1].y, arrNearPlane[1].z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);				

				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);				

				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(arrNearPlane[3].x, arrNearPlane[3].y, arrNearPlane[3].z);		
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);				
			}

		glEnd();
		//End of line joining the camera location to the point at which the camera is looking*
	}
	//**************BEGIN CUBOID**************************************
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
		//Front face
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMaxy, cuboidParams.fltMaxz);
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMiny, cuboidParams.fltMaxz);

		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMiny, cuboidParams.fltMaxz);
		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMiny, cuboidParams.fltMaxz);

		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMiny, cuboidParams.fltMaxz);
		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMaxy, cuboidParams.fltMaxz);

		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMaxy, cuboidParams.fltMaxz);
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMaxy, cuboidParams.fltMaxz);
		//Back face
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMaxy, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMiny, cuboidParams.fltMinz);

		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMiny, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMiny, cuboidParams.fltMinz);

		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMiny, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMaxy, cuboidParams.fltMinz);

		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMaxy, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMaxy, cuboidParams.fltMinz);
		//Lines joining the faces
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMaxy, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMaxy, cuboidParams.fltMaxz);

		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMiny, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMinx, cuboidParams.fltMiny, cuboidParams.fltMaxz);
		
		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMiny, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMiny, cuboidParams.fltMaxz);

		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMaxy, cuboidParams.fltMinz);
		glVertex3f(cuboidParams.fltMaxx, cuboidParams.fltMaxy, cuboidParams.fltMaxz);
	glEnd();
	//*********************End of Cuboid***********************************

	//*************COORDINATE AXES****************
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(-fltViewDistance,0.0f,0.0f);
		glVertex3f(fltViewDistance,0.0f,0.0f);		
		glVertex3f(0.0f,-fltViewDistance,0.0f);
		glVertex3f(0.0f,fltViewDistance,0.0f);		
		glVertex3f(0.0f,0.0f,-fltViewDistance);
		glVertex3f(0.0f,0.0f,fltViewDistance);		
	glEnd();	
	//**********END COORDINATE AXES***************	

	//~~~~~~~~~~~~~~~~Draw the frustum along camera direction~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
	glEnable(GL_BLEND);
	float fltTransparency = 0.50f;
	if(blnPlanesDefined) {
		glBegin(GL_TRIANGLES);
		if(!blnCameraAtInfinity) { //draw a frustum
			
			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);	
			glVertex3f(camPos.x, camPos.y, camPos.z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);				

			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);	
			glVertex3f(camPos.x, camPos.y, camPos.z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);				
			
			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(camPos.x, camPos.y, camPos.z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);				
			
			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(camPos.x, camPos.y, camPos.z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);				

		} else { //draw a cuboid
			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);				
			glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);				

			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);		
			glVertex3f(arrNearPlane[3].x, arrNearPlane[3].y, arrNearPlane[3].z);
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);				
			
			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[3].x, arrNearPlane[3].y, arrNearPlane[3].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);				
			glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);				

			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[3].x, arrNearPlane[3].y, arrNearPlane[3].z);		
			glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);				
			
			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);				
			glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);				

			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);		
			glVertex3f(arrNearPlane[1].x, arrNearPlane[1].y, arrNearPlane[1].z);
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);				
			
			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[1].x, arrNearPlane[1].y, arrNearPlane[1].z);		
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);				
			glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);				

			glColor4f(0.0f, 0.0f, 1.0f, fltTransparency);
			glVertex3f(arrNearPlane[1].x, arrNearPlane[1].y, arrNearPlane[1].z);		
			glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);
			glColor4f(1.0f, 0.0f, 0.0f, fltTransparency);
			glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);	
			
			//Draw the closing rectangle at the near end
			glColor4f(166.0f/FLT_MAX_CLR, 166.0f/FLT_MAX_CLR, 255.0f/FLT_MAX_CLR, 
				fltTransparency);
			glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);				
			glVertex3f(arrNearPlane[1].x, arrNearPlane[1].y, arrNearPlane[1].z);				
			glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);
			
			glVertex3f(arrNearPlane[0].x, arrNearPlane[0].y, arrNearPlane[0].z);				
			glVertex3f(arrNearPlane[2].x, arrNearPlane[2].y, arrNearPlane[2].z);
			glVertex3f(arrNearPlane[3].x, arrNearPlane[3].y, arrNearPlane[3].z);
		}

		//Draw the closing rectangle at the far end
		glColor4f(255.0f/FLT_MAX_CLR, 130.0f/FLT_MAX_CLR, 255.0f/FLT_MAX_CLR, fltTransparency);
		glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);				
		glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);
		glVertex3f(arrFarPlane[1].x, arrFarPlane[1].y, arrFarPlane[1].z);				
						
		glVertex3f(arrFarPlane[0].x, arrFarPlane[0].y, arrFarPlane[0].z);				
		glVertex3f(arrFarPlane[3].x, arrFarPlane[3].y, arrFarPlane[3].z);
		glVertex3f(arrFarPlane[2].x, arrFarPlane[2].y, arrFarPlane[2].z);				
		
		glEnd();	
	}

	// Restore the matrix state
	glPopMatrix();	// Modelview matrix
	
	
	//Display the labels of the axis using orthographic projection~~~~~~~~~~~~~~~~~~~~~~~~~~~
   	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Establish clipping volume using glOrtho	
	GLfloat nRange = 50.0f;
	GLfloat xmax, ymax;
	if (w <= h) {
		xmax=nRange; ymax=nRange*h/w;
	} else  {	
		ymax=nRange; xmax=nRange*h/w;
	}	
	glOrtho (0.0f, xmax, 0.0f, ymax, -210000, 210000);//-nRange,nRange);
	
	//Display the font
	glColor3f(0.0f,0.0f,0.0f);
	
	
	char chrTop[3], chrBtm[3], chrLeft[3], chrRight[3], chrViewTypeStatus[20];
	
	switch(displayType) {
	case FRONT_VIEW:
		strcpy(chrTop,"+Y"); 
		strcpy(chrBtm,"-Y"); 
		strcpy(chrLeft,"-X"); 
		strcpy(chrRight,"+X");
		strcpy(chrViewTypeStatus, "FRONT VIEW");
		break;
	case BACK_VIEW:
		strcpy(chrTop,"+Y"); 
		strcpy(chrBtm,"-Y"); 
		strcpy(chrLeft,"+X"); 
		strcpy(chrRight,"-X");
		strcpy(chrViewTypeStatus, "BACK VIEW");
		break;
	case TOP_VIEW:
		strcpy(chrTop,"-Z"); 
		strcpy(chrBtm,"+Z"); 
		strcpy(chrLeft,"-X"); 
		strcpy(chrRight,"+X");
		strcpy(chrViewTypeStatus, "TOP VIEW");
		break;
	case BOTTOM_VIEW:
		strcpy(chrTop,"+Z"); 
		strcpy(chrBtm,"-Z"); 
		strcpy(chrLeft,"-X"); 
		strcpy(chrRight,"+X");
		strcpy(chrViewTypeStatus, "BOTTOM VIEW");
		break;
	case LEFT_VIEW:
		strcpy(chrTop,"+Y"); 
		strcpy(chrBtm,"-Y"); 
		strcpy(chrLeft,"-Z"); 
		strcpy(chrRight,"+Z");
		strcpy(chrViewTypeStatus, "LEFT VIEW");
		break;
	case RIGHT_VIEW:
		strcpy(chrTop,"+Y"); 
		strcpy(chrBtm,"-Y"); 
		strcpy(chrLeft,"+Z"); 
		strcpy(chrRight,"-Z");
		strcpy(chrViewTypeStatus, "RIGHT VIEW");
		break;	
	}
		
	glRasterPos3i(1,23, 3);
	f->FontPrintf(f->returnfont(), 1, "%s", &chrLeft);
	glRasterPos3i(47,23, 3);
	f->FontPrintf(f->returnfont(), 1, "%s", &chrRight);
	glRasterPos3i(25,48, 3);
	f->FontPrintf(f->returnfont(), 1, "%s", &chrTop);
	glRasterPos3i(25,1, 3);
	f->FontPrintf(f->returnfont(), 1, "%s", &chrBtm);
	glRasterPos3i(37,49, 3);
	f->FontPrintf(f->returnfont(), 1, "%s", &chrViewTypeStatus);
	//End of display the labels of the axis using orthographic projection~~~~~~~~~~~~~~~~~~~~
	

	//*******************END DRAWING*****************
	
	// Flush drawing commands
    // glFlush(); //SwapBuffers works better(no flicker) than glFlush
	SwapBuffers(m_hDC);
}

void viewCamOrntn::updateDisplay(FLTPOINT3D inCameraPos, FLTPOINT3D inLookingAt, 
								 DISPLAY_TYPE inDisplayType, BOOL blnRedraw) {
	//Check if the values are still the same 
	BOOL blnPointsDiffer;
	
	if(camPos.x  == inCameraPos.x && camPos.y  == inCameraPos.y 
		&& camPos.z  == inCameraPos.z && lookingAt.x == inLookingAt.x &&  
		lookingAt.y == inLookingAt.y && lookingAt.z == inLookingAt.z)
		blnPointsDiffer = false;
	else
		blnPointsDiffer = true;
	
	if(!blnPointsDiffer && displayType == inDisplayType)
		return;	
	
	if(blnPointsDiffer) {
		camPos = inCameraPos;
		lookingAt = inLookingAt;

		//Calculate points on the planes perpendicular to the line joining camPos and 
		//lookingAt and passing through them
		setNearFarRectangle();
	}
	
	displayType = inDisplayType;

	if(blnRedraw)	
		Invalidate();
}

void viewCamOrntn::OnDestroy() {

	CWnd::OnDestroy();
	//Delete the font object
	delete f;
	
	//The following lines of code are very important:
	//The first line of code deselects the rendering context
	//The second line of code deletes the rendering context
	//If the rendering context is not deleted, then the system might crash
	//while creating the rendering context again. 
	//This was happening quite a lot when I started testing this view on windows 2000.
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hRC);

	if(m_hpalette != NULL)
		DeleteObject(m_hpalette);
	
	//Delete the device context
	::ReleaseDC(this->m_hWnd, m_hDC);
	m_hDC = NULL;
}

BEGIN_MESSAGE_MAP(viewCamOrntn, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

////////////////////////////////////End of viewCamOrntn////////////////////////////////////////
//******************************//////////////////////////***********************************//


