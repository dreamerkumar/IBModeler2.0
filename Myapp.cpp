#include<afxwin.h>

#include "molddoc.h"
#include "modeldoc.h"

#include "moldview.h"
#include "modelview.h"
#include "bitmapview.h"
#include "htmlhelp.h"


#include "mychildwnd.h"
#include "modelwnd.h"

#include "myframe.h"

#include "resource.h"

#include "dialogs.h"

#include "myapp.h"



BEGIN_MESSAGE_MAP(myApp, CWinApp)
	ON_COMMAND(ID_FILE_NEW, createNewMold)
	ON_COMMAND(ID_FILE_OPEN, openFile)
	ON_COMMAND(MENU_EXIT, OnClose)
	ON_COMMAND(MENU_HELP_CONTENTS, showHelp)
	ON_COMMAND(ID_HELP, showHelp)
	ON_COMMAND(MENU_HELP_ABOUT, showAboutDlg)	
END_MESSAGE_MAP()


myApp a;


void myApp::showHelp() {
	
	CString strHelpFile;
	
	switch(g_curWindow) {
	case CURRENT_WINDOW_APP:
		strHelpFile = "::/mainsteps.html";
		break;
	case CURRENT_WINDOW_MOLD:
		strHelpFile = "::/applyingimages.html";
		break;
	case CURRENT_WINDOW_MOLD_IMAGE:
		strHelpFile = "::/identifymodel.html";
		break;
	case CURRENT_WINDOW_MODEL:
		strHelpFile = "::/viewmodeloptions.html";
		break;
	case CURRENT_WINDOW_CREATE_MOLD:
		strHelpFile = "::/dlghelps/createmolddlg.html";
		break;	
	case CURRENT_WINDOW_POINT_RANGE:
		strHelpFile = "::/dlghelps/specifymodelrangedlg.html";
		break;		
	case CURRENT_WINDOW_BACK_COLOR:
		strHelpFile = "::/dlghelps/backcolordlg.html";
		break;
	case CURRENT_WINDOW_DEFINE_CAM:
		strHelpFile = "::/dlghelps/definecam.html";
		break;
	case CURRENT_WINDOW_LEGENDS:
		strHelpFile = "::/dlghelps/legends.html";
		break;
	case CURRENT_WINDOW_LOCATION:
		strHelpFile = "::/dlghelps/locationdlg.html";
		break;
	case CURRENT_WINDOW_LOOKING_AT:
		strHelpFile = "::/dlghelps/lookingatdlg.html";
		break;
	case CURRENT_WINDOW_MARKER_SIZE:
		strHelpFile = "::/dlghelps/markersizedlg.html";
		break;
	case CURRENT_WINDOW_MODEL_VIEW_DISTANCE:
		strHelpFile = "::/dlghelps/modelviewdistancedlg.html";
		break;
	case CURRENT_WINDOW_MODIFY_MOLD_PTS:
		strHelpFile = "::/dlghelps/modifymoldptsdlg.html";
		break;
	case CURRENT_WINDOW_ORIENTATION:
		strHelpFile = "::/dlghelps/orientationdlg.html";
		break;
	case CURRENT_WINDOW_RTN_SPEED:
		strHelpFile = "::/dlghelps/rtnspeeddlg.html";
		break;
	case CURRENT_WINDOW_SET_BACK_SCREEN:
		strHelpFile = "::/dlghelps/backscreendlg.html";
		break;
/*	
	case CURRENT_WINDOW_:
		strHelpFile = "::/";
		break;
*/	
	case CURRENT_WINDOW_CREATE_MODEL:
		strHelpFile = "::/creatingmodel.html";
		break;

	default:
		strHelpFile = "::/index.html";
		break;

	}	
	
	
	if(HtmlHelp(AfxGetMainWnd()->GetSafeHwnd(), getHelpFilePath() + strHelpFile,
			HH_DISPLAY_TOPIC, NULL) == NULL)
		AfxMessageBox("Error: Help could not be loaded", MB_OK|MB_ICONSTOP); 
	
	/*
	if(HtmlHelp(AfxGetMainWnd()->GetSafeHwnd(),"G:\\projects\\development\\IBModeler\\Debug\\IBModeler.chm" + strHelpFile,
			HH_DISPLAY_TOPIC, NULL) == NULL)
		AfxMessageBox("Error: Help could not be loaded", MB_OK|MB_ICONSTOP); 
	*/
}

void myApp::showAboutDlg() {
	dlgAbout  * myDlgAbout = new dlgAbout(IDD_DIALOG19);
	myDlgAbout->DoModal();
	delete myDlgAbout;
	
}


void myApp::OnClose() {
	myFrame *m = (myFrame *)AfxGetMainWnd();
	m->OnClose();
}


