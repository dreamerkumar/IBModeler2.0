#ifndef DEFINITONS_H_
#define DEFINITONS_H_

#include <afxext.h>


#define MOLD_HEADER_SIZE		141 
//41 bytes are used as of now while the rest have been kept for future use
#define MOLD_HEADER_EXTRA_BYTES 100            

#define PROCESS_USING_SNAPS		  1
#define PROCESS_USING_SLIDES	  0

#define IMAGE_TOP_INDENT		 30
#define IMAGE_BOTTOM_INDENT		 10
#define IMAGE_LEFT_INDENT		 10
#define IMAGE_RIGHT_INDENT		 10

#define DEFLT_WINDOW_HEIGHT		200
#define DEFLT_WINDOW_WIDTH		200
#define DEFLT_IMG_MAGNIFICATION   1

#define BORDER_WIDTH              2
#define BITMAPINFOHEADER_SIZE	 40

#define DEFLT_DRAW_COLOR_R		200 
#define DEFLT_DRAW_COLOR_G		200
#define DEFLT_DRAW_COLOR_B		200

#define DEFLT_BKGND_COLOR_R		200 
#define DEFLT_BKGND_COLOR_G		200
#define DEFLT_BKGND_COLOR_B		200

#define DEFLT_PADDING_COLOR_R   200 
#define DEFLT_PADDING_COLOR_G   200
#define DEFLT_PADDING_COLOR_B   200


#define DEFLT_MODEL_COLOR_R		200 
#define DEFLT_MODEL_COLOR_G		100
#define DEFLT_MODEL_COLOR_B		200


#define MENU_EXIT               1103

//#define MENU_LOAD_IMG	        2101 Is being done by ID_FILE_OPEN
//#define MENU_PROCESS_IMGS	    2102
#define MENU_SAVE_IMG			2103
#define MENU_SAVE_AS_BITMAP		2104
#define MENU_CLOSE_IMG          2105
#define MENU_CREATE_MODEL       2106
#define MENU_RUN_SCRIPT		    2107
//#define MENU_CLOSE_PROJECT      2107

#define MENU_UNDO						2201
#define MENU_REDO						2202
//#define MENU_BKG_COLOR					2203
//#define MENU_PAD_COLOR					2204
#define MENU_MRK_SETS_RGN				2205
#define MENU_MRK_SETS_COLOR				2206
#define MENU_MRK_SIZE					2207
//#define MENU_IMAGE_SIZE					2208
#define MENU_CAMERA_PARAMS_CHANGE		2209
#define MENU_CAMERA_PARAMS_SAVE		    2210
#define MENU_APPLY_IMG_TO_MOLD	        2211
#define MENU_SET_BKGND			        2212

//#define MENU_MOLD_DETAILS		2301
//#define MENU_IMG_TOOLBAR		2302

//#define MENU_ZOOM_1X			2303
//#define MENU_ZOOM_2X			2304
//#define MENU_ZOOM_3X			2305
//#define MENU_ZOOM_4X			2306

#define MENU_HELP_CONTENTS		1201
#define MENU_HELP_ABOUT			1202

#define PRODUCT_DIR_REGKEY "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IBModeler.exe"

enum CURRENT_WINDOW {
	CURRENT_WINDOW_APP,
	CURRENT_WINDOW_MOLD,
	CURRENT_WINDOW_MOLD_IMAGE,
	CURRENT_WINDOW_MODEL,
	CURRENT_WINDOW_CREATE_MOLD,
	CURRENT_WINDOW_POINT_RANGE,
	CURRENT_WINDOW_BACK_COLOR,
	CURRENT_WINDOW_DEFINE_CAM,
	CURRENT_WINDOW_LEGENDS,
	CURRENT_WINDOW_LOCATION,
	CURRENT_WINDOW_LOOKING_AT,
	CURRENT_WINDOW_MARKER_SIZE,
	CURRENT_WINDOW_MODEL_VIEW_DISTANCE,
	CURRENT_WINDOW_MODIFY_MOLD_PTS,
	CURRENT_WINDOW_ORIENTATION,
	CURRENT_WINDOW_RTN_SPEED,
	CURRENT_WINDOW_CREATE_MODEL,
	CURRENT_WINDOW_SET_BACK_SCREEN
	
} ;



//Menu options when the model is being viewed--------
#define MENU_MODEL_MODEL_INFO		3201
#define MENU_MODEL_MODEL_CLR		3202
#define MENU_MODEL_CHANGE_CLR		3203

#define MENU_MODEL_TGL_ROTATE		3204
#define MENU_MODEL_TGL_AXIS			3205
#define MENU_MODEL_RTN_SPEED		3206

