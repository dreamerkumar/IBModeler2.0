#include <afxwin.h>
#include "molddoc.h"
#include "myframe.h"
#include "afxdlgs.h"
#include "cameraorntn.h"
#include "htmlhelp.h"
#include "myapp.h"
#include "math.h"


IMPLEMENT_DYNCREATE(moldDoc, CDocument)

BEGIN_MESSAGE_MAP(moldDoc, CDocument)
	ON_COMMAND(MENU_RUN_SCRIPT, runScriptAndSetFlag)		

	ON_COMMAND(ID_FILE_NEW, dummyFunction) 

	ON_COMMAND(ID_FILE_OPEN, loadImage)
	ON_COMMAND(MENU_SAVE_IMG, saveImgMenu)
	ON_COMMAND(MENU_SAVE_AS_BITMAP, saveImgNewPrompt)
	ON_COMMAND(MENU_CLOSE_IMG, closeImageFileMenu)
	ON_COMMAND(MENU_CREATE_MODEL, createModelMenu)
	ON_COMMAND(MENU_UNDO, undo)
	ON_COMMAND(MENU_REDO, redo)
	ON_COMMAND(MENU_MRK_SIZE, changeMarkerSize)
	ON_COMMAND(MENU_MRK_SETS_RGN, setMarkerType_Rgn)
	ON_COMMAND(MENU_MRK_SETS_COLOR, setMarkerType_Color)
	ON_COMMAND(MENU_CAMERA_PARAMS_CHANGE, setCameraParamsOnly)
	ON_COMMAND(MENU_CAMERA_PARAMS_SAVE, saveCameraParams)
	ON_COMMAND(MENU_APPLY_IMG_TO_MOLD, applyToMoldMenu)
	ON_COMMAND(MENU_SET_BKGND, onSetBkgndScreen)
	//ON_COMMAND(MENU_ZOOM_1X, changeMagnification1X)
	//ON_COMMAND(MENU_ZOOM_2X, changeMagnification2X)
	//ON_COMMAND(MENU_ZOOM_3X, changeMagnification3X)
	//ON_COMMAND(MENU_ZOOM_4X, changeMagnification4X)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, showLoadImage)
	ON_UPDATE_COMMAND_UI_RANGE( MENU_SAVE_IMG,  MENU_CLOSE_IMG, showImageOptions)
	ON_UPDATE_COMMAND_UI_RANGE(MENU_UNDO, MENU_SET_BKGND, showImageOptions)
	//ON_UPDATE_COMMAND_UI_RANGE(MENU_IMG_TOOLBAR, MENU_ZOOM_4X, showImageOptions)
	//ON_UPDATE_COMMAND_UI(MENU_IMG_TOOLBAR, showImageOptions)
	ON_COMMAND(MENU_KEYBOARD_HELP, showKeyboard)
	
END_MESSAGE_MAP()

//All the custom functions that are called through the menu options should not have a return type
//I found those function calls through the menu option crashing, in the release version. This 
//bug was also the hardest to crack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void moldDoc::saveImgMenu() {
	saveImg();
}

void moldDoc::closeImageFileMenu() {
	closeImageFile();
}

void moldDoc::createModelMenu() {
	createModel();
}