BOOL myApp::InitInstance() {

	//Display the splash screen----------
	dlgSplash myDlgSplash(IDD_DIALOG18);
	myDlgSplash.DoModal();
	delete myDlgSplash;
	//-----------------------------------

	//Standard initialization (Commented the below line on 13-Oct-04 as the pop ups were giving a real bad display in XP)
	//SetDialogBkColor(RGB(213, 204, 187)); //Sets desert color for all the dialog pop ups
	
	//SetRegistryKey("IBModeler"); 
	
	//LoadStdProfileSettings();
	/*
	Call this member function from within the InitInstance member function to enable and 
	load the list of most recently used (MRU) files and last preview state. If nMaxMRU is 0, 
	no MRU list will be maintained.
	*/

	CMultiDocTemplate *t;


	//first document type
	d  = RUNTIME_CLASS(modelDoc);
	cf = RUNTIME_CLASS(modelWnd);
	v  = RUNTIME_CLASS(modelView);

	t = new CMultiDocTemplate (IDR_CHILDFRAME2, d, cf, v);
	AddDocTemplate(t);
	
	//second document type
	d  = RUNTIME_CLASS(moldDoc);
	cf = RUNTIME_CLASS(myChildWnd);
	v  = RUNTIME_CLASS(bitmapView);

	
	t = new CMultiDocTemplate (IDR_CHILDFRAME3, d, cf, v);
	AddDocTemplate(t);

	myFrame *f = new myFrame();
	
	f->myLoadFrame(IDR_MAINFRAME);
	m_pMainWnd = f;

	
	//Register supported file types
	enableFileTypeSupport() ;

		//Determine the filepath for the help file---------------------------------------
	if(!readRegKey(HKEY_LOCAL_MACHINE, PRODUCT_DIR_REGKEY, g_strHelpFilePath))
		AfxMessageBox(g_strHelpFilePath + "Error: Could not read help document's path from the registry.\nHelp support will not be available through the application.", MB_OK | MB_ICONSTOP, NULL);
	else
		//The application file path is returned above. Modify it to point to the help file.
		g_strHelpFilePath.Replace(".exe", ".chm");

	
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//We won't allow the loading of a new document when the frame initially loads.
	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	if(!ProcessShellCommand(cmdInfo))
			return FALSE;
	
	//Accept dragged files. However, in the corresponding doc frames, we are 
	//making a check to ensure that more than one doc is not loaded
	m_pMainWnd->DragAcceptFiles(); 
	

	f->ShowWindow(SW_SHOWMAXIMIZED);

	//AfxGetApp()->OpenDocumentFile("HI");
	//CWinApp::OnFileNew();
	
	//Set the enum value for help display
	g_curWindow = CURRENT_WINDOW_APP;	
	
	

	return TRUE;
}