#define MENU_MODEL_FRONT			3207
#define MENU_MODEL_TOP				3208
#define MENU_MODEL_LEFT				3209
#define MENU_MODEL_RIGHT			3210
#define MENU_MODEL_BACK				3211
#define MENU_MODEL_BOTTOM			3212

#define MENU_MODEL_DISTANCE			3213
#define MENU_MODEL_TGL_AXIS_DSPLY	3214

#define MENU_KEYBOARD_HELP			3302
//---------------------------------------------------

#define DEFLT_MARKER_SIZE		7

#define FLT_MAX_CLR				255.0f

#define MODEL_VIEW_DISTANCE_MULTIPLIER	   5.0f
#define MODEL_TIMER_ID					   1
#define MODEL_FLT_MAX_VIEW_DISTANCE		   10000.0f

#define UINT_MAX_AXES_POINTS			   1000


#define	UINT_BYTES_PER_KILOBYTE            1000
//Used only in the model.h file. Ideally it should always be used

//Functions
CString getErrDescription(unsigned long inLngErrCode);
BOOL sameSigns(float inVal1, float inVal2 );
float modulus(float inVal) ;
int getIntFromFloat(float inFltVal);
	

enum MODIFICATION_TYPE {
	NO_MODIFICATION,
	COLORS_SET_TO_BKGRND,
	RGN_SET_TO_BKGRND,
	TOP_PADDING_ALTERED,
	BOTTOM_PADDING_ALTERED,
	LEFT_PADDING_ALTERED,
	RIGHT_PADDING_ALTERED,
	TOP_PADDING_ALTER_UNDONE,
	BOTTOM_PADDING_ALTER_UNDONE,
	LEFT_PADDING_ALTER_UNDONE,
	RIGHT_PADDING_ALTER_UNDONE,
	COLORS_SET_TO_BKGRND_UNDONE,
	RGN_SET_TO_BKGRND_UNDONE
};

enum LOADED_CURSOR_TYPE {
	NORMAL_CURSOR,
	TOP_BOTTOM_PADDING_CURSOR,
	LEFT_RIGHT_PADDING_CURSOR,
	REGION_MARKER_CURSOR,
	COLOR_MARKER_CURSOR
};

enum MARKER_TYPE{
	SETS_REGION_AS_BACKGROUND,
	SETS_COLOR_AS_BACKGROUND	
};

enum COLOR_SELECTION_TYPE {
	SAT,
	LUM,
	CUR_AND_BRIGHTER,
	CUR_AND_DARKER,
	RED_SCREEN,
	GREEN_SCREEN,
	BLUE_SCREEN,
	NONE	
};


//Color
class RGBCOLOR {
public:
	BYTE red;
	BYTE blue;
	BYTE green;
	RGBCOLOR();
	BOOL operator == ( RGBCOLOR inC ) ; 
	BOOL operator != ( RGBCOLOR inC ) ; 
};

struct CAMERA_INFO {
	
	CAMERA_INFO() {
		blnDataInitialized = false;
		blnCameraAtInfinity = true;
		blnWtSameAsHt = false;
		
		fltInfinityHt = 20.0f;
		fltInfinityWt = 20.0f;
		fltHtDistance1 = 1.0f;
		fltHtDistance2 = 1.0f;
		fltHeight1 = 1.0f;
		fltHeight2 = 1.0f;
		fltWtDistance1 = 1.0f;
		fltWtDistance2 = 1.0f;
		fltWidth1 = 1.0f;
		fltWidth2 = 1.0f;
	}

	void operator = (CAMERA_INFO inCameraInfo) {
		
		blnDataInitialized = inCameraInfo.blnDataInitialized;
		blnCameraAtInfinity = inCameraInfo.blnCameraAtInfinity;
		blnWtSameAsHt = inCameraInfo.blnWtSameAsHt;

		if(blnCameraAtInfinity) {
			fltInfinityHt = inCameraInfo.fltInfinityHt;
			fltInfinityWt = inCameraInfo.fltInfinityWt;
		} else {
		
			fltHtDistance1 = inCameraInfo.fltHtDistance1;
			fltHtDistance2 = inCameraInfo.fltHtDistance2;
			fltHeight1 = inCameraInfo.fltHeight1;
			fltHeight2 = inCameraInfo.fltHeight2;

			if(!blnWtSameAsHt) {
				fltWtDistance1 = inCameraInfo.fltWtDistance1;
				fltWtDistance2 = inCameraInfo.fltWtDistance2;
				fltWidth1 = inCameraInfo.fltWidth1;
				fltWidth2 = inCameraInfo.fltWidth2;
			}
		}
	}