void moldDoc::applyToMoldMenu() {
	applyToMold();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void moldDoc::runScriptAndSetFlag() {
	runScript();
	blnSuppressPrompts = false;
}


//Don't call the following function directly. Call it through runScriptAndSetFlag so that
//the flag to display the message prompts is reset once this funciton returns.
void moldDoc::runScript() {

	try {
		CFileDialog fd(1, 0, 0, 0, "Script file(*.txt)|*.txt||");
		fd.m_ofn.lpstrTitle = "Load the script file";
		fd.m_ofn.Flags = fd.m_ofn.Flags | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
		if(fd.DoModal() != IDOK)
			return;

		CString strPath = fd.GetPathName();
		delete fd;

		CString strMessage;
		CString strAbortScrMsg = "\nAborting script";


		scriptReader myScriptReader;
		if(!myScriptReader.openFile(strPath, strMessage)) {
			AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
			return;			
		}
		
		
		blnSuppressPrompts = true; //Suppress prompts while the script is being executed
		
		SCR_INSTRUCTION scrInstruction;
		
		while(1 == 1) { //Keep reading until end of file is reached

			if(!myScriptReader.readNextValue(scrInstruction, strMessage)) {
				
				if(strMessage == "End of file reached") {
					AfxMessageBox("The script commands have been successfully executed." ,  MB_ICONINFORMATION  | MB_OK, NULL);
					return;
				} else if(strMessage == "Blank line") { //No error message for blank line
					continue;
				} else {
					//Display the error message and exit
					AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
					return;
				}
			}
			
			
			switch(scrInstruction.instructionType) {
			
			case SCR_LOADIMAGE:
				{ //enclosing parenthesis is required to declare a variable inside a case statement

					SCR_IMAGEPATH_PARAMS *ptrParams =(SCR_IMAGEPATH_PARAMS *)scrInstruction.getParams();

					if(!loadImage1(ptrParams->imagepath)) {
						strMessage.Format("Error occured while loading file '%s'.\nThe error occured at line number %d", 
							ptrParams->imagepath, myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;			
					}
				}
				break;



			case SCR_SETBACKGROUNDSCREEN:
				{
					SCR_SETBACKGROUNDSCREEN_PARAMS *ptrParams =(SCR_SETBACKGROUNDSCREEN_PARAMS  *)scrInstruction.getParams();
					
					BYTE btRed =0;
					BYTE btGreen =0;
					BYTE btBlue = 0;
					COLOR_SELECTION_TYPE clrSelType;
					
					switch(ptrParams->screenType) {
					
					case SCR_SCREENTYPE_RED:
						btRed = ptrParams->ucharMinColorOffset;
						clrSelType = RED_SCREEN;
						break;
					
					case SCR_SCREENTYPE_GREEN:
						btGreen = ptrParams->ucharMinColorOffset;
						clrSelType = GREEN_SCREEN;
						break;
					
					case SCR_SCREENTYPE_BLUE:
						btBlue = ptrParams->ucharMinColorOffset;
						clrSelType = BLUE_SCREEN;
						break;
					}

					
					if(!setColorsToBkgrnd(btRed, btGreen, btBlue, clrSelType, ptrParams->ucharMaxDiffPercent)) {

						strMessage.Format("Error occured while setting a background screen.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;			

					}
						
				}
				break;
			
			case SCR_SETBACKGROUNDCOLOR:
				{
					SCR_SETBACKGROUNDCOLOR_PARAMS *ptrParams =(SCR_SETBACKGROUNDCOLOR_PARAMS  *)scrInstruction.getParams();
					if(!setColorsToBkgrnd(ptrParams->ucharBgColorR, ptrParams->ucharBgColorG, ptrParams->ucharBgColorB, 
						ptrParams->colorSelType, ptrParams->ucharVarianceVal)) {

						strMessage.Format("Error occured while setting background color.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;			

					}

				}
				
				break;
			
			case SCR_SETCAMERAPARAMS:
				{
					
					if(!flgSnaps) {
						strMessage.Format("Invalid instruction.\nSETCAMERAPARAMS is not applicable to this mold as it uses slides.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}
					
					SCR_SETCAMERAPARAMS_PARAMS *ptrParams =(SCR_SETCAMERAPARAMS_PARAMS  *)scrInstruction.getParams();
	
					cameraInfo.blnDataInitialized = true;
					cameraInfo.blnCameraAtInfinity = false; //Camera not at infinity
					cameraInfo.blnWtSameAsHt = false;
		
					cameraInfo.fltHtDistance1 = ptrParams->fltH1Distance;
					cameraInfo.fltHtDistance2 = ptrParams->fltH2Distance;
					cameraInfo.fltHeight1 = ptrParams->fltH1;
					cameraInfo.fltHeight2 = ptrParams->fltH2;
					cameraInfo.fltWtDistance1 = ptrParams->fltW1Distance;
					cameraInfo.fltWtDistance2 = ptrParams->fltW2Distance;
					cameraInfo.fltWidth1 = ptrParams->fltW1;
					cameraInfo.fltWidth2 = ptrParams->fltW2;
				}
				
				break;
			
			case SCR_SETCAMERAPARAMS_INFINITY:
				{
					
					if(!flgSnaps) {
						strMessage.Format("Invalid instruction.\nSETCAMERAPARAMS_INFINITY is not applicable to this mold as it uses slides.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}
					
					
					
					
					
					
					SCR_SETCAMERAPARAMS_INFINITY_PARAMS *ptrParams =(SCR_SETCAMERAPARAMS_INFINITY_PARAMS  *)scrInstruction.getParams();
					
					cameraInfo.blnDataInitialized = true;
					cameraInfo.blnCameraAtInfinity = true;
					cameraInfo.blnWtSameAsHt = false;

					cameraInfo.fltInfinityHt = ptrParams->fltHeight;
					cameraInfo.fltInfinityWt = ptrParams->fltWidth;
				}
				
				break;
			
			case SCR_APPLYIMAGE_SNAPS:
				{
					SCR_APPLYIMAGE_SNAPS_PARAMS *ptrParams =(SCR_APPLYIMAGE_SNAPS_PARAMS  *)scrInstruction.getParams();
					
					//Check whether the instruction is applicable
					if(!flgSnaps) {
						strMessage.Format("Invalid instruction.\nAPPLYIMAGE_SNAPS is not applicable to this mold as it uses slides.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}
					
					camOrientation.camPos.x		= ptrParams->fltCameraLocationX; 
					camOrientation.camPos.y		= ptrParams->fltCameraLocationY;
					camOrientation.camPos.z		= ptrParams->fltCameraLocationZ;
					camOrientation.lookingAt.x	= ptrParams->fltPointingToX;
					camOrientation.lookingAt.y	= ptrParams->fltPointingToY;
					camOrientation.lookingAt.z	= ptrParams->fltPointingToZ;
					
					if(!applyToMold()) {
						strMessage.Format("Error occured while applying image.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}

				}
				
				break;
			
			case SCR_APPLYIMAGE_SLIDES:
				{
					SCR_APPLYIMAGE_SLIDES_PARAMS *ptrParams =(SCR_APPLYIMAGE_SLIDES_PARAMS  *)scrInstruction.getParams();

					//Check whether the instruction is applicable
					if(flgSnaps) {
						strMessage.Format("Invalid instruction.\nAPPLYIMAGE_SLIDES is not applicable to this mold as it uses snaps.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}

					//Check whether the image is loaded
					if(!flgBitmapLoaded) {
						strMessage.Format("Cannot apply image. No image is loaded.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;
					}
					
					
					unsigned int uintNewXVals = 0;
					unsigned int uintNewYVals = 0;

					if(ptrParams->intModifyPtsVal > 0 ) {
						
						if(intFilesProcessed != 0) {//Points cannot be modified

							strMessage.Format("Mold points cannot be modified as slides have already been added to it.\nThe error occured at line number %d", 
								myScriptReader.getLineNumber() );
							AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
							return;				
					
						} else {
							//Calculate and assign the new values along the X and Y axis.for the mold
							int intActualImgWidth, intActualImgHeight;
							intActualImgWidth = intWidth - intLtPadding - intRtPadding;
							intActualImgHeight = intHeight -intTpPadding - intBtmPadding;
							
							uintNewXVals = ptrParams->intModifyPtsVal * intActualImgWidth + 1;
							uintNewYVals = ptrParams->intModifyPtsVal * intActualImgHeight + 1;

							if(uintNewXVals > UINT_MAX_AXES_POINTS || uintNewYVals > UINT_MAX_AXES_POINTS) {

								strMessage.Format("Error occured while adding image as slide.\nCannot modify the mold point density.\n The new values calculated on the basis of parameter modifyPtsValue exceed the max value of %d along any axis.\nThe error occured at line number %d", 
									UINT_MAX_AXES_POINTS, myScriptReader.getLineNumber() );
								AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
								return;	
							}
						}
					}
					
					if(!applyToMold(uintNewXVals, uintNewYVals)) {
						strMessage.Format("Error occured while adding image as slide.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}
					
				}
				
				break;
			
			case SCR_SAVEIMAGE:
				{
					if(!saveImg()) {
						strMessage.Format("Error occured while saving file.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}
				}
				
				break;
			
			case SCR_SAVEIMAGEAS:
				{
					SCR_IMAGEPATH_PARAMS *ptrParams =(SCR_IMAGEPATH_PARAMS  *)scrInstruction.getParams();
					if(!saveImgNew(FALSE, ptrParams->imagepath)) {
						strMessage.Format("Error occured while saving file at location \n'%s'.\nThe error occured at line number %d", 
							ptrParams->imagepath, myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}
				}
				
				break;
			
			case SCR_CLOSEIMAGE:
				{
					if(!closeImageFile(FALSE)) {					
						strMessage.Format("Error occured while closing file.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;			
					}
				}
				
				break;
			
	
			case SCR_CREATEMODEL:
			case SCR_CREATEMODELPART:
				{
					SCR_CREATEMODELPART_PARAMS *ptrParams =(SCR_CREATEMODELPART_PARAMS  *)scrInstruction.getParams();
					
					if(ptrParams->intMaxXIndex > cuboidParams.uintXPoints || 
						ptrParams->intMaxYIndex > cuboidParams.uintYPoints ||
						ptrParams->intMaxZIndex > cuboidParams.uintZPoints) {
						
						strMessage.Format("Cannot create model.\nSupplied index values exceed the mold points.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber());
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	

					}
					
					if(!createModel(ptrParams)) {
						strMessage.Format("Error occured while creating model.\nThe error occured at line number %d", 
							myScriptReader.getLineNumber() );
						AfxMessageBox(strMessage + strAbortScrMsg,  MB_ICONERROR | MB_OK, NULL);
						return;	
					}
				}
				break;
			}			
		}
		

	} catch (CException *e) {
		e->Delete();
		AfxMessageBox("Error occured while running the script.",  MB_ICONERROR | MB_OK, NULL);
		return;
	}
	
	return;

}

void moldDoc::onSetBkgndScreen() {
	
	identifyBkgnd dlg(IDD_DIALOG20, c_bkgndScreen);
	
	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_SET_BACK_SCREEN);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD_IMAGE);

	
	if(intRtnVal == IDOK) {
	
		c_bkgndScreen = dlg.getBkgndScreen();

		BYTE btRed = 0;
		BYTE btGreen = 0;
		BYTE btBlue = 0;
		COLOR_SELECTION_TYPE selectionType;
		BYTE btSatLumVal = c_bkgndScreen.uintMaxDiffPercent;

		
		switch(c_bkgndScreen.screenClr) {
		case RED:
			btRed = c_bkgndScreen.uintMinDiff;
			selectionType = RED_SCREEN;
			break;
		case GREEN:
			btGreen = c_bkgndScreen.uintMinDiff;
			selectionType = GREEN_SCREEN;
			break;
		case BLUE:
			btBlue = c_bkgndScreen.uintMinDiff;
			selectionType = BLUE_SCREEN;
			break;
		default:
			delete dlg;
			return;
		}

		setColorsToBkgrnd(btRed, btGreen, btBlue, selectionType, btSatLumVal);
	}

	delete dlg;

}

void moldDoc::showKeyboard() {
	dlgKeyboardMap dlg(IDD_DIALOG15, MOLD);
	dlg.DoModal();
	delete dlg;
}

void moldDoc::dummyFunction() {} //Used to nullify the calls of App class through accelerator keys

//THIS FUNCTION WILL NEVER BE CALLED
BOOL moldDoc::OnNewDocument() {
	//Base class function's code
	DeleteContents();//Delete previously created contents
	m_strPathName.Empty();   // no path name yet  
	return TRUE;
}
moldDoc::moldDoc() {
	flgBitmapLoaded = false;
	flgDocLoaded = false;
	flgFirstLoad = true; //Will be set to false by the view class
	g_strMoldDetails = "";//Used for display on the left frame
	//Initialize the camera orientation
	CAM_ORIENTATION temp;
	camOrientation = temp;
	initialize();	//Initialize the bitmap parameters
}
BOOL moldDoc::OnOpenDocument(LPCSTR f) {
	
//	if(!CDocument::OnOpenDocument(f))
//		return FALSE;

	g_strFilePath  =  f; //Get the file path
	
	
	//Check for an invalid file extension----------
	//------------------------------------------------------------------------------
	g_strFilePath.TrimRight();
	CString strExtension;
	strExtension = g_strFilePath.Right(4);
	strExtension.MakeUpper();
	if(strExtension !=  _T(".MLD")) {
		AfxMessageBox("Error: This file type is not supported by the application.",  MB_ICONERROR | MB_OK, NULL);
		return false;
	}	
	//------------------------------------------------------------------------------

	CFile fp;
	BYTE btOption;
	CString strError, strErrorInfo;

	

	if(!fp.Open(g_strFilePath, CFile::modeReadWrite | CFile::shareExclusive )) {
		AfxMessageBox("Error: Could not open file: \n'" + g_strFilePath + "'\n for writing.");
		return false;
	}	
	
	if(fp.GetLength() < MOLD_HEADER_SIZE) {
		AfxMessageBox("Error: Invalid File format: \n'" + g_strFilePath);
		return false;
	
	}

	try {
		fp.SeekToBegin();//Read the header info
		if(fp.Read(&cuboidParams.uintXPoints, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.uintYPoints, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.uintZPoints, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.fltMinx, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.fltMaxx, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.fltMiny, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.fltMaxy, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.fltMinz, 4) != 4 )
			throw new CException();
		if(fp.Read(&cuboidParams.fltMaxz, 4) != 4 )
			throw new CException();
		if(fp.Read(&btOption, 1) != 1)
			throw new CException();
		if(fp.Read(&intFilesProcessed, 4) != 4)
			throw new CException();
		if(fp.Read(&fltInterSlideDistance, 4) != 4)
			throw new CException();
		
		if(btOption == PROCESS_USING_SNAPS)
			flgSnaps = true;
		else
			flgSnaps = false;
		
		//Future use: Read any other header info if present----------------------------		
	
	} catch ( CException *e ) { 
		e->Delete();	
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
				fp.Close();
		strError = "Error: An error occured while reading from file: \n'" + g_strFilePath + "'";
		AfxMessageBox(strError, MB_OK | MB_ICONSTOP);
		return false;
	}	
	
	if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();

	
	//Generate a BITMAPINFO structure of 24 bpp format
	pBMIH = (BITMAPINFOHEADER*)abBitmapInfo;
    memset(pBMIH, 0, BITMAPINFOHEADER_SIZE);
	//pBMIH->biWidth        = intWidth; Will be initialized 
    //pBMIH->biHeight       = intHeight; afer image is loaded
	pBMIH->biSize        = BITMAPINFOHEADER_SIZE;
	pBMIH->biPlanes      = 1;
    pBMIH->biBitCount    = 24 ; 
    pBMIH->biCompression = BI_RGB ; //=======================	
	
	//Specify the default color for the model
	modelColor.red = DEFLT_MODEL_COLOR_R;
	modelColor.green = DEFLT_MODEL_COLOR_G;
	modelColor.blue = DEFLT_MODEL_COLOR_B;
	
	
	//Load/Unload the toolbars--------------------------------------
	mainToolBar.CreateEx(AfxGetMainWnd(), TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS );

	mainToolBar.LoadToolBar(IDR_TOOLBAR2);

	imageToolBar.CreateEx(AfxGetMainWnd(), TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS );

	imageToolBar.LoadToolBar(IDR_TOOLBAR3);

	reBar.Create(AfxGetMainWnd());
	reBar.AddBar(&mainToolBar); 
	reBar.AddBar(&imageToolBar); 
	
	//Initially hide the image toolbar as no image is loaded.
	((myFrame *)AfxGetMainWnd())->ShowControlBar(&imageToolBar, FALSE, FALSE);

	//We need to hide the main toolbar if a document is being opened.
	((myFrame *)AfxGetMainWnd())->showHideToolBar(FALSE);
	
	
	//Generate the initial info for display of mold details on the left frame------------------
	g_strMoldDetails = "Mold Details\t\n-------------------------------------\t\nUses ";
	g_strMoldDetails+= (flgSnaps? " pictures " : " slides of cut sections ") ;
	g_strMoldDetails+= "of the model.\t\n";
	g_strMoldDetails+= "%d images processed\t\n-------------------------------------\t\nPoint Ranges\t\n";
	g_strMoldDetails+= "----------------------\t\n";
	CString strTemp;
	strTemp.Format("X  %4.2f to %4.2f\t\nY  %4.2f to %4.2f\t\nZ  %4.2f to %4.2f\t\n\t\n",
		cuboidParams.fltMinx,
		cuboidParams.fltMaxx,
		cuboidParams.fltMiny,
		cuboidParams.fltMaxy,
		cuboidParams.fltMinz,
		cuboidParams.fltMaxz);

	g_strMoldDetails+= strTemp;
	g_strMoldDetails+= "Total Points\t\n-------------------\t\nX  %d\t\nY  %d\t\nZ  %d\t\n", 
	
	
	g_strMoldDetails+= "\t\nLocation  " + g_strFilePath;
	//-----------------------------------------------------------------------------------------
	
	flgDocLoaded = true;
	
	//Set the Mainframe's flag window to true, so that another document would not be loaded
	((myFrame *)AfxGetMainWnd())->setDocLoadedFlag(TRUE);

	blnSuppressPrompts = false; //All prompts should be displayed by default

	return true;
}


void moldDoc::DeleteContents() {
	//delete all
}

void moldDoc::loadImage() {
		
	
	CFileDialog fd(1, 0, 0, 0, "Bitmap Images(*.bmp)|*.bmp|Bitmap Images(*.dib)|*.dib||");
	fd.m_ofn.lpstrTitle = "Load a bitmap image to process mold";
	fd.m_ofn.Flags = fd.m_ofn.Flags | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	if(fd.DoModal() != IDOK)
		return;

	CString strPath = fd.GetPathName();
	loadImage1(strPath);
}

BOOL moldDoc::loadImage1(CString inStrPath) {
	
	if(flgBitmapLoaded) {
		AfxMessageBox("An image is already open.\n Please close it before opening a new image.", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
		
	BeginWaitCursor();

	//Load the bitmap*********
	hImage = (HBITMAP)LoadImage(NULL, inStrPath, 
		IMAGE_BITMAP,0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION|
		LR_DEFAULTSIZE);
	if (!hImage) {
		AfxMessageBox("LoadImage failed");
		return FALSE;
	}//--------------------------------------------------------
	
	
	//Attach the image handle to a CBitmap object*****
	if (!m_Bitmap.Attach(hImage)) {
		AfxMessageBox("Bitmap could not be attached");
		return FALSE;
	}//----------------------------------------------
	
	//Get image information into a bitmap structure***
	BITMAP pBitMap;
	m_Bitmap.GetBitmap(&pBitMap);
	//------------------------------------------------
	
	//Get the dimensions of the image********
	intHeight = pBitMap.bmHeight;
	intWidth  = pBitMap.bmWidth;
	BYTE rem = (intWidth * 3 )%4;
	if (rem > 0 ) 
		extraBytes = 4 - rem ; 
	else 
		extraBytes = 0; 
	g_uintTotalImgBytes = (intWidth+extraBytes)*intHeight*3;
	//---------------------------------------
	
	
	// Fill in the header info.**************
    pBMIH->biWidth        = intWidth;
    pBMIH->biHeight       = intHeight;
	//---------------------------------------

	//Allocate memory to store bitmap bits**************
	if ((lpvImgBits = malloc(g_uintTotalImgBytes))== NULL) {
		AfxMessageBox("Memory could not be allocated");
		return FALSE;
	}
	if ((lpvPrevImgBits = malloc(g_uintTotalImgBytes))== NULL) {
		AfxMessageBox("Memory could not be allocated");
		return FALSE;
	}//-----------------------------------------------------------------------

	
	
	//Reload the file in 24 bytes per pixel format
	if(!reloadAs24Bpp()) { 
		closeImageFile(FALSE);
		return FALSE;
	}

	//CallGetDIBits(lpvImgBits); //Get the pixel bits of the loaded bitmap
	//This call is already made while reloading the image in the above statement.
	

	flgBitmapLoaded = true;
	flgNewImage = true;
	updtImgWindowSize();
	if(intWidth <= 0 || intHeight <= 0 ) {
		AfxMessageBox("Error: Invalid bitmap dimensions.\n The image file will be closed now.");
		closeImageFile(FALSE);
		return FALSE;
	}
	
	g_strImgPath = inStrPath; //Store the location of the file

	//Load the image toolbar 
	((myFrame *)AfxGetMainWnd())->ShowControlBar(&imageToolBar, TRUE, FALSE);

	//Prepare the string used for the display of the left frame
	g_strImageDetails = "\t\n\t\nCurrent Image  " + g_strImgPath;
	
	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD_IMAGE);

	
	EndWaitCursor();

	return TRUE;
	
}

BOOL moldDoc::reloadAs24Bpp() {

	CString strNewFilePath = g_strFilePath + ".bak";
	
	if(!saveAs24BppBitmap(strNewFilePath))
		return false;

	//Delete the old object
	m_Bitmap.DeleteObject();

	//Load the bitmap------------------------------------------
	hImage = (HBITMAP)LoadImage(NULL, strNewFilePath, 
		IMAGE_BITMAP,0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION|
		LR_DEFAULTSIZE);
	if (!hImage) {
		AfxMessageBox("LoadImage failed");
		return false;
	}//--------------------------------------------------------
	
	
	//Attach the image handle to a CBitmap object----
	if (!m_Bitmap.Attach(hImage)) {
		AfxMessageBox("Bitmap could not be attached", MB_OK | MB_ICONSTOP);
		return false;
	}

	CFile fp;
	try {
		fp.Remove(strNewFilePath);
	} catch(CException *e) {
		e->Delete();
		AfxMessageBox("Error occured while deleting temporary file.", MB_OK | MB_ICONSTOP);
		//We need not return false for this error
	}

	BITMAP pBitMap;
	m_Bitmap.GetBitmap(&pBitMap);

	if(pBitMap.bmBitsPixel   != 24) {
		AfxMessageBox("Error in loading image.\nComputer does not support 24 bytes per pixel color format.\n", MB_OK | MB_ICONSTOP);
		m_Bitmap.DeleteObject();//close image file won't delete it in this case, since the load image
		//flag that it checks for, is still false.
		return false;
	}

	return true;
}

BOOL moldDoc::CallGetDIBits(LPVOID imgBits) {

	//Create a DC for the GetDIBits function**
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	//----------------------------------------
	
	//  Copy the information in the bitmap into a buffer in the format specified**********
	int noOfScanLines = GetDIBits(
						  dc,						// handle to DC
						  hImage,					// handle to bitmap
						  0,						// first scan line to set
						  intHeight,				// number of scan lines to copy
						  imgBits,					// array for bitmap bits
						  (BITMAPINFO *)pBMIH,		// bitmap data buffer
						  DIB_RGB_COLORS			// RGB or palette index
	);
	if(noOfScanLines <= 0 ) {
		AfxMessageBox("Error: GetDIBits function failed.");
		return FALSE;
	} else
		return TRUE;
	//--------------------------------------------------------------------------------
	
}
BOOL moldDoc::CallSetDIBits(LPVOID newImgBits) {

	memDC.DeleteDC(); //If the image is loaded on the device context, setDIBits wont work

	
	//Use the modified buffer to alter the bitmap  stored in hImage**********************
	CDC tempHdc;
	tempHdc.CreateCompatibleDC(NULL);//Only used if the last parameter is set to DIB_PAL_COLORS
	
	int noOfScanLines = SetDIBits(
	  tempHdc,              // handle to DC
	  hImage,              // handle to bitmap
	  0,                    // starting scan line
	  intHeight,               // number of scan lines
	  newImgBits, // array of bitmap bits
	  (BITMAPINFO *)pBMIH,	
	  DIB_RGB_COLORS        // type of color indexes to use
	);

	if(noOfScanLines <= 0 ) {
		AfxMessageBox("Error: Setdib function failed.\n");
		return FALSE;
	}	

	flgImgModified = true;
	UpdateAllViews(NULL);	
	return TRUE;
}

void moldDoc::showLoadImage(CCmdUI *item) {

	if(flgBitmapLoaded)
		item->Enable(FALSE);
	else
		item->Enable(TRUE);
}


void moldDoc::initialize() {

	g_lpPreviousRect		= NULL;
	
	intWindowHeight			= DEFLT_WINDOW_HEIGHT;
	intWindowWidth			= DEFLT_WINDOW_WIDTH;
	intMagnification		= DEFLT_IMG_MAGNIFICATION;
	
	markerSize				= DEFLT_MARKER_SIZE;
	markerType				= SETS_COLOR_AS_BACKGROUND;
	
	backgroundColor.red		= DEFLT_BKGND_COLOR_R;
	backgroundColor.green	= DEFLT_BKGND_COLOR_R;
	backgroundColor.blue	= DEFLT_BKGND_COLOR_R;
	
	drawColor.red			= DEFLT_DRAW_COLOR_R;
	drawColor.green			= DEFLT_DRAW_COLOR_R;
	drawColor.blue			= DEFLT_DRAW_COLOR_R;
	
	padColor.red			= DEFLT_PADDING_COLOR_R;
	padColor.green			= DEFLT_PADDING_COLOR_G;
	padColor.blue			= DEFLT_PADDING_COLOR_B;

	modType					= NO_MODIFICATION;
	intTpPadding			= intBtmPadding   =  intLtPadding     = intRtPadding = 0;	
	flgSavedOnce			= flgBitmapLoaded =  flgMoldProcessed = flgImgModified = 
								flgScrollSizeSet = flgNewImage  = false;
	flgPadModified			= true;

	g_blnPadMsgShown = false;
	
	//No image details to display on the left frame
	g_strImageDetails = "";

	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD);
}

void moldDoc::showImageOptions(CCmdUI *item) {
	
	if(item->m_nID == MENU_UNDO ) { 
		if(modType != NO_MODIFICATION && modType < TOP_PADDING_ALTER_UNDONE) 
			item->Enable(TRUE); 
		else
			item->Enable(FALSE);
	} else if(item->m_nID == MENU_REDO ) { 
		if(modType >=  TOP_PADDING_ALTER_UNDONE) 
			item->Enable(TRUE); 
		else
			item->Enable(FALSE);
	} else if (item->m_nID == MENU_MRK_SETS_RGN){
		if(flgBitmapLoaded && markerType != SETS_REGION_AS_BACKGROUND) 
			item->Enable(TRUE);
		else
			item->Enable(FALSE);
	} else if (item->m_nID == MENU_MRK_SETS_COLOR){
		if(flgBitmapLoaded && markerType != SETS_COLOR_AS_BACKGROUND) 
			item->Enable(TRUE);
		else
			item->Enable(FALSE);
	} else if (item->m_nID == MENU_CAMERA_PARAMS_SAVE){
		if(flgSnaps && cameraInfo.blnDataInitialized) 
			item->Enable(TRUE);
		else
			item->Enable(FALSE);
	} else if (item->m_nID == MENU_CAMERA_PARAMS_CHANGE)	{
		if(flgSnaps) 
			item->Enable(TRUE);
		else
			item->Enable(FALSE);
	} else if (item->m_nID == MENU_SAVE_IMG) {
		if(flgImgModified) 
			item->Enable(TRUE);
		else
			item->Enable(FALSE);
	} else {
		if(flgBitmapLoaded) 
			item->Enable(TRUE);
		else
			item->Enable(FALSE);
	}
}

void moldDoc::updtImgWindowSize() {
	intWindowHeight = IMAGE_TOP_INDENT	+ intMagnification*intHeight + IMAGE_BOTTOM_INDENT;
	intWindowWidth  = IMAGE_LEFT_INDENT + intMagnification*intWidth  + IMAGE_RIGHT_INDENT; 
	flgScrollSizeSet = false;
	flgPadModified   = true;	
	UpdateAllViews(NULL);
}
int moldDoc::getImgHeight() { return intHeight;}
int moldDoc::getImgWidth() { return intWidth; }	
CDC* moldDoc::getBitmapDC() {return &memDC;}
CString moldDoc::getFilePath() { return g_strFilePath; }
BOOL moldDoc::isSnaps() { return flgSnaps; }
BOOL moldDoc::isBitmapLoaded() { return flgBitmapLoaded; }
BOOL moldDoc::isDocLoaded() { return flgDocLoaded; }
BOOL moldDoc::isScrollSizeSet() { return flgScrollSizeSet; }
BOOL moldDoc::isFirstLoad() { return flgFirstLoad;}
void moldDoc::resetFirstLoadFlag() { flgFirstLoad = false;}
void moldDoc::setScrollSizeFlag() { flgScrollSizeSet = true; }
BOOL moldDoc::isNewImage() { return flgNewImage; }
void moldDoc::resetNewImageFlag() { flgNewImage = false; }
BOOL moldDoc::isPadModified() { return flgPadModified; }
void moldDoc::resetPadModifiedFlag() { flgPadModified = false; }
	
int moldDoc::getFilesProcessed() { return intFilesProcessed; }
float moldDoc::getInterSlideDistance() { return fltInterSlideDistance; }
int moldDoc::getMagnification() { return intMagnification; }
UINT moldDoc::getWindowWidth() { return intWindowWidth;}
UINT moldDoc::getWindowHeight() { return intWindowHeight;}

//Functions for the padded regions of the image
UINT moldDoc::getTpPadding()  { return intTpPadding;  } 
UINT moldDoc::getBtmPadding() { return intBtmPadding; } 
UINT moldDoc::getLtPadding()  { return intLtPadding;  } 
UINT moldDoc::getRtPadding()  { return intRtPadding;  } 

MARKER_TYPE moldDoc::getMarkerType() {	return markerType; }
RGBCOLOR moldDoc::getBackgroundColor() {	return backgroundColor; }	
RGBCOLOR moldDoc::getDrawColor() { return drawColor; }

void moldDoc::selectBitmap(CDC *pDCMem) { 
	CBitmap* tempBitmap = pDCMem->SelectObject(&m_Bitmap);
}


//Function called by the bitmap view class after setting a particular region to background
void moldDoc::setModStatus_RgnToBk() {	
	//Take back-up of current
	if(!CallGetDIBits(lpvPrevImgBits))
		return;
	modType = RGN_SET_TO_BKGRND;
	flgImgModified = true;
}

void moldDoc::setTpPadding(UINT intVal)  { 
	VERIFY(intVal > 0);	

	//Take backup of current 
	if(!CallGetDIBits(lpvPrevImgBits))
		return;
	intPrevTpPadding = intTpPadding;
	
	intTpPadding  += intVal;
	modType = TOP_PADDING_ALTERED;

	drawPadRectangle(0, 0, intWidth, intTpPadding);	
	flgImgModified = true;
} 

void moldDoc::setBtmPadding(UINT intVal) { 
	VERIFY(intVal > 0);
	
	//Take backup of current 
	if(!CallGetDIBits(lpvPrevImgBits))
		return;
	intPrevBtmPadding = intBtmPadding;
	
	intBtmPadding  += intVal;
	modType = BOTTOM_PADDING_ALTERED;	
	
	drawPadRectangle(0, intHeight - intBtmPadding, intWidth, intHeight);
	flgImgModified = true;
} 

void moldDoc::setLtPadding(UINT intVal)  { 
	VERIFY(intVal > 0);

	//Take backup of current 
	if(!CallGetDIBits(lpvPrevImgBits))
		return;
	intPrevLtPadding = intLtPadding;
	
	intLtPadding  += intVal;
	modType = LEFT_PADDING_ALTERED;
	
	drawPadRectangle(0, 0, intLtPadding, intHeight);
	flgImgModified = true;
} 

void moldDoc::setRtPadding(UINT intVal)  { 
	VERIFY(intVal > 0);

	//Take backup of current 
	if(!CallGetDIBits(lpvPrevImgBits))
		return;
	intPrevRtPadding = intRtPadding;
	
	intRtPadding  += intVal;
	modType = RIGHT_PADDING_ALTERED;

	drawPadRectangle(intWidth - intRtPadding, 0, intWidth, intHeight);
	flgImgModified = true;
} 

void moldDoc::drawPadRectangle(int intLeft, int intTop, int intRight, int intBottom) { 
	
	CRgn   rgn;
	BOOL blnSuccess = rgn.CreateRectRgn( intLeft, intTop, intRight, intBottom);
	ASSERT( blnSuccess == TRUE );

	CBrush mybrush;
	mybrush.CreateSolidBrush(RGB(padColor.red, padColor.green, padColor.blue));
	
	memDC.FillRgn( &rgn, &mybrush);
	mybrush.DeleteObject();
	rgn.DeleteObject();

	flgPadModified = true;		
	flgImgModified = true;
	UpdateAllViews(NULL);	
}


void moldDoc::undo() {
	UINT temp;
	
	switch(modType) {
	
	
	case TOP_PADDING_ALTERED:
		
		//swap padding info
		temp = intTpPadding;
		intTpPadding = intPrevTpPadding;
		intPrevTpPadding = temp;
		
		modType = TOP_PADDING_ALTER_UNDONE;
		flgPadModified = true;	
		break;
	case BOTTOM_PADDING_ALTERED:
		
		//swap padding info
		temp = intBtmPadding;
		intBtmPadding = intPrevBtmPadding;
		intPrevBtmPadding = temp;

		modType = BOTTOM_PADDING_ALTER_UNDONE;
		flgPadModified = true;	
		break;
	case LEFT_PADDING_ALTERED:
		
		//swap padding info
		temp = intLtPadding;
		intLtPadding = intPrevLtPadding;
		intPrevLtPadding = temp;
	
		modType = LEFT_PADDING_ALTER_UNDONE;
		flgPadModified = true;	
		break;
	case RIGHT_PADDING_ALTERED:
		
		//swap padding info
		temp = intRtPadding;
		intRtPadding = intPrevRtPadding;
		intPrevRtPadding = temp;
	
		modType = RIGHT_PADDING_ALTER_UNDONE;
		flgPadModified = true;	
		break;
	case COLORS_SET_TO_BKGRND:
		modType = COLORS_SET_TO_BKGRND_UNDONE;
		break;
	case RGN_SET_TO_BKGRND:
		modType = RGN_SET_TO_BKGRND_UNDONE;
		break;
	}

	//Take backup of current 
	if(!CallGetDIBits(lpvImgBits))
		return;
	
	if(!CallSetDIBits(lpvPrevImgBits))
		return;	
	
	//Store the backed up info again in lpvPrevImgBits
	memcpy(lpvPrevImgBits, lpvImgBits, g_uintTotalImgBytes);

	flgImgModified = true;
}

void moldDoc::redo() {
	UINT temp;
	switch(modType) {
	
	case TOP_PADDING_ALTER_UNDONE:		
		
		//swap padding info
		temp             = intTpPadding;
		intTpPadding     = intPrevTpPadding;
		intPrevTpPadding = temp;

		modType = TOP_PADDING_ALTERED;
		flgPadModified = true;
		break;
	case BOTTOM_PADDING_ALTER_UNDONE:
			
		//swap padding info
		temp              = intBtmPadding;
		intBtmPadding     = intPrevBtmPadding;
		intPrevBtmPadding = temp;

		modType = BOTTOM_PADDING_ALTERED;
		flgPadModified = true;
		break;
	case LEFT_PADDING_ALTER_UNDONE:
		
		//swap padding info
		temp             = intLtPadding;
		intLtPadding     = intPrevLtPadding;
		intPrevLtPadding = temp;

		modType = LEFT_PADDING_ALTERED;
		flgPadModified = true;
		break;
	case RIGHT_PADDING_ALTER_UNDONE:
		
		//swap padding info
		temp             = intRtPadding;
		intRtPadding     = intPrevRtPadding;
		intPrevRtPadding = temp;

		modType = RIGHT_PADDING_ALTERED;
		flgPadModified = true;	
		break;
	case COLORS_SET_TO_BKGRND_UNDONE:
		modType = COLORS_SET_TO_BKGRND;
		break;
	case RGN_SET_TO_BKGRND_UNDONE:
		modType = RGN_SET_TO_BKGRND;
		break;
	}
		
	
	//Take backup of current 
	if(!CallGetDIBits(lpvImgBits))
		return;
	
	if(!CallSetDIBits(lpvPrevImgBits))
		return;	
	
	//Store the backed up info again in lpvPrevImgBits
	memcpy(lpvPrevImgBits, lpvImgBits, g_uintTotalImgBytes);

	flgImgModified = true;

}

void moldDoc::changeMarkerSize() {
	dlgChangeMarkerSize dlg(IDD_DIALOG4, markerSize);
	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MARKER_SIZE);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD_IMAGE);

	if(intRtnVal == IDOK) {
		markerSize = dlg.getMarkerSize();
	}
}

void moldDoc::setMarkerType_Rgn() {
	markerType = SETS_REGION_AS_BACKGROUND;	
}
void moldDoc::setMarkerType_Color() {
	markerType = SETS_COLOR_AS_BACKGROUND;
}


BOOL moldDoc::saveImg() {	
	
	//Check whether the image is loaded
	if(!flgBitmapLoaded) {
		AfxMessageBox("Cannot save. No image is loaded.", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	
	
	if(!flgImgModified) //No need to save if the image is not modified
		return TRUE;

	if(!flgSavedOnce)
		return saveImgNew(true); //We are trying to save the image to the same location but for the first time. Give the option to save to a different locaiton. 
	
	else {
		
		BOOL blnPadMsgDisplayed = false;
	
		if((intTpPadding != 0 || intBtmPadding != 0 || intLtPadding != 0 || intRtPadding != 0)
			&& !g_blnPadMsgShown) {
			if(!blnSuppressPrompts) {
				CString strMsg;
				strMsg = "You have specified padded regions for the image?\n\n";
				strMsg+= "The padding info will not be saved.\n\n";
				strMsg+= "Do you wish to continue?";
				dlgYesNo d(IDD_DIALOG2, strMsg, TRUE);
				if(d.DoModal() != IDOK)
					return FALSE;
			}
			blnPadMsgDisplayed = true;		
		}		
		
		if(!saveAs24BppBitmap(g_strImgPath))
			return FALSE;	

		flgImgModified = false;	
		if(blnPadMsgDisplayed)
			g_blnPadMsgShown = true;
	}	
	return TRUE;
}

void moldDoc::saveImgNewPrompt() {
	saveImgNew(false);
}

BOOL moldDoc::saveImgNew(BOOL blnPromptOverwrite, CString inStrFilePath) {
	
	//Check whether the image is loaded
	if(!flgBitmapLoaded) {
		AfxMessageBox("Cannot save. No image is loaded.", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	
	BOOL blnPadMsgDisplayed = false;
	
	if(intTpPadding != 0 || intBtmPadding != 0 || intLtPadding != 0 || intRtPadding != 0) {
		if(!blnSuppressPrompts) {
			CString strMsg;
			strMsg = "You have specified padded regions for the image?\n\n";
			strMsg+= "The padding info will not be stored.\n\n";
			strMsg+= "Do you wish to continue?";
			dlgYesNo d(IDD_DIALOG2, strMsg, TRUE);
			if(d.DoModal() != IDOK)
				return FALSE;
		}
		blnPadMsgDisplayed = true;
		
	} 
	
	CString strFilePath = "";

	if(blnPromptOverwrite) {  //We are trying to save the image to the same location from where it was loaded and for the first time
		if(!blnSuppressPrompts) {
			//Give the option of overwriting the existing file
			CString str;
			str = "The image will be stored as bitmap of type 24 bits per pixel.\n" ;
			str+=	"Do you wish to overwrite the existing file?\n\n" ;
			str+=	"Click No to assign a different filename.";
			dlgYesNo d(IDD_DIALOG2, str, FALSE);
			if(d.DoModal() == IDOK)
				strFilePath = g_strImgPath;	
			delete d;
		} else 
			strFilePath = g_strImgPath; //Set the file path to default one without prompting	
	}
	
	if(strFilePath == "") {
		if(inStrFilePath != "") 
			strFilePath = inStrFilePath;
		else {
			 
			//Get user input for the filename and directory to store the bitmap file
			CFileDialog fd(FALSE, "bmp","image1", 0, "Bitmap File(*.bmp)|*.bmp||");
			fd.m_ofn.lpstrTitle ="Save as bitmap file :";
			fd.m_ofn.Flags = fd.m_ofn.Flags | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST 
				| OFN_HIDEREADONLY;
				
			if(fd.DoModal() == IDOK) {
				strFilePath = fd.GetPathName();			
				delete fd;
			
			} else {
				delete fd;
				return FALSE;
			}
		}
	}
		
	
	//Add a .bmp extension if it is not already there. This is a bug with the filedialog.
	//If the user enters a registered extension, then the filedialog accepts it and does
	//not add the .bmp extension at the end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CString strExtension = strFilePath.Right(4);
	strExtension.MakeLower();
	if(strExtension != ".bmp") 
		strFilePath = strFilePath + ".bmp";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	
	if(!saveAs24BppBitmap(strFilePath))
		return FALSE;	

	
	g_blnPadMsgShown = blnPadMsgDisplayed;
	flgSavedOnce   = true;
	g_strImgPath = strFilePath;
	flgImgModified = false;	

	
	//Prepare the string used for the display of the left frame
	g_strImageDetails = "\t\n\t\nCurrent Image  " + g_strImgPath;
	UpdateAllViews(NULL); //Update the left frame's view

	return TRUE;
}

BOOL moldDoc::closeImageFile(BOOL blnPromptMessages) {
	
	if(flgBitmapLoaded) {
		
		//Display Prompts~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if(!blnSuppressPrompts && blnPromptMessages) {
			
			//Prompt to save if the image has been modified 
			if(flgImgModified) { 
				dlgYesNo d(IDD_DIALOG2, "Save the modified image before quitting?", TRUE);
				if(d.DoModal() == IDOK) {
					delete d;
					if(!saveImg())       //Either there was some problem with the save operation 
						return FALSE;   //or the save operation was cancelled. We should abort 
								       //the closing of the file in this case
				} else
					delete d;
			}
			
			
			//If the image is not yet applied to the mold, prompt to apply it
			if(!flgMoldProcessed) {
				dlgYesNo d(IDD_DIALOG2, "This image has not been applied on the mold.\nStay on to apply the image to mold?", TRUE);
				if(d.DoModal() == IDOK) {
					delete d;
					return FALSE;
					
				} else 
					delete d;				
			}
		}// End of prompts ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		
		
		delete lpvImgBits; lpvImgBits = NULL;
		delete lpvPrevImgBits; lpvPrevImgBits = NULL;
		
		memDC.DeleteDC();
		m_Bitmap.DeleteObject();
		initialize(); 
		UpdateAllViews(NULL);
		//Unload the image toolbar 
		((myFrame *)AfxGetMainWnd())->ShowControlBar(&imageToolBar, FALSE, FALSE);
		
		return TRUE;
	} else 
		return FALSE; //Image not loaded

	return TRUE;
}

/**********Functions to set the selected colors as background********************************/
BOOL moldDoc::setColorsToBkgrnd(BYTE inBtRed, BYTE inBtGreen, BYTE inBtBlue, 
								COLOR_SELECTION_TYPE inSelectionType, BYTE inBtSatLumVal) {

	//Check whether the image is loaded
	if(!flgBitmapLoaded) {
		AfxMessageBox("Cannot set color. No image is loaded.", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	
	//Store the bits of the image
	if(!CallGetDIBits(lpvImgBits))
		return FALSE;
	
	modType = NO_MODIFICATION;//Set to no_modification till the modification is through

	
	SET_BK_COLOR_INFO setBkColorInfo;
	
	setBkColorInfo.btR =inBtRed;
    setBkColorInfo.btG =inBtGreen;
	setBkColorInfo.btB =inBtBlue;
	setBkColorInfo.selType =inSelectionType;
	setBkColorInfo.btLumSatVal =inBtSatLumVal;
	setBkColorInfo.lpvImgBits =lpvImgBits;
	setBkColorInfo.intWidth =intWidth;
	setBkColorInfo.intHeight =intHeight;
	setBkColorInfo.intExtraBts =extraBytes;
	setBkColorInfo.btBkR =backgroundColor.red;
	setBkColorInfo.btBkG =backgroundColor.green;
	setBkColorInfo.btBkB =backgroundColor.blue;                              	
	
	dlgCallThread dlg(IDD_DIALOG3, SET_BK_COLOR, &setBkColorInfo);
	dlg.DoModal();
	if(dlg.blnSuccess) {
		//Take backup of current 
		if(!CallGetDIBits(lpvPrevImgBits))
			return FALSE;
		
		if(!CallSetDIBits(lpvImgBits))
			return FALSE;
		modType = COLORS_SET_TO_BKGRND;
		flgImgModified = true;	
		return TRUE;
	} else
		return FALSE;
}

/*******************Function to save the bitmap in 24 bpp pixel format**********************/
BOOL moldDoc::saveAs24BppBitmap(CString inStrFilePath) {

	CFile fp;
	CString strMsg = "Error occured while storing the the bitmap to a 24 bpp format.";
	
	//Open the file for writing~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if(!fp.Open(inStrFilePath, CFile::modeCreate | CFile::modeReadWrite 
		| CFile::shareExclusive )) {			
			strMsg+= "\nDetails: Could not open file: \n'" + inStrFilePath + "'\n for writing.";
			AfxMessageBox(strMsg);
			return false;
	}	

	//Identify the sizes
	UINT uintBmpInfoHdrSize = sizeof(BITMAPINFOHEADER);
	UINT uintFileHdrSize = sizeof(BITMAPFILEHEADER);
	UINT uintBitSize = g_uintTotalImgBytes;
	UINT uintFileSize = uintFileHdrSize + uintBmpInfoHdrSize + uintBitSize;
	
	
	//Initialize the file header~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	BITMAPFILEHEADER bmpFileHeader;
	
	bmpFileHeader.bfType = 'MB';
	bmpFileHeader.bfSize = uintFileSize;
	bmpFileHeader.bfReserved1 = 0;
	bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = uintFileHdrSize + uintBmpInfoHdrSize;


	//Initialize the bitmapinfo header~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	BITMAPINFOHEADER bitmapInfoHdr;
	
	bitmapInfoHdr.biSize = uintBmpInfoHdrSize;
	bitmapInfoHdr.biWidth = intWidth;
	bitmapInfoHdr.biHeight = intHeight;
	bitmapInfoHdr.biPlanes = 1;
	bitmapInfoHdr.biBitCount = 24; //We will always store the bmp in 24bpp format
	bitmapInfoHdr.biCompression = BI_RGB;
	bitmapInfoHdr.biSizeImage = 0;
	bitmapInfoHdr.biXPelsPerMeter = 0;
	bitmapInfoHdr.biYPelsPerMeter = 0;
	bitmapInfoHdr.biClrUsed = 0;
	bitmapInfoHdr.biClrImportant = 0;


	
	//Initialize the bits that have to be stored~~~~~~~~~~~~~~~~~~~~~~~~
	if(!CallGetDIBits(lpvImgBits))
		return FALSE; //Get the pixel bits of the loaded bitmap



	//Write all the information to the file~~~~~~~~~~~~~~
	try {

		fp.Write(&bmpFileHeader, uintFileHdrSize);

		fp.Write(&bitmapInfoHdr, uintBmpInfoHdrSize);

		fp.Write(lpvImgBits, uintBitSize);



	} catch(CException *e) {
		e->Delete();
		CString strMsg1;
		e->GetErrorMessage((char *) &strMsg1, 400, NULL);
		strMsg+= "\nDetails: " + strMsg1;
		AfxMessageBox(strMsg);
		return false;
	}

	if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
		fp.Close();
	return TRUE;
}
	
/************************Function to set the camera parameters******************************/
void moldDoc::setCameraParamsOnly() {
	setCameraParams(true);
}

BOOL moldDoc::setCameraParams(BOOL inBlnSetParamsOnly) {
	dlgGetCameraInfo dlg(IDD_DIALOG7, cameraInfo, inBlnSetParamsOnly);

	 //Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_DEFINE_CAM);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD_IMAGE);

	if(intRtnVal == IDOK)  	{//Camera parameters altered
		cameraInfo = dlg.outCameraInfo;
		return true;
	} else 
		return false;
}

void moldDoc::saveCameraParams() {

	//Get user input for the filename and directory to store the mold file	
	CFileDialog fd(FALSE, "cam","Camera1", 0, "Camera Params(*.cam)|*.cam||");
	fd.m_ofn.lpstrTitle ="Save camera params as:";
	fd.m_ofn.Flags = fd.m_ofn.Flags | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST
			| OFN_HIDEREADONLY;
	
	CString filePath;
	CFile fp;

	if(fd.DoModal() == IDOK) {
		filePath = fd.GetPathName();			
		delete fd;
	
	} else {
		delete fd;
		return;
	}

	//Add a .cam extension if it is not already there. This is a bug with the filedialog.
	//If the user enters a registered extension, then the filedialog accepts it and does
	//not add the .bmp extension at the end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CString strExtension = filePath.Right(4);
	strExtension.MakeLower();
	if(strExtension != ".cam") 
		filePath = filePath + ".cam";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	
	if(!fp.Open(filePath, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive )) {
			AfxMessageBox("Error: Could not open file: \n'" + filePath + "'\n for writing.");
			return ;
	}
	
	fp.SeekToBegin();
	
	BYTE btCameraAtInfinity, btWtSameAsHt;
	
	cameraInfo.blnCameraAtInfinity? btCameraAtInfinity = 1 : btCameraAtInfinity = 0;
	
	cameraInfo.blnWtSameAsHt? btWtSameAsHt = 1 : btWtSameAsHt = 0;

	try {
		fp.Write(&btCameraAtInfinity, 1);
		fp.Write(&btWtSameAsHt, 1);
		
		fp.Write(&cameraInfo.fltInfinityHt, 4);
		fp.Write(&cameraInfo.fltInfinityWt, 4);
		fp.Write(&cameraInfo.fltHtDistance1, 4);
		fp.Write(&cameraInfo.fltHtDistance2, 4);
		fp.Write(&cameraInfo.fltHeight1, 4);
		fp.Write(&cameraInfo.fltHeight2, 4);
		fp.Write(&cameraInfo.fltWtDistance1, 4);
		fp.Write(&cameraInfo.fltWtDistance2, 4);
		fp.Write(&cameraInfo.fltWidth1, 4);
		fp.Write(&cameraInfo.fltWidth2, 4);

	} catch(CException *e) {
		e->Delete();
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();
		AfxMessageBox("Error: Could not write to file", MB_OK | MB_ICONSTOP);
		return;
	}
	
	AfxMessageBox("Camera Parameters successfully saved to the file");
	if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
		fp.Close();

}

BOOL moldDoc::setCameraOrntn() {
	camOrietationDlg dlg(IDD_DIALOG8, camOrientation, cuboidParams, cameraInfo);

	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_ORIENTATION);
	int intRtnVal = dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD_IMAGE);

	if(intRtnVal == IDOK) {
		camOrientation = dlg.camOrientation;
		return true;
	}
		return false;

}

/////////////////////////////////////////////applyToMold//////////////////////////////////////
//After all the image parameters is set, this function is called to process the mold using
//the image
BOOL moldDoc::applyToMold(UINT inUintNewXPts, UINT inUintNewYPts) {
	
	//Check whether the image is loaded
	if(!flgBitmapLoaded) {
		AfxMessageBox("Cannot apply image. No image is loaded.", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	
	
	int intActualImgWidth, intActualImgHeight;
	intActualImgWidth = intWidth - intLtPadding - intRtPadding;
	intActualImgHeight = intHeight -intTpPadding - intBtmPadding;
	
	//Check for a valid region to process
	if(intActualImgWidth <= 0 || intActualImgHeight <= 0) {
		AfxMessageBox("No valid image region to process.");
		return FALSE;
	}

	if(flgSnaps && !blnSuppressPrompts) {
		
		if(!setCameraParams(false))
			return FALSE;
		if(!setCameraOrntn())
			return FALSE;		
	}
	
	PROCESS_MLD_INFO objProcessMld;

	objProcessMld.g_strInputFile = g_strFilePath;
	
	//Set values for the camera ratio=========================================================
	if(flgSnaps) {
		if(cameraInfo.blnCameraAtInfinity) {
			objProcessMld.cameraRatio.xRangeAtInfinity = cameraInfo.fltInfinityWt;
			objProcessMld.cameraRatio.yRangeAtInfinity = cameraInfo.fltInfinityHt;
		} else {
			//Calculate the y Ratio
			
			float x; //x here stands for an extra variable whose value has to be computed before
			//we get the value of the y ratio
			
			if(cameraInfo.fltHeight1 == cameraInfo.fltHeight2)
				x = 0.0f;
			else
				x = (cameraInfo.fltHeight2 * cameraInfo.fltHtDistance1 - 
					cameraInfo.fltHtDistance2 * cameraInfo.fltHeight1)/ 
					(cameraInfo.fltHeight1 - cameraInfo.fltHeight2);
				
			objProcessMld.cameraRatio.yRatio 
				= cameraInfo.fltHeight1/(x + cameraInfo.fltHtDistance1);

			//Calculate the x Ratio
			if(cameraInfo.blnWtSameAsHt)
				objProcessMld.cameraRatio.xRatio = objProcessMld.cameraRatio.yRatio;
			else {
				if(cameraInfo.fltWidth1 == cameraInfo.fltWidth2)
					x = 0.0f;
				else
					x = (cameraInfo.fltWidth2 * cameraInfo.fltWtDistance1 - 
						cameraInfo.fltWtDistance2 * cameraInfo.fltWidth1)/ 
						(cameraInfo.fltWidth1 - cameraInfo.fltWidth2);
					
				objProcessMld.cameraRatio.xRatio
					= cameraInfo.fltWidth1/(x + cameraInfo.fltWtDistance1);
			}
		}
	}//=======================================================================================

	objProcessMld.imgParams.leftPadding = intLtPadding;
	objProcessMld.imgParams.bottomPadding = intBtmPadding;
	objProcessMld.imgParams.rightPadding = intRtPadding;
	objProcessMld.imgParams.topPadding =intTpPadding;
	objProcessMld.imgParams.extraBytes = extraBytes;

	if(flgSnaps) {
		objProcessMld.imgParams.cameraLocation = camOrientation.camPos;
		objProcessMld.imgParams.lookingAt = camOrientation.lookingAt;		
		objProcessMld.imgParams.flgAtInfinity = cameraInfo.blnCameraAtInfinity;
	}
	
	objProcessMld.imgParams.height = intHeight;
	objProcessMld.imgParams.width = intWidth;

	objProcessMld.imgParams.InvalidColor.green = backgroundColor.green;
	objProcessMld.imgParams.InvalidColor.red = backgroundColor.red;
	objProcessMld.imgParams.InvalidColor.blue  = backgroundColor.blue;

	//Give the option of modifying the mold x and y points in accordance with the first slide
	if(!flgSnaps && intFilesProcessed == 0) {
		if(!blnSuppressPrompts) {
		
			alterPointsDlg dlg(IDD_DIALOG16, cuboidParams.uintXPoints, 
				cuboidParams.uintYPoints, intActualImgWidth, intActualImgHeight);

			//Set the value for displaying the help info
			((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MODIFY_MOLD_PTS);
			int intRtnVal = dlg.DoModal();
			((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD_IMAGE);

			delete dlg;		

			if(intRtnVal == IDOK) {
				dlg.getNewPoints(objProcessMld.uintNewXPts, objProcessMld.uintNewYPts);					
			} else {
				return FALSE;
			}
		} else { //Use values supplied to the function. (If no values are passed then zeroes would be assigned)
			objProcessMld.uintNewXPts = inUintNewXPts;
			objProcessMld.uintNewYPts = inUintNewYPts;
		}
	}		

	if(!CallGetDIBits(lpvImgBits))
		return FALSE;//Get the pixel bits of the loaded bitmap

	objProcessMld.imgPointer = (BYTE *)lpvImgBits;
	
	//Set the security string that would be used by the component to validate the calling component
	if(flgSnaps)
		objProcessMld.setSecurity(SECURITY_STRING_IMGTOOBJ);
	else
		objProcessMld.setSecurity(SECURITY_STRING_SLIDESTOOBJ);
	
	
	dlgCallThread dlg(IDD_DIALOG3, flgSnaps? PROCESS_MOLD_SNAPS: PROCESS_MOLD_SLIDES,
		&objProcessMld);
	dlg.DoModal();
	if(dlg.blnSuccess) {
		intFilesProcessed++; //One more image has been processed
		flgMoldProcessed = true; //Mold has been processed using this image		
		
		if(objProcessMld.uintNewXPts != 0) { //cuboid points were modified
			//Update the status of cuboid X and Y points
			cuboidParams.uintXPoints = objProcessMld.uintNewXPts;
			cuboidParams.uintYPoints = objProcessMld.uintNewYPts;
		}

		if(!flgSnaps) //Update the status of cuboid zPoints
			cuboidParams.uintZPoints = intFilesProcessed;

		CString str;
		if(flgSnaps)
			str = "Mold successfully processed for this image.";
		else 
			str = "The slide has been successfully added to the mold file.";

		str+= "\nThe image file will be closed now.";

		if(!blnSuppressPrompts)
			AfxMessageBox(str);
		
		if(!blnSuppressPrompts) //Don't close the file if we are running a script
			closeImageFile(TRUE);

	}  else if(dlg.isCancelled()) {
		
		if(flgSnaps) {
		
			if(!blnSuppressPrompts) {
				//Take a confirmation before deleting the file
				dlgYesNo *dlg = new dlgYesNo(IDD_DIALOG2, "As the operation was cancelled in between, the mold has been corrupted.\nUsing it again would give unpredictable results.\nDelete the mold file(Recommended)?", true);
				if(dlg->DoModal() == IDOK) {
					delete dlg;
					CFile fp;
					fp.Remove(objProcessMld.g_strInputFile);
					saveImg();
					closeImageFile(FALSE);
					((CWinApp* )AfxGetApp())->CloseAllDocuments(false);
				} else 
					delete dlg;
			}
			
		} else if(objProcessMld.uintNewXPts != 0) { //cuboid points were modified
		
			//Update the status of cuboid X and Y points
			cuboidParams.uintXPoints = objProcessMld.uintNewXPts;
			cuboidParams.uintYPoints = objProcessMld.uintNewYPts;
		}
		
		return FALSE;
		
	} else {
		AfxMessageBox(getErrDescription(dlg.getExitCode()), MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	UpdateAllViews(NULL); 
	return TRUE;
		
}		

/////////////////////////////////////////////createModel//////////////////////////////////////
//After all the images are processed, this function is used to create the actual model using 
//the carved out mold.
BOOL moldDoc::createModel(SCR_CREATEMODELPART_PARAMS *ptrParams) {
	
	//If any images are currently loaded then prompt the user to process or close it----
	if(flgBitmapLoaded) {
		AfxMessageBox("Please process and close all images before creating the model.");
		return FALSE;
	}//---------------------------------------------------------------------------------

	
	//Check whether there are sufficient points to create the model
	if(flgSnaps) {
		//If no images are processed, confirm whether only outline of mold should be created--		
		if(intFilesProcessed == 0 && !blnSuppressPrompts) {
			dlgYesNo d(IDD_DIALOG2, "The mold has not been processed by any image.\n\nA model created from this mold would look like the same cuboid as the mold.\n\n Do you wish to continue?", FALSE);
				if(d.DoModal() != IDOK)	
					return FALSE;
		}//-----------------------------------------------------------------------------------
	} else {
		if(intFilesProcessed < 2) {
			AfxMessageBox("At least two slides should be processed before creating the model.");
			return FALSE;
		}
	}

	
	CREATE_MODEL_INFO objCreateModel; //Object to pass information to model creating dialog

	//Pass the path of the mold 
	objCreateModel.g_strInputFile = g_strFilePath;

	if(!blnSuppressPrompts) { //Do not get inputs from the user if we are calling this fn from a script
	
	
		//Get user input for the range of mold points that should be processed--
		pointRangeDlg dlg1(IDD_DIALOG10, cuboidParams.uintXPoints, cuboidParams.uintYPoints, 
			flgSnaps? cuboidParams.uintZPoints : intFilesProcessed);
		
		//Set the value for displaying the help info
		((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_POINT_RANGE);
		int intRtnVal = dlg1.DoModal();
		((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD);

		if(intRtnVal != IDOK) {
			delete dlg1;
			return FALSE;
		}

		dlg1.getPointRange(objCreateModel.intMinx, objCreateModel.intMaxx, 
			objCreateModel.intMiny, objCreateModel.intMaxy,
			objCreateModel.intMinz, objCreateModel.intMaxz);
		delete dlg1;//----------------------------------------------------------

		
		//Get user input for the color of the model-----
		dlgModelColor dlg2(IDD_DIALOG11, modelColor);

		
		//Set the value for displaying the help info
		((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_CREATE_MODEL);
		intRtnVal = dlg2.DoModal();
		((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD);

		if(intRtnVal != IDOK) {
			delete dlg2;
			return FALSE;
		}
		
		dlg2.getColorValues(modelColor);
		objCreateModel.modelClr.red = modelColor.red;
		objCreateModel.modelClr.green = modelColor.green;
		objCreateModel.modelClr.blue = modelColor.blue;
		delete dlg2;//-----------------------------------


		//Get user input for the filename and directory to store the model file----------	
		CFileDialog fd(FALSE, "mdl","model1", 0, "Model Files(*.mdl)|*.mdl||");
		fd.m_ofn.lpstrTitle ="Save the model file as:";
		fd.m_ofn.Flags = fd.m_ofn.Flags | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST
			| OFN_HIDEREADONLY;
			
		if(fd.DoModal() == IDOK) {
			objCreateModel.g_strOutputFile = fd.GetPathName();			
			delete fd;
		
		} else {
			delete fd;
			return FALSE;
		}//------------------------------------------------------------------------------


		//Add a .mdl extension if it is not already there. This is a bug with the filedialog.
		//If the user enters a registered extension, then the filedialog accepts it and does
		//not add the .bmp extension at the end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		CString strExtension = objCreateModel.g_strOutputFile.Right(4);
		strExtension.MakeLower();
		if(strExtension != ".mdl") 
			objCreateModel.g_strOutputFile = objCreateModel.g_strOutputFile + ".mdl";
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	


	} else {
		
		//Populate using values passed by the script
		 objCreateModel.g_strOutputFile = ptrParams->strFilePath;
		 objCreateModel.intMinx = ptrParams->intMinXIndex;
		 objCreateModel.intMaxx = ptrParams->intMaxXIndex;
		 objCreateModel.intMiny = ptrParams->intMinYIndex;
		 objCreateModel.intMaxy = ptrParams->intMaxYIndex;
		 objCreateModel.intMinz = ptrParams->intMinZIndex;
		 objCreateModel.intMaxz = ptrParams->intMaxZIndex;
		 objCreateModel.modelClr.red = ptrParams->ucharModelClrR;
		 objCreateModel.modelClr.green = ptrParams->ucharModelClrG;
		 objCreateModel.modelClr.blue = ptrParams->ucharModelClrB;
	}
	
	//Set the security string that would be used by the component to validate the calling component
	objCreateModel.setSecurity(SECURITY_STRING_PTSTOPOLYGONS);
	
	//Use a dialog frame with cancel option and a worker thread to create model--	
	dlgCallThread dlg(IDD_DIALOG3, CREATE_MODEL, &objCreateModel);

	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_CREATE_MODEL);
	dlg.DoModal();
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD);
	
	if(dlg.blnSuccess) {
		delete dlg;
		
		if(!blnSuppressPrompts) { //Give option to delete the mold file and to load the model file
		
			BOOL blnMoldDeleted = false;	
			
			CString str;
			
			//Prompt the user to delete the mold file. 
			str = "Delete the mold file that was used to create the model?\n\n" ; 
			str+=  "Click on No to keep it for further processing.\n\n"; 	
			dlgYesNo d1(IDD_DIALOG2, str, TRUE);
			if(d1.DoModal() == IDOK) {
				CFile fp;
				try { 
					fp.Remove(objCreateModel.g_strInputFile);
					blnMoldDeleted = true;
				} catch (CException *e) {
					e->Delete();
					AfxMessageBox("Error occured while deleting the mold file.", MB_OK | MB_ICONSTOP);
					return FALSE;
				}
				
			} 
				
			delete d1;	
			
			//Ask the user whether he wants to view the created model		
			str = "Model created successfully.\n\nLocation : " + objCreateModel.g_strOutputFile +
				"\nWould you like to view the model now?";
			dlgYesNo d(IDD_DIALOG2, str, TRUE);
			if(d.DoModal() != IDOK) {
				delete d;
				if(blnMoldDeleted)  //Mold file does not exist
					//Since all images are already closed, there wont be any problem in closing the document
					((CWinApp* )AfxGetApp())->CloseAllDocuments(false);
				return FALSE;	
			}  else 
				delete d;

			
			//Close the mold file and load the model file
			
			((CWinApp* )AfxGetApp())->CloseAllDocuments(false);
			
			((CWinApp* )AfxGetApp())->OpenDocumentFile(objCreateModel.g_strOutputFile); 

		}

	} else if(dlg.isCancelled()) {
		delete dlg;
		return FALSE;
	}
	else {
		delete dlg;
		AfxMessageBox(getErrDescription(dlg.getExitCode()), MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	//---------------------------------------------------------------------------
	return TRUE;
}

//Function called by the framework when the document is being unloaded.
BOOL moldDoc::SaveModified() {
	
	if(flgBitmapLoaded) //If image is loaded, try to close the image first
		return closeImageFile(TRUE); //TRUE means display the prompts
	else
		return TRUE;
}

void moldDoc::OnCloseDocument() {

	CDocument::OnCloseDocument();
	
	//We need to show the main toolbar if all the documents are being closed
	((myFrame *)AfxGetMainWnd())->showHideToolBar(TRUE);


	//Set the Mainframe's flag window indicating that no document is loaded as of now
	((myFrame *)AfxGetMainWnd())->setDocLoadedFlag(FALSE);

	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_APP);


}

//Function called by the left frame to display the mold details
CString moldDoc::getMoldDetails() {
	CString strDisplay;
	strDisplay.Format(g_strMoldDetails, 
		intFilesProcessed, 
		cuboidParams.uintXPoints,
		cuboidParams.uintYPoints,
		cuboidParams.uintZPoints
		);

	return strDisplay +  g_strImageDetails;
}

LPRECT moldDoc::getLpPreviousRect() {
	return g_lpPreviousRect;

}

void moldDoc::setLpPreviousRect(CRect inRect) {
	if(g_lpPreviousRect != NULL) 
		delete g_lpPreviousRect;
	g_lpPreviousRect = new CRect(inRect.left, inRect.top, inRect.right, inRect.bottom);

}
void moldDoc::setLpPreviousRectToNull() {
	if(g_lpPreviousRect != NULL) 
		delete g_lpPreviousRect;
	g_lpPreviousRect = NULL;	
}