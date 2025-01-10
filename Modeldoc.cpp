#include <afxwin.h>
#include "modeldoc.h"
#include "modelview.h"
#include "myframe.h"
#include "dialogs.h"
#include "myapp.h"

IMPLEMENT_DYNCREATE(modelDoc, CDocument)
/*
BEGIN_MESSAGE_MAP(modelDoc, CDocument)
END_MESSAGE_MAP()
*/

//Redundant function: This function will never be used. 
BOOL modelDoc::OnNewDocument() {

	/*
	//Base class function's code
	DeleteContents();//Delete previously created contents
	m_strPathName.Empty();   // no path name yet  
	
	strTest = "Created a new model document";
		
*/
  return true;
}

BOOL modelDoc::OnOpenDocument(LPCSTR filePath) {
	//if(!CDocument::OnOpenDocument(filePath))  //We are doing the file handling ourselves
	//	return FALSE;                         

	strFilePath = filePath;
	
	
	//Check for an invalid file extension----------
	//------------------------------------------------------------------------------
	strFilePath.TrimRight();
	CString strExtension;
	strExtension = strFilePath.Right(4);
	strExtension.MakeUpper();
	if(strExtension !=  _T(".MDL")) {
		AfxMessageBox("Error: This file type is not supported by the application.",  MB_ICONERROR | MB_OK, NULL);
		return false;
	}	
	//------------------------------------------------------------------------------

	
	//Load the model
	if(!loadModel())
		return false;
	
	//Create the model toolbar 
	modelToolBar.CreateEx(AfxGetMainWnd(), TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS );

	//Load the model toolbar
	modelToolBar.LoadToolBar(IDR_TOOLBAR4);

	//We need to hide the main toolbar if a document is being opened.
	((myFrame *)AfxGetMainWnd())->showHideToolBar(FALSE);	
	
	
	//Set the Mainframe's flag window to true, so that another document would not be loaded
	((myFrame *)AfxGetMainWnd())->setDocLoadedFlag(TRUE);

	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MODEL);
	
	return TRUE;
}

//Function to load the model file
//NOTE: All messages are displayed using the handle to the mainframe as this document file
//has not yet loaded.
BOOL modelDoc::loadModel() {

	//File Handle
	CFile fp;

	//Try to open the file for reading
	if(!fp.Open(strFilePath, CFile::modeRead)) {
		((myFrame *)AfxGetMainWnd())->MessageBox("The file could not be opened for reading","Error", MB_OK | MB_ICONSTOP);
		return false; //The document will be closed
	}
	
	//Get the size
	UINT uintFileSize = fp.GetLength();
	if(uintFileSize < 63) {
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();
		((myFrame *)AfxGetMainWnd())->MessageBox("Error: Invalid File size", "Error", MB_OK | MB_ICONSTOP);
		return false;
	}
	
	
	UINT size = uintFileSize - 27;
	if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
		fp.Close();
	
	//Inform the user about the total memory needed to load and ask for his confirmation------
	//----------------------------------------------------------------------------------------
	CString unit;
	UINT uintTotSize = size*2;
	if(uintTotSize > UINT_BYTES_PER_KILOBYTE)
		unit.Format(" %.2f KB", ((float)uintTotSize/(float)UINT_BYTES_PER_KILOBYTE));
	else
		unit.Format(" %d bytes", uintTotSize);
	CString str = "This model file requires " + unit + " of memory to load.\n"
		+ "Do you wish to continue?";
	
	if( (((myFrame *)AfxGetMainWnd())->MessageBox(str, "Confirm Loading", MB_YESNO)) != IDYES)
		return false;
	//----------------------------------------------------------------------------------------
		
	
	//Try loading the model	
	CString strInfo = myModel.loadModel(strFilePath);
	if(strInfo != "SUCCESS") {
		CString str = "The model could not be loaded due to the following error:\n" 
			+ strInfo;
		((myFrame *)AfxGetMainWnd())->MessageBox(str, "Error", MB_OK | MB_ICONSTOP);
		return false; //The document will be closed
	}
	
	return TRUE;

}

CString modelDoc::getFilePath() { 
	return strFilePath; 
}

UINT modelDoc::getFileSize() { 
	return myModel.getFileSize();
}

BOOL modelDoc::isModelLoaded() {
	return myModel.isModelLoaded();
}

RGBCOLOR modelDoc::getColor() {
	return myModel.getModelColor();
}

float * modelDoc::getPoints() {
	return myModel.getPoints();
}

float * modelDoc::getNormals() {
	return myModel.getNormals();
}

UINT modelDoc::getVertexCount() {
	return myModel.getVertexCount();
}

MODEL_POINT_RANGE modelDoc::getPointRange() { 
	return myModel.getPointRange();
 }

void modelDoc::DeleteContents() {
	
}

void modelDoc::OnCloseDocument() {
	
	//myModel object would be deleted in the next statement, so store the status in a flag
	BOOL blnModelLoaded = myModel.isModelLoaded();
	
	CDocument::OnCloseDocument();

	if(blnModelLoaded) {//if we had the model loaded
		//We need to show the main toolbar if all the documents are being closed
		((myFrame *)AfxGetMainWnd())->showHideToolBar(TRUE);

	}
	
	//Set the Mainframe's flag window indicating that no document is loaded as of now
	((myFrame *)AfxGetMainWnd())->setDocLoadedFlag(FALSE);
	
	//Set the value for displaying the help info
	((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_APP);
	
}