	BOOL blnDataInitialized;
	BOOL blnCameraAtInfinity;
	float fltInfinityHt;
	float fltInfinityWt;
	float fltHtDistance1;
	float fltHtDistance2;
	float fltHeight1;
	float fltHeight2;
	BOOL blnWtSameAsHt;
	float fltWtDistance1;
	float fltWtDistance2;
	float fltWidth1;
	float fltWidth2;
};

/**********************************from the h project****************************************/
//Camera range:
struct CAMERARATIO {
	float xRatio;
	float yRatio;
	float xRangeAtInfinity;
	float yRangeAtInfinity;
};


//TARGETCUBOID targetCuboid;

//xPoints, yPoints and zPoints specify the number of points along the three axes and 
//can have values from 2 to  UINT_MAX_AXES_POINTS. 

//minx, maxx, miny, maxy, minz, maxz can have values from - 500  to + 500.
struct TARGETCUBOID {

	UINT uintXPoints;
	UINT uintYPoints;
	UINT uintZPoints;
	float fltMinx;
	float fltMiny;
	float fltMinz;
	float fltMaxx;
	float fltMaxy;
	float fltMaxz;
};


//Points in 3d space:
//although this is defined as a class for operator overloading
//I will be using it more like a struct so it's name is in 
//capital letters
class FLTPOINT3D {//FLTPOINT3D fltPoint3d; Can be used to define the points in cms.

public: 
	float x;
	float y;
	float z;
	FLTPOINT3D() ;
	FLTPOINT3D( float inX, float inY, float inZ ) ;
	operator = ( FLTPOINT3D inP ) ;
	FLTPOINT3D operator + ( FLTPOINT3D inP );
	FLTPOINT3D operator - ( FLTPOINT3D inP ) ; //Added on 21st Jan 2004
	FLTPOINT3D operator * ( float inK ) ;
	FLTPOINT3D operator / ( float inK ) ;	

}; 


class VECTOR { 
public :
	VECTOR();
	VECTOR(float inI, float inJ, float inK);
	VECTOR (FLTPOINT3D inP);
	float i;
	float j;
	float k;
	operator = ( VECTOR inV ) ;
	VECTOR operator + ( VECTOR inV ) ; 
	VECTOR operator - ( VECTOR inV ) ; 
	VECTOR operator * ( VECTOR inV ) ; //vector cross product
	VECTOR operator * (float inK   ) ;//scalar multiplcation
	VECTOR operator / (float inK   ) ;//check for null value before calling this function
	float  modulus();
	
};

/*************************end of definitions from the h project******************************/
enum DISPLAY_TYPE {
	FRONT_VIEW,
	BACK_VIEW,
	TOP_VIEW, 
	BOTTOM_VIEW, 
	LEFT_VIEW, 
	RIGHT_VIEW
};

enum CAM_DRTN_SEL_TYPE {
	DRTN_COORDINATES,
	DRTN_ROTN_Y_AXIS,
	DRTN_ROTN_X_AXIS,
	DRTN_CUBE_CNTR,
	DRTN_ORGN
};

enum CAM_POS_SEL_TYPE {
	POS_COORDINATES,
	POS_ROTN_CUBE_Y_AXIS,
	POS_ROTN_CUBE_X_AXIS,
	POS_ROTN_Y_AXIS,
	POS_ROTN_X_AXIS
};


struct CAM_ORIENTATION {
	
	BOOL blnDataInitialized;
	
	FLTPOINT3D camPos, lookingAt;
	
	CAM_POS_SEL_TYPE camPosSelType;

	CAM_DRTN_SEL_TYPE camDrtnSelType;

	DISPLAY_TYPE displayType;
		
	CAM_ORIENTATION() {
		blnDataInitialized = false;
		camPosSelType = POS_COORDINATES;
		camDrtnSelType = DRTN_CUBE_CNTR;
		displayType = TOP_VIEW;		
	}

	operator = (CAM_ORIENTATION inCamOrntn) {
		blnDataInitialized = inCamOrntn.blnDataInitialized;
		camPos = inCamOrntn.camPos;
		lookingAt = inCamOrntn.lookingAt;
		camPosSelType = inCamOrntn.camPosSelType;
		camDrtnSelType = inCamOrntn.camDrtnSelType;
		displayType = inCamOrntn.displayType;		
	}	
};

//Borrowed from the Dll for processing the mold using the image===============================
//Image Parameters:
struct IMAGEPARAMS  {
	RGBCOLOR InvalidColor;
	UINT height;
	UINT width;
	unsigned char extraBytes;
	UINT leftPadding;
	UINT rightPadding;
	UINT topPadding;
	UINT bottomPadding;
	FLTPOINT3D cameraLocation;
	FLTPOINT3D lookingAt; 
	BOOLEAN flgAtInfinity;
};
//============================================================================================

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

enum THREAD_OPERATION_TYPE {
	PROCESS_MOLD_SNAPS,
	PROCESS_MOLD_SLIDES,
	SET_BK_COLOR,
	CREATE_MODEL,
	CREATE_MOLD
};


#define SECURITY_STRING_IMGTOOBJ "S^&S$G#D#G)(G(*^D<ytSggDnFn"
#define SECURITY_STRING_PTSTOPOLYGONS "<C!@%GM&&I^(&E%jhKqLqJj#!@~hwJqAj"
#define SECURITY_STRING_SLIDESTOOBJ "InP@!$#L@K)_Z)(W&~!aXaQmoNijUye"

struct PROCESS_INFO {
	PROCESS_INFO() {
		g_blnCancelProcess = false;
		g_fltPercentComplete = 0.0f;
		g_uintTotalCount = 0;
		g_strInputFile = "";
		g_strOutputFile = "";
		g_strSecurity = "";

	}
private:
	BOOL g_blnCancelProcess;
	UINT g_uintTotalCount;
	float g_fltPercentComplete;
	CString g_strSecurity;

public:
	CString g_strInputFile;
	CString g_strOutputFile;	

	void setSecurity(CString inStrSecurity) {
		g_strSecurity = inStrSecurity;
	}

	CString getSecurity() {
		return g_strSecurity;
	}

	void cancelProcess() {
		g_blnCancelProcess = true;
	}
		
	float getCompletedPercent() {
		return g_fltPercentComplete;
	}
	
	BOOL isCancelled() {
		return g_blnCancelProcess;
	}

	void initMaxNumber(UINT inUintMaxNumber) {
		g_uintTotalCount = inUintMaxNumber;
	}
	
	void setNewPercent(UINT inUintCompleted) {
		VERIFY(g_uintTotalCount > 0 && inUintCompleted <= g_uintTotalCount);
		float fltPercentComplete = ((float)inUintCompleted/(float)g_uintTotalCount)*100.0f;
		VERIFY(fltPercentComplete >= g_fltPercentComplete);
		g_fltPercentComplete = fltPercentComplete;
	}
};

struct PROCESS_MLD_INFO : PROCESS_INFO {

	BYTE *imgPointer; 
	IMAGEPARAMS imgParams;
	CAMERARATIO cameraRatio;
	
	UINT uintNewXPts, uintNewYPts; //If the first slide is being processed and these points 
								 //are other than zero the cuboid X and Y points are modified
								//with the new values
	PROCESS_MLD_INFO() {
		uintNewXPts = uintNewYPts = 0;		
	}
};

//Structure used to pass all the information to the worker thread for converting all specified
//colors in an image to the background color
struct SET_BK_COLOR_INFO : PROCESS_INFO {
	BYTE btR;
	BYTE btG;
	BYTE btB; 
	COLOR_SELECTION_TYPE selType;
	BYTE btLumSatVal;
	LPVOID lpvImgBits;
	int intWidth; 
	int intHeight;
	int intExtraBts;
	int btBkR;
	int btBkG;
	int btBkB;
};

struct CREATE_MODEL_INFO : PROCESS_INFO {
	HMODULE moduleHandle;
	RGBCOLOR modelClr;
	UINT intMinx;
	UINT intMaxx;
	UINT intMiny;
	UINT intMaxy;
	UINT intMinz;
	UINT intMaxz;
};

struct CREATE_MOLD_INFO : PROCESS_INFO {
	UINT bytesToWrite; 
};
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

struct MODEL_POINT_RANGE {
	float fltMinx;
	float fltMaxx;
	float fltMiny;
	float fltMaxy;
	float fltMinz;
	float fltMaxz;
};

struct MODEL_INFO {

	CString strFilePath;
	UINT intFileSize;

	UINT intTotTriangles;
	float fltMinx;
	float fltMaxx;
	float fltMiny;
	float fltMaxy;
	float fltMinz;
	float fltMaxz;

	float fltViewDistance;
	CString strCurrentView;

	void operator = (MODEL_INFO inModelInfo) {
		strFilePath = inModelInfo.strFilePath;
		intFileSize = inModelInfo.intFileSize;

		intTotTriangles = inModelInfo.intTotTriangles;
		fltMinx = inModelInfo.fltMinx;
		fltMaxx = inModelInfo.fltMaxx;
		fltMiny = inModelInfo.fltMiny;
		fltMaxy = inModelInfo.fltMaxy;
		fltMinz = inModelInfo.fltMinz;
		fltMaxz = inModelInfo.fltMaxz;

		fltViewDistance = inModelInfo.fltViewDistance;
		strCurrentView = inModelInfo.strCurrentView;
	}
};

enum KEYBOARD_MAP_TYPE {
	MODEL,
	MOLD
};


#endif
