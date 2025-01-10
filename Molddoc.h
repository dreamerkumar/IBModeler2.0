#ifndef MOLDDOC_H_
#define MOLDDOC_H_


#include <afxwin.h>
#include <afxext.h>
#include "definitions.h"
#include "errorConstants.h"
#include "identifybkgnd.h"
#include "scriptreader.h"


class moldDoc : public CDocument {
	DECLARE_DYNCREATE(moldDoc)
	
public:
	moldDoc();
	BOOL OnNewDocument();
	BOOL OnOpenDocument(LPCSTR filePath);
	virtual void OnCloseDocument();
	void DeleteContents();
	CString getMoldDetails();//Function called by the left frame to display the mold details

	LPRECT getLpPreviousRect();
	void setLpPreviousRect(CRect inRect);
	void setLpPreviousRectToNull();
private:
	//Variables

	LPRECT g_lpPreviousRect; //Previous rectangle drawn when the marker is set regions to background.

	//Strings to store the mold information for left frame's display
	CString g_strMoldDetails, g_strImageDetails;


	CString g_strFilePath; //File path of the mold file
	TARGETCUBOID cuboidParams;
	BOOL flgSnaps, flgBitmapLoaded, flgDocLoaded, flgScrollSizeSet;
	BOOL flgImgModified, flgSavedOnce, flgMoldProcessed, flgPadModified;
	BOOL flgFirstLoad, flgNewImage; 
	int intFilesProcessed;	
	float fltInterSlideDistance;
	BYTE intMagnification;
	UINT intWindowWidth, intWindowHeight;
	UINT intTpPadding, intBtmPadding, intLtPadding, intRtPadding;
	UINT intPrevTpPadding, intPrevBtmPadding, intPrevLtPadding, intPrevRtPadding;

	CString g_strImgPath;

	BOOL g_blnPadMsgShown; //This flag is used to show the information regarding inability to
	//store the padding information (if padding was applied to the image ). The flag is used 
	//as a check to display the message only once
	
	//Marker properties
private :
	BYTE markerSize;
	MARKER_TYPE markerType;
public: 
	BYTE getMarkerSize() { return markerSize; }
	MARKER_TYPE getMarkerType();// {	return markerType; }


	//Background color
private:
	RGBCOLOR backgroundColor;
public:
	RGBCOLOR getBackgroundColor() ;//{	return backgroundColor; }
	
	//Draw color
private:
	RGBCOLOR drawColor;
public:
	RGBCOLOR getDrawColor();// { return drawColor; }

	//Padding color
private:
	RGBCOLOR padColor;

	//structure to store information about the camera
	CAMERA_INFO cameraInfo;

	BOOL setCameraParams(BOOL inBlnSetParamsOnly);//Function called to set the camera params
	//Can be called by the  setCameraParamsOnly as well as applyToMold function
	void setCameraParamsOnly(); //Called through the menu option 

	void saveCameraParams(); //Function called for saving the camera params
	
	//Private Functions
private:
	void initialize();
	void updtImgWindowSize();
	BOOL CallSetDIBits(LPVOID newImgBits);
	BOOL CallGetDIBits(LPVOID imgBits);
	void undo();
	void redo();
	BOOL saveImg();
	BOOL saveImgNew(BOOL blnPromptOverwrite, CString inStrFilePath = "");
	void saveImgNewPrompt();
	BOOL closeImageFile(BOOL blnPromptMessages = TRUE);
	void changeMarkerSize();
	void setMarkerType_Rgn();
	void setMarkerType_Color();
	
	//Functions to enable/disable menu items	
	void showLoadImage(CCmdUI *item);
	void showImageOptions(CCmdUI *item);

	
//Image variables------------------------------	
private:
	
	CBitmap m_Bitmap;
	BITMAPINFOHEADER* pBMIH;
	BYTE* abBitmapInfo[BITMAPINFOHEADER_SIZE] ;
   
	int intHeight, intWidth, extraBytes; 
	UINT g_uintTotalImgBytes;
	LPVOID lpvImgBits, lpvPrevImgBits; //to store the image pixels
	HBITMAP hImage; //Handle to hold the image 
	
	CDC memDC; //The bitmap is selected into this memory DC for drawing operations
	

	MODIFICATION_TYPE modType; //used by the undo and redo functions
public:
	int getImgHeight() ;//{ return intHeight;}
	int getImgWidth() ;//{ return intWidth; }	
	CDC* getBitmapDC() ;//{return &memDC;}
	//List of public functions to retrieve doc values
	CString getFilePath();// { return strFilePath; }
	BOOL isSnaps() ;//{ return flgSnaps; }
	BOOL isBitmapLoaded();// { return flgBitmapLoaded; }
	BOOL isDocLoaded();// { return flgDocLoaded; }
	BOOL isScrollSizeSet();// { return flgScrollSizeSet; }
	BOOL isFirstLoad();// { return flgFirstLoad;}
	void resetFirstLoadFlag();// { flgFirstLoad = false;}
	void setScrollSizeFlag() ;//{ flgScrollSizeSet = true; }
	BOOL isNewImage() ;//{ return flgNewImage; }
	void resetNewImageFlag() ;//{ flgNewImage = false; }
	BOOL isPadModified() ;//{ return flgPadModified; }
	void resetPadModifiedFlag();// { flgPadModified = false; }
		
	int getFilesProcessed() ;//{ return intFilesProcessed; }
	float getInterSlideDistance();// { return fltInterSlideDistance; }
	int getMagnification() ;//{ return intMagnification; }
	UINT getWindowWidth() ;//{ return intWindowWidth;}
	UINT getWindowHeight() ;//{ return intWindowHeight;}
	
	//Functions for the padded regions of the image
	UINT getTpPadding()  ;//{ return intTpPadding;  } 
	UINT getBtmPadding() ;//{ return intBtmPadding; } 
	UINT getLtPadding()  ;//{ return intLtPadding;  } 
	UINT getRtPadding()  ;//{ return intRtPadding;  } 
	void setTpPadding(UINT intVal);
	void setBtmPadding(UINT intVal);
	void setLtPadding(UINT intVal);
	void setRtPadding(UINT intVal);
	void drawPadRectangle(int intLeft, int intTop, int intRight, int intBottom);
	

	//Bitmap functions
	void selectBitmap(CDC *pDCMem) ;//{ CBitmap* tempBitmap = pDCMem->SelectObject(&m_Bitmap);}
		
	//Function to set selected colors to the background color
	BOOL setColorsToBkgrnd(BYTE inBtRed, BYTE inBtGreen, BYTE inBtBlue, 
		COLOR_SELECTION_TYPE inSelectionType, BYTE inBtSatLumVal);
	
	//Function called by the bitmap view class after setting a particular region to background
	void setModStatus_RgnToBk();

	//Functions to change magnification of the image
/*	void changeMagnification1X() { intMagnification = 1; updtImgWindowSize(); }
	void changeMagnification2X() { intMagnification = 2; updtImgWindowSize(); }
	void changeMagnification3X() { intMagnification = 3; updtImgWindowSize(); }
	void changeMagnification4X() { intMagnification = 4; updtImgWindowSize(); }
*/

private:
	
	
	void loadImage();

	BOOL saveAs24BppBitmap(CString inStrFilePath);

	BOOL reloadAs24Bpp();

	
	//Function to set the camera orientation
	BOOL setCameraOrntn();

	//After all the image parameters is set, this function is called to process the mold using
	//the image
	BOOL applyToMold(UINT inUintNewXPts = 0, UINT inUintNewYPts = 0); 

	//After all the images are processed, this function is used to create the actual model 
	//using the carved out mold.
	BOOL createModel(SCR_CREATEMODELPART_PARAMS *ptrParams = NULL);
private:
	CAM_ORIENTATION camOrientation;

	RGBCOLOR modelColor;

	virtual BOOL SaveModified();
	
	DECLARE_MESSAGE_MAP()
protected: //Could have private or protected
	CToolBar mainToolBar, imageToolBar;
	CReBar reBar;

	void dummyFunction(); //Used to nullify the calls of App class through accelerator keys

	void showKeyboard();

private:
	BKGND_SCREEN c_bkgndScreen;
	void onSetBkgndScreen();

	void runScriptAndSetFlag();
	void runScript(); 

	BOOL loadImage1(CString inStrPath);

	BOOL blnSuppressPrompts;

	//All the custom functions that are called through the menu options should not have a return type
	//I found those function calls through the menu option crashing, in the release version. This 
	//bug was also the hardest to crack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	void saveImgMenu();

	void closeImageFileMenu(); 

	void createModelMenu() ;

	void applyToMoldMenu(); 

	//~~~~~~~~~~~~~~~~~~~~~~~~~

	
};

#endif