void myApp::enableFileTypeSupport() {
	
	BOOL blnModelTypeEntryDone = false;
	BOOL blnMoldTypeEntryDone = false;

	BOOL blnModelSupport = false;
	BOOL blnMoldSupport = false;

	CString strValue;
	
	//Check for the entry for the model file extension
	BOOL blnModelEntryDone = readRegKey(HKEY_CLASSES_ROOT, ".mdl", strValue);

	if(blnModelEntryDone) {
		if(strValue != "Model.File") //Some other application supports the model file type
			blnModelSupport = true;
	}

	//Check for the entry for the mold file extension
	BOOL blnMoldEntryDone = readRegKey(HKEY_CLASSES_ROOT, ".mld", strValue);

	if(blnMoldEntryDone) {
		if(strValue != "Mold.File")  //Some other application supports the mold file type
			blnMoldSupport = true;		
	}
	
	if(blnModelSupport || blnMoldSupport) {
		//Give a proper message

		CString strMessage = "The following file types are registered with some other application:\n\n";
		if(blnModelSupport) 
			strMessage += "Model file (.mdl extension)\n\n";
		
		if(blnMoldSupport) 
			strMessage += "Mold file (.mld extension)\n\n";

		
		strMessage += "To make IBModeler the default application for the above file types:\n " ;
		strMessage += "I. Unregister the existing support and \nII. Run this application again.\n\n";
		strMessage += "To unregister the the file types:\n1. Open 'My Computer'\n ";
		strMessage += "2. Select the 'Folder Options' menu item\n";
		strMessage += "3. Look in the 'File Types' tab\n4. Delete the above file extensions from the list.";
		AfxMessageBox(strMessage, MB_OK | MB_ICONINFORMATION, NULL);
	}



	if(blnModelEntryDone && blnMoldEntryDone) { //Check for other entries
		
		blnMoldTypeEntryDone = readRegKey(HKEY_CLASSES_ROOT, "Mold.File", strValue);
		if(blnMoldTypeEntryDone)//Check for model type entry
			blnModelTypeEntryDone = readRegKey(HKEY_CLASSES_ROOT, "Model.File", strValue);			
	}

	if((!blnModelEntryDone) || (!blnMoldEntryDone) || (!blnModelTypeEntryDone) || (!blnMoldTypeEntryDone)) {
		
		EnableShellOpen(); // enable file manager drag/drop and DDE Execute open
						/*
						Call this function, typically from your InitInstance override, to enable 
						your application’s users to open data files when they double-click the 
						files from within the Windows File Manager. Call the 
						RegisterShellFileTypes member function in conjunction with this function, 
						or provide a .REG file with your application for manual registration of 
						document types
						*/


		
		//Make suitable registry entries to register document types---------------------------------------	
		//Note: This can also be done by shipping a .reg file along with the installation files

		RegisterShellFileTypes(FALSE); 
						/*
						TRUE adds registration entries for shell commands Print and Print To, 
						allowing a user to print files directly from the shell, or by dragging
						the file to a printer object. It also adds a DefaultIcon key. By default,
						this parameter is FALSE for backward compatibility
						*/

		/*
		RegisterShellFileTypes also makes entry for the ddeexec key to enable DDE support. 
		Due to this, whenever the user clicks on a supported file type (mld or mdl) an error
		message was being shown by the Windows Manager whenever a user tries to open the file
		by double clicking on it. Deleting the ddeexec keys for the two document types removes this bug.
		*/
		
		BOOL blnDeleted = true; //Flag used as a check before displaying the error message
		
		if(RegDeleteKey( HKEY_CLASSES_ROOT, "Mold.File\\shell\\open\\ddeexec") != 0 ) 
			blnDeleted = false;
		else if(RegDeleteKey( HKEY_CLASSES_ROOT, "Model.File\\shell\\open\\ddeexec") != 0 )
			blnDeleted = false;
		
		if(!blnDeleted) {
			//Prepare the error message
			CString strMessage;
			strMessage = "Error occured while registering the file types for this application.\n\n";
			strMessage += "If the file types are not registered, then you may not be able to open the application by double clicking on a supported file type.\n\n";
			strMessage += "Possible cause of Error: It looks like the user does not have permission to modify the windows registry.\n\n";
			strMessage += "Solution: To register the file types, the application needs to be run once by the administrator, or any other user who has the rights to modify the registry.";
			//Display the error message
			AfxMessageBox(strMessage, MB_OK | MB_ICONSTOP, NULL);
		}		
		
		//------------------------------------------------------------------------------------------------
	}
}

//Function to read a value from the registry
BOOL myApp::readRegKey(HKEY inHkParentKey, CString inStrSubKey, CString &outStrValue) {
	
	//Open a registry key for reading~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	HKEY hkResult;
	if(RegOpenKeyEx(
	  inHkParentKey,         // handle to open key
	  inStrSubKey,  // subkey name
	  0,   // reserved
	  KEY_READ, // security access mask
	  &hkResult    // handle to open key
	  ) != ERROR_SUCCESS) {
		return FALSE;
	}
	

	//Read the value from the registry key~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	unsigned char *ptrData;
	ptrData = (unsigned char *)malloc(10000); //10 KB is a sufficiently long string to hold the value
	
	DWORD dwType;
	DWORD dwCount = 10000; //Should be the same as the above allocated space else it won't work on windows 98

	if(RegQueryValueEx(hkResult, 
		NULL,NULL, &dwType, (LPBYTE)ptrData, &dwCount)
		!= ERROR_SUCCESS) {
		
		//Release the memory used for reading the registry value
		free(ptrData);
		return FALSE;	
	}
		
	//Store the value in the string
	outStrValue.Format("%s", ptrData);
	
	//Release the memory used for reading the registry value
	free(ptrData);

	//Close the registry key~~~~~~~~~~~~~~~~~~~~
	if(RegCloseKey(hkResult) != ERROR_SUCCESS) {
		return FALSE;
	}
	

	return TRUE;
}

//-----------------------------------createNewMold-------------------------------------------
//Instead of giving a call to the base class OnFileNew() which asks a user to select from the 
//available file types and then create a new file by giving a call to OpenDocumentFile(NULL)
//we will call to this function on selection of new in the menu. ----------------------------
void myApp::createNewMold() {
		
	CFile fp;
	BYTE option; //Whether snaps or slides option is chosen
	CString filePath;	
	BOOL flgUsingSnaps;
	UINT totalBits, totalBytes, rem;
	UINT ptDensityX, ptDensityY, ptDensityZ;//Points along the three axis
	float minx, maxx, miny, maxy, minz, maxz;//Ranges for the mold

	int filesProcessed = 0;	
		
	//Get user input for the dimensions of the mold
	dlgCreateMold *dlg= new dlgCreateMold(IDD_DIALOG1);
	
	//Set the value for displaying the help info
	g_curWindow = CURRENT_WINDOW_CREATE_MOLD;	
	dlg->DoModal();
	//Set the value for displaying the help info
	g_curWindow = CURRENT_WINDOW_APP;	
	
	if(dlg->flgOKBtn) {
		minx = dlg->minx;
		miny = dlg->miny;		
		
		maxx = dlg->maxx;
		maxy = dlg->maxy;
		
		minz = dlg->minz;
		maxz = dlg->maxz;

		ptDensityX = dlg->ptDensityX;
		ptDensityY = dlg->ptDensityY;
		

		flgUsingSnaps = dlg->flgUsingSnaps;
		if(flgUsingSnaps) { 
			ptDensityZ = dlg->ptDensityZ;
			option = PROCESS_USING_SNAPS;
		} else {
			ptDensityZ = 0;
			option = PROCESS_USING_SLIDES;
		}
		
		delete dlg;
	} else {
		delete dlg;
		return;
	}

	//Get user input for the filename and directory to store the mold file	
	CFileDialog fd(FALSE, "mld","mold1", 0, "Mold Files(*.mld)|*.mld||");
	fd.m_ofn.lpstrTitle ="Save new mold file as:";
	fd.m_ofn.Flags = fd.m_ofn.Flags | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST
		| OFN_HIDEREADONLY;
		
	if(fd.DoModal() == IDOK) {
		filePath = fd.GetPathName();			
		delete fd;
	
	} else {
		delete fd;
		return;
	}

	
	//Add a .mld extension if it is not already there. This is a bug with the filedialog.
	//If the user enters a registered extension, then the filedialog accepts it and does
	//not add the .bmp extension at the end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	CString strExtension = filePath.Right(4);
	strExtension.MakeLower();
	if(strExtension != ".mld") 
		filePath = filePath + ".mld";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	if(!fp.Open(filePath, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive )) {
			AfxMessageBox("Error: Could not open file: \n'" + filePath + "'\n for writing.");
			return ;
	}	
	
	try {
	
		/*~~~~~~~~~create the cuboid in the file~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		/* A one at a particular position signifies the point is valid.            */
		/* A zero at a particular position signifies the point is invalid.         */
		/* So we need to set all the bits to 1(valid) initially.                   */
		fp.SeekToBegin();

		//First write the information about the point density along
		//the three axes on the target cuboid
		fp.Write(&ptDensityX, 4); 
		fp.Write(&ptDensityY, 4); 
		fp.Write(&ptDensityZ, 4) ;
		fp.Write(&minx, 4) ; 
		fp.Write(&maxx, 4) ; 
		fp.Write(&miny, 4) ; 
		fp.Write(&maxy, 4) ; 
		fp.Write(&minz, 4) ; 
		fp.Write(&maxz, 4) ;	
			
		//Write OTHER HEADER INFO AS WELL----------------------------		
		//Write whether snaps or slides option is chosen
		fp.Write(&option, 1); 
		//Number of files processed.
		fp.Write(&filesProcessed, 4); 	
		//-----------------------------------------------------------
	
	} catch(CException *e) {
		e->Delete();
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();
		AfxMessageBox("Error: Could not write to file.", MB_OK | MB_ICONSTOP);
		return;
	}
	
		
	if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
		fp.Close();

	CREATE_MOLD_INFO moldInfo;
	moldInfo.g_strOutputFile = filePath;

	if(flgUsingSnaps) {
		totalBits = ptDensityX * ptDensityY * ptDensityZ ; //size of the file in bits
		//increase the value to make it divisible by 8
		rem = totalBits%8 ;
		if(rem != 0 )
			totalBits = totalBits + (8 - rem);
			
		//find the total number of bytes required to store the points
		totalBytes = totalBits / 8;
		
		moldInfo.bytesToWrite = totalBytes + MOLD_HEADER_EXTRA_BYTES;

	} else
		moldInfo.bytesToWrite = MOLD_HEADER_EXTRA_BYTES;

	//Call the dialog window to write the bytes
	dlgCallThread dlg1(IDD_DIALOG3,  CREATE_MOLD, &moldInfo);
	dlg1.DoModal();
	delete dlg1;
	if(!dlg1.blnSuccess)
		return;
		
	if(OpenDocumentFile(filePath) == NULL)
		return;
}

void myApp::openFile() {
	CWinApp::OnFileOpen();
}

CString myApp::getHelpFilePath() {
	return g_strHelpFilePath;
}
	
void myApp::setCurWindowInfo(CURRENT_WINDOW  inCurWindow) {
	g_curWindow = inCurWindow;
}