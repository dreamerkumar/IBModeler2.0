#include "dialogs.h"
#include "errorConstants.h"
#include "myapp.h"
#include "productid.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////Worker functions for five different operations through threads////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

/* I.Worker function to write mold data ----------------------------------------------------*/
UINT workerFunctionCreateMld( LPVOID pt ) {
	
	CFile fp;
	CString path;
	BYTE *byte, *remByte;
	CString strErrorInfo, error;	
	UINT totalBytes, remainder, quotient, bytesWritten;
		
	byte = remByte = NULL;
	
	//read data from the passed object
	CREATE_MOLD_INFO *obj = (CREATE_MOLD_INFO *) pt;
	path = obj->g_strOutputFile;
	totalBytes = obj->bytesToWrite;
	obj->initMaxNumber(totalBytes);
	
	if(!fp.Open(path, CFile::modeReadWrite | CFile::shareExclusive )) {
		AfxMessageBox("Error: Could not open file: \n'" + path + "'\n for writing.");
		return 1;
	}
	//Check whether the operation has been cancelled
	if(obj->isCancelled()) {
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();
		return 1;
	}//---------------------------------------------


	//Write cuboid data to the file in units of 100,000 Bytes
	bytesWritten = 0;
	remainder  = totalBytes%100000;
	totalBytes = totalBytes - remainder;
	
		try {
			fp.SeekToEnd();//Bypass the header info
			
				if(obj->isCancelled()) {
					if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
						fp.Close();			
					return 1;
				}

			
			if(totalBytes > 0 ) {
				byte =(BYTE *)malloc(100000);
				memset(byte, 255, 100000);
				quotient = totalBytes/100000;				
				
				for(UINT ctr = 1; ctr <= quotient; ctr++) {
					
					//Check whether the operation has been cancelled
					if(obj->isCancelled()) {
						if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
							fp.Close();
						free(byte);
						byte = NULL;
						return 1;
						
					}//----------------------------------------------

					fp.Write(byte, 100000);
					bytesWritten+= 100000;
					obj->setNewPercent(bytesWritten);
				}
				
				free(byte);
				byte = NULL;
			}
			
			//Check whether the operation has been cancelled
			if(obj->isCancelled()) {
				if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
					fp.Close();
				return 1;						
			}//---------------------------------------------

			if(remainder > 0 ) {
				remByte = (BYTE *)malloc(remainder);
				memset(remByte, 255, remainder);
				fp.Write(remByte, remainder);
				obj->setNewPercent(obj->bytesToWrite); //All the bytes have been written
				free(remByte);
				remByte = NULL;
			}
		} catch ( CException *e ) { 
		
			error = "Error: An error occured while writing to file: \n'" + path + "'";
			if(e->GetErrorMessage((char *) &strErrorInfo, 400, NULL))
				error = error + "\n" + strErrorInfo;
			e->Delete();
			
			if(remByte != NULL) 
				free(remByte); 
			if(byte != NULL) 
				free(byte); 

			if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
				fp.Close();
			try {
				fp.Remove(path);
			} catch (CException *e) {
				e->Delete();
			}

			AfxMessageBox(error);
			return FILE_ACCESS_ERROR;
		}
	
	if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
		fp.Close();
	return 0;
}

/* II. Worker function for setting the background color ------------------------------------*/
UINT workerFnSetBkClr( LPVOID pt ) {
	
	//read data from the passed object
	SET_BK_COLOR_INFO *obj = (SET_BK_COLOR_INFO *) pt;
	
	BYTE btRVals[255], btGVals[255], btBVals[255];
	
	
	//Define the color values****************************************************************
	
	btRVals[0] = obj->btR; 
	btGVals[0] = obj->btG; 
	btBVals[0] = obj->btB; 
	
	int colorValues = 0;
	CHSL myHSL;
	
	BYTE btH, btS, btL;
	myHSL.RGB2HSL(btRVals[0], btGVals[0], btBVals[0], btH, btS, btL);

	if(obj->selType == SAT || obj->selType == LUM) {
		
		BYTE btVal1, btVal2;
		
		if(obj->selType == SAT) 
			btVal1 = btS;
		else
			btVal1 = btL;
		
		btVal2 = obj->btLumSatVal;
		
		if(btVal1 != btVal2) {
		
			BOOL blnAscending;
			if(btVal1 < btVal2)
				blnAscending = true;
			else
				blnAscending = false;
			
			while(( blnAscending && btVal1 < btVal2) || (!blnAscending && btVal1 > btVal2)) {
				if(blnAscending)
					btVal1++;
				else
					btVal1--;
				colorValues++;

				if(obj->selType == SAT) 
					myHSL.HSL2RGB(btRVals[colorValues], btGVals[colorValues], 
						btBVals[colorValues], btH, btVal1, btL);
				else
					myHSL.HSL2RGB(btRVals[colorValues], btGVals[colorValues], 
						btBVals[colorValues], btH, btS, btVal1);					
			}
		}
	
	}//********************End of defining the color values**********************************

	//Loop through the image pixels to set them to the background color
	BYTE btBkR, btBkG, btBkB;
	int intHeight, intWidth, intExtraBytes, intXCtr, intYCtr, intColorsCtr;
	BOOL blnMatched;
	btBkR = obj->btBkR;
	btBkG = obj->btBkG;
	btBkB = obj->btBkB;
	intHeight = obj->intHeight;
	intWidth = obj->intWidth;
	intExtraBytes = obj->intExtraBts;
	
	BYTE btTempH, btTempS, btTempL;

	BYTE *ptr = (BYTE *) obj->lpvImgBits;

	obj->initMaxNumber(intWidth*intHeight);

	UINT uintPixelsProcessed = 0;

	
	//Variables specific to the screen option -----------------------------------------------------
	float fltPercentMultiplier = ((float)obj->btLumSatVal) /100.0f;
	BYTE btMainClrOffset;
	int intAllowedDiff;
	//---------------------------------------------------------------------------------------------


	//TODO: Should not work on the padded regions. However, since in this version, the padded color
	//and the background color is the same, we can allow it here.
	
	for( intYCtr = 1; intYCtr<= intHeight; intYCtr++) {
		
		for(intXCtr = 1; intXCtr <= intWidth; intXCtr++) {
			blnMatched = false;			

			uintPixelsProcessed++;

			if(obj->isCancelled())
				return FAILURE;
			
			
			switch(obj->selType) {
			
			
			case CUR_AND_BRIGHTER:
			case CUR_AND_DARKER:

				myHSL.RGB2HSL(ptr[2],ptr[1],ptr[0], btTempH, btTempS, btTempL);		
				
				if((obj->selType == CUR_AND_BRIGHTER  && btTempL >= btL)
					|| (obj->selType == CUR_AND_DARKER && btTempL <= btL )) 
					blnMatched = true;

				break;

			case SAT:
			case LUM:
				for(intColorsCtr = 0 ; intColorsCtr <= colorValues; intColorsCtr ++) {
					
					if(abs(ptr[2] - btRVals[intColorsCtr]) <= 1 && abs(ptr[1] - btGVals[intColorsCtr]) <= 1 
						&& abs(ptr[0] - btBVals[intColorsCtr]) <= 1 ) {

						blnMatched = true;
						break;
					}
					
				}
				
				break;				
			
			
			case RED_SCREEN:
				
				if(ptr[2] >= btRVals[0] && (ptr[1] == 0 || ptr[2] >= (ptr[1] + btRVals[0]))
					&& (ptr[0] == 0 || ptr[2] >= (ptr[0] + btRVals[0]))) {
				
						
					if(ptr[1] > ptr[0]) 
						btMainClrOffset = ptr[2] - ptr[1];
					else
						btMainClrOffset = ptr[2] - ptr[0];

				
					intAllowedDiff = (int) ( fltPercentMultiplier * ((float)btMainClrOffset) );

					
					
					if(abs(ptr[0] - ptr[1]) <= intAllowedDiff)
						blnMatched = true;						
					
				
				}

				break;
			case GREEN_SCREEN:

				if(ptr[1] >= btGVals[0] && (ptr[0] == 0 || ptr[1] >= (ptr[0] + btGVals[0]))
					&& (ptr[2] == 0 || ptr[1] >= (ptr[2] + btGVals[0]))) {
				
						
					if(ptr[0] > ptr[2]) 
						btMainClrOffset = ptr[1] - ptr[0];
					else
						btMainClrOffset = ptr[1] - ptr[2];

				
					intAllowedDiff = (int) ( fltPercentMultiplier * ((float)btMainClrOffset) );

					
					
					if(abs(ptr[0] - ptr[2]) <= intAllowedDiff)
						blnMatched = true;						
					
				
				}
				break;
			case BLUE_SCREEN:
				
				if(ptr[0] >= btBVals[0] && (ptr[1] == 0 || ptr[0] >= (ptr[1] + btBVals[0]))
					&& (ptr[2] == 0 || ptr[0] >= (ptr[2] + btBVals[0]))) {
				
						
					if(ptr[1] > ptr[2]) 
						btMainClrOffset = ptr[0] - ptr[1];
					else
						btMainClrOffset = ptr[0] - ptr[2];

				
					intAllowedDiff = (int) ( fltPercentMultiplier * ((float)btMainClrOffset) );

					
					if(abs(ptr[2] - ptr[1]) <= intAllowedDiff)
						blnMatched = true;											
				}
				
				break;

			case NONE: //Match exact colors
				if(ptr[0] == btBVals[0] && ptr[1] == btGVals[0] && ptr[2] == btRVals[0])
					blnMatched = true;	
				break;
			}

					
			if(blnMatched) {
				ptr[2] = btBkR;
				ptr[1] = btBkG;
				ptr[0] = btBkB;					
			}
			ptr += 3;
		}
		ptr += intExtraBytes;	//Bypass the extra pixels used for alignment
	
		obj->setNewPercent(uintPixelsProcessed);
	}
	
	return SUCCESS; //0
}

/* III. Worker function for processing the mold using the bitmap----------------------------*/
UINT workerFnProcessMld( LPVOID pt ) {
	
	//read data from the passed object
	PROCESS_MLD_INFO *obj = (PROCESS_MLD_INFO *) pt;

	HMODULE moduleHandle;
	moduleHandle = ::LoadLibrary("imgtoobj.dll");

	typedef UINT (pfun) (PROCESS_MLD_INFO *inData);
	
	pfun *f = 0;

	f = (pfun *) ::GetProcAddress(moduleHandle, "processMold");
	
	UINT intRetVal = (*f) (obj);

	::FreeLibrary(moduleHandle);

	return intRetVal;
	
}

/* IV. Worker function for adding a slide to the mold---------------------------------------*/
UINT workerFnAddSlide(LPVOID pt) {
	
	//read data from the passed object
	PROCESS_MLD_INFO *obj = (PROCESS_MLD_INFO *) pt;

	HMODULE moduleHandle;
	moduleHandle = ::LoadLibrary("slidestoobj.dll");

	typedef UINT (pfun) (PROCESS_MLD_INFO *inData);
	
	pfun *f = 0;

	f = (pfun *) ::GetProcAddress(moduleHandle, "addSlide");
	
	UINT intRetVal = (*f) (obj);

	::FreeLibrary(moduleHandle);

	return intRetVal;
	
}
/* V. Worker function for creating the model -----------------------------------------------*/
UINT workerFnCreateModel( LPVOID pt ) {
	
	//read data from the passed object
	CREATE_MODEL_INFO *obj = (CREATE_MODEL_INFO *) pt;

	HMODULE moduleHandle;
	moduleHandle = ::LoadLibrary("ptstopolygons.dll");

	typedef UINT (pfun) (CREATE_MODEL_INFO *inData);
	
	pfun *f = 0;

	f = (pfun *) ::GetProcAddress(moduleHandle, "createModel");
	
	UINT intRetVal = (*f) (obj);

	::FreeLibrary(moduleHandle);

	return intRetVal;
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////End of worker functions///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
dlgCallThread::dlgCallThread(int n, THREAD_OPERATION_TYPE inOprtnType, PROCESS_INFO *info) 
		: CDialog(n) {
	clkStart = clock();
	ptr = info;
	oprtnType = inOprtnType;
	blnSuccess  = false;
	g_blnCancelClicked = false;
	percentLast = 0;

}

int dlgCallThread::OnInitDialog() {
	CDialog::OnInitDialog();

    pgr = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
	pgr->SetStep(5);

	stHeading	= (CStatic *)GetDlgItem(IDC_STATIC1);
	stPercent	= (CStatic *)GetDlgItem(IDC_STATIC2);
	stTime		= (CStatic *)GetDlgItem(IDC_STATIC3);
	
	CString strHeading;

	//Initiate the worker thread
	if(oprtnType == SET_BK_COLOR) {
		t = AfxBeginThread(workerFnSetBkClr, (LPVOID)ptr, THREAD_PRIORITY_NORMAL, 0, 
			CREATE_SUSPENDED, NULL);
		strHeading = "Setting Color to background";
	} else if(oprtnType == CREATE_MODEL) {
		t = AfxBeginThread(workerFnCreateModel, (LPVOID)ptr, THREAD_PRIORITY_NORMAL, 0, 
			CREATE_SUSPENDED, NULL);
		strHeading = "Creating Model";
	} else if(oprtnType == PROCESS_MOLD_SNAPS)  {
		strHeading = "Processing Mold";
		t = AfxBeginThread(workerFnProcessMld, (LPVOID)ptr, THREAD_PRIORITY_NORMAL, 0, 
			CREATE_SUSPENDED, NULL);
	} else if(oprtnType == PROCESS_MOLD_SLIDES)  {
		strHeading = "Adding Slide";
		t = AfxBeginThread(workerFnAddSlide, (LPVOID)ptr, THREAD_PRIORITY_NORMAL, 0, 
			CREATE_SUSPENDED, NULL);
	} else if(oprtnType == CREATE_MOLD) {
		strHeading = "Creating Mold";
		t = AfxBeginThread(workerFunctionCreateMld, (LPVOID)ptr, THREAD_PRIORITY_NORMAL, 0, 
			CREATE_SUSPENDED, NULL);
	}	
	
	stHeading->SetWindowText(strHeading);
	t->m_bAutoDelete = FALSE;
	t->ResumeThread();	

	m_nTimer = SetTimer(1, 100, NULL); //set the timer function 

	return TRUE;
}

void dlgCallThread::OnTimer(UINT nIDEvent) {

	if(g_blnCancelClicked)
		return;
	
	if(GetExitCodeThread(t->m_hThread,&g_lngExitCode)) {
		if(g_lngExitCode != STILL_ACTIVE) { //Thread has exited
			
			if(g_lngExitCode == 0 ) 
				blnSuccess = true;
			
			KillTimer(m_nTimer);
			delete t;
			t = NULL;
			
			if(ptr->isCancelled()) {
				if(g_lngExitCode == 0) {
					//Tell the user that the operation has already completed
					AfxMessageBox("Operation Complete:\n\nThe operation completed before the cancel request could be executed.");
					
				} else {
					AfxMessageBox("The operation was cancelled.");
					if(oprtnType == CREATE_MODEL || oprtnType == CREATE_MOLD ) {
						CFile fp;
						fp.Remove(ptr->g_strOutputFile);
					} 
				}
			}

			CDialog::OnCancel();
		}	else {
			
					
			float fltPercent = ptr->getCompletedPercent() ;
			CString str;
			str.Format("%.2f", fltPercent);
			stPercent->SetWindowText("Status: " + str + "% Complete");	
			
			int percent = getIntFromFloat(fltPercent);
			if(percent > (percentLast + 4)) {
				percentLast = percent;
				pgr->SetPos(percent);
			}
				
			displayElapsedTime();
		}
			
	} else {
		MessageBox("Error: could not get the exit code of thread");
		KillTimer(m_nTimer);	
		CDialog::OnCancel();
	}
}
void dlgCallThread::displayElapsedTime() {
	
	int rem;
	int hours = 0;
	int minutes = 0;
	CString time;
	clock_t clkCurrent = clock();
	int seconds = (int) ((clkCurrent - clkStart) / CLOCKS_PER_SEC);
	
	if(seconds >= 3600) {//Hours left
		rem = seconds%3600;
		hours = (seconds - rem)/3600;
		seconds = rem;
	}

	if(seconds >= 60) {//Minutes left
		rem = seconds%60;
		minutes = (seconds - rem)/60;					
		seconds = rem;
	}
	
	time.Format("%2d:%2d:%2d", hours, minutes, seconds);
	if(hours < 10)
		time.SetAt(0,'0');
	if(minutes < 10)
		time.SetAt(3,'0');
	if(seconds < 10)
		time.SetAt(6,'0');

	stTime->SetWindowText("Elapsed time: " + time);

}

void dlgCallThread::OnCancel() {

	g_blnCancelClicked = true; //This will stop the timer function from executing.

	if(oprtnType == PROCESS_MOLD_SNAPS) {
	
		dlgYesNo dlg(IDD_DIALOG2, "Are you sure you want to quit the operation?", FALSE);
		if(dlg.DoModal() != IDOK) {
			g_blnCancelClicked = false;
			return;
		}
	}
	
	((CButton *)GetDlgItem(IDCANCEL))->EnableWindow(FALSE);//Don't allow the user to click on
	//cancel twice

	ptr->cancelProcess(); //If the worker function is still working, it will terminate.
	
	g_blnCancelClicked = false; //Timer function can be called again

}

BEGIN_MESSAGE_MAP(dlgCallThread, CDialog)
	ON_WM_TIMER()	
END_MESSAGE_MAP()


/*********Dialog window for the initial splash screen*********************************/
//the view window class for the splash view

BEGIN_MESSAGE_MAP(splashView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()	
END_MESSAGE_MAP()

int splashView::OnCreate ( LPCREATESTRUCT l ) {
	CWnd::OnCreate(l);
	
	m_blnShowImage = true;
	
	//Load the bitmap*********
	m_hImage = (HBITMAP)LoadImage(NULL, "splash.bmp", 
		IMAGE_BITMAP,0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION|
		LR_DEFAULTSIZE);
	if (!m_hImage) {
		m_blnShowImage = false;
		return TRUE;
	}//--------------------------------------------------------
	
	//Attach the image handle to a CBitmap object*****
	if (!m_bitmap.Attach(m_hImage)) {
		m_blnShowImage = false;
		return TRUE;
	}//----------------------------------------------


	//Get image information into a bitmap structure***
	BITMAP pBitMap;
	m_bitmap.GetBitmap(&pBitMap);
	//------------------------------------------------
	
	//Get the dimensions of the image********
	m_intHeight = pBitMap.bmHeight;
	m_intWidth  = pBitMap.bmWidth;

	return TRUE;

}

void splashView::OnPaint() {
	
	CWnd::OnPaint();
	if(!m_blnShowImage)
		return;
	
	CClientDC p(this);	

	//If the bitmap is not selected on the document class CDC object then select it
	if(m_memDC.m_hDC == NULL) {
		if (!m_memDC.CreateCompatibleDC(&p)) {	
			return;
		}
		m_memDC.SelectObject(&m_bitmap);
	}		
	
	//Copy from the doc memory DC to display DC
	
	if (!p.StretchBlt(0, 0, 
			m_intWidth, 
			m_intHeight, 
			&m_memDC, 0, 0, 
			m_intWidth, m_intHeight, SRCCOPY))
			TRACE0("BitBlt failed\n");
			
}

void splashView::OnDestroy() {
	if(m_memDC.m_hDC != NULL)
		m_memDC.DeleteDC();
	if(m_blnShowImage)
		m_bitmap.DeleteObject();
	CWnd::OnDestroy();
}

//The dialog class for the splash view
BEGIN_MESSAGE_MAP(dlgSplash, CDialog)
	ON_WM_TIMER( )
END_MESSAGE_MAP()

dlgSplash::dlgSplash(int n): CDialog(n) {}
	
int dlgSplash::OnInitDialog() {
	CDialog::OnInitDialog();
	
	
	WNDCLASS wndClass;

	wndClass.style = CS_OWNDC;
	wndClass.lpfnWndProc = AfxWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = AfxGetInstanceHandle();
	wndClass.hIcon = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "SplashViewClass";

	// Register the window class
	AfxRegisterClass(&wndClass);
	
	
	//IMPORTANT NOTE: PLEASE DON'T DELETE THE ABOVE CODE; EVEN IF YOU ARE NOT USING THE SPLASH WINDOW
	//BECAUSE THE ABOVE REGISTERED CLASS IS BEIND USED BY OTHER PREVIEW WINDOWS.	
	
	CRect rect;
	GetClientRect(&rect);
	m_view.Create("SplashViewClass",NULL,WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			CRect(0, 0, rect.Width(), rect.Height()),this, 1,NULL);
	
	m_view.Invalidate(FALSE);	
	
	SetTimer ( 1, 2000, NULL ) ;

	return TRUE;
}

void dlgSplash::OnTimer() {
	OnCancel( ) ;
}

/*************************************************************************************/

/**************************Dialog window for about box***************/
dlgAbout::dlgAbout(int n)  : CDialog(n) {} 

int dlgAbout::OnInitDialog() {	
	CDialog::OnInitDialog();
	
	CEdit *edit = (CEdit *)GetDlgItem(IDC_EDIT1);
	edit->SetWindowText("Warning: This computer program is protected by copyright law and international treaties. Unauthorized reproductin or distribution of this program, or any portion of it, may result in severe civil and criminal penalties, and will be prosecuted to the maximum extent possible under the law. ");
	
	//Display the product id
	CStatic *st = (CStatic *)GetDlgItem(IDC_STATIC1);
	CString strProductID;
	strProductID = "Product ID: ";
	strProductID += PRODUCT_ID;
	st->SetWindowText(strProductID);

	return TRUE;
}
/*************************************************************************************/

/**********Dialog box which takes up the input parameters for creating the mold**************/

BEGIN_MESSAGE_MAP(dlgCreateMold, CDialog)
	ON_COMMAND(IDC_RADIO1,  updtStatusForSnaps)
	ON_COMMAND(IDC_RADIO2,  updtStatusForSlides)
	ON_EN_CHANGE(IDC_EDIT1, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT2, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT3, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT4, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT5, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT6, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT7, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT8, updtFileSizeInfo)
	ON_EN_CHANGE(IDC_EDIT9, updtFileSizeInfo)
END_MESSAGE_MAP()

dlgCreateMold::dlgCreateMold(int n) : CDialog(n) {
	flgOKBtn = false;	
	flgSt27Created =false;
	minx = miny = minz = -10.0f;
	maxx = maxy = maxz =  10.0f;
}

int dlgCreateMold::OnInitDialog() {
	
	CDialog::OnInitDialog();
	spin1 = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN1);
	spin2 = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN2);
	spin3 = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN3);

	spin1->SetRange(2,  UINT_MAX_AXES_POINTS);
	spin1->SetPos(50);

	spin2->SetRange(2,  UINT_MAX_AXES_POINTS);
	spin2->SetPos(50);

	spin3->SetRange(2,  UINT_MAX_AXES_POINTS);
	spin3->SetPos(50);

	edit1 = (CEdit *)GetDlgItem(IDC_EDIT1);
	edit2 = (CEdit *)GetDlgItem(IDC_EDIT2);
	edit3 = (CEdit *)GetDlgItem(IDC_EDIT3);
	edit4 = (CEdit *)GetDlgItem(IDC_EDIT4);
	edit5 = (CEdit *)GetDlgItem(IDC_EDIT5);
	edit6 = (CEdit *)GetDlgItem(IDC_EDIT6);
	edit7 = (CEdit *)GetDlgItem(IDC_EDIT7);
	edit8 = (CEdit *)GetDlgItem(IDC_EDIT8);
	edit9 = (CEdit *)GetDlgItem(IDC_EDIT9);

	edit1->LimitText(7);
	edit2->LimitText(7);
	edit3->LimitText(4);
	edit4->LimitText(7);
	edit5->LimitText(7);
	edit6->LimitText(4);
	edit7->LimitText(7);
	edit8->LimitText(7);
	edit9->LimitText(4);
	
	st25 = (CStatic *)GetDlgItem(IDC_STATIC25);
	st27 = (CStatic *)GetDlgItem(IDC_STATIC27);
	
	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	flgSt27Created =true;

	updtFileSizeInfo();
	return TRUE;
}

void dlgCreateMold::DoDataExchange(CDataExchange *pdx) {
	
	CDialog::DoDataExchange(pdx);	
			
	DDX_Text(pdx, IDC_EDIT1, minx);
	DDV_MinMaxFloat(pdx, minx, -500.00f, 500.00f);

	DDX_Text(pdx, IDC_EDIT2, maxx);
	DDV_MinMaxFloat(pdx, maxx, -500.00f, 500.00f);

	DDX_Text(pdx, IDC_EDIT3, ptDensityX);
	DDV_MinMaxUInt(pdx, ptDensityX, 2,  UINT_MAX_AXES_POINTS);

	DDX_Text(pdx, IDC_EDIT4, miny);
	DDV_MinMaxFloat(pdx, miny, -500.00f, 500.00f);

	DDX_Text(pdx, IDC_EDIT5, maxy);
	DDV_MinMaxFloat(pdx, maxy, -500.00f, 500.00f);

	DDX_Text(pdx, IDC_EDIT6, ptDensityY);
	DDV_MinMaxUInt(pdx, ptDensityY, 2,  UINT_MAX_AXES_POINTS);

	DDX_Text(pdx, IDC_EDIT7, minz);
	DDV_MinMaxFloat(pdx, minz, -500.00f, 500.00f);

	DDX_Text(pdx, IDC_EDIT8, maxz);
	DDV_MinMaxFloat(pdx, maxz, -500.00f, 500.00f);

	if(flgUsingSnaps) {

		DDX_Text(pdx, IDC_EDIT9, ptDensityZ);
		DDV_MinMaxUInt(pdx, ptDensityZ, 2,  UINT_MAX_AXES_POINTS);
	}
		
}

/*---------------If snapshots option is chosen-------------------------------------*/
void dlgCreateMold::updtStatusForSnaps() {
	spin3->EnableWindow(TRUE);
	
	edit9->EnableWindow(TRUE);
	
	st25->EnableWindow(TRUE);
	updtFileSizeInfo();
}

/*---------------If slides option is chosen---------------------------------------*/
void dlgCreateMold::updtStatusForSlides() {
	spin3->EnableWindow(FALSE);
	
	edit9->EnableWindow(FALSE);
	
	st25->EnableWindow(FALSE);
	updtFileSizeInfo();
}
	
/*------Function called on change of any values to update the file size info-----*/
void dlgCreateMold::updtFileSizeInfo() {
	
	if(flgSt27Created) {//If initialization of the static control is complete.
		
		float fltSize;
		CString strUnit;
		CString strOut = "";
		
		strOut = updtVariables();
		if(strOut == "") { //Data not Erroneous

			if(flgUsingSnaps) {//Snaps selected 
				//Check whether the number of points exceed 4, 000, 000, 000.
				fltSize = (float) (ptDensityX )* (float)(ptDensityY * ptDensityZ);
							
				if(fltSize  > 4000000000.0f) 
					strOut.Format("Total points (approx %.0f) exceeds the maximum limit of 4, 000, 000, 000 points.", fltSize);
			
			} else
				fltSize =  (float)(ptDensityX * ptDensityY);  	
				
		}
			
		if(strOut == "") { //Data not Erroneous	
			
			fltSize/= 8.0f; //Convert to bytes		
			
			float fltBytesPerKB = (float)UINT_BYTES_PER_KILOBYTE;

			
			if(fltSize < fltBytesPerKB) 
				strUnit = "Bytes";
			else if(fltSize < 100000.0f) {
				strUnit = "KB";
				fltSize/= fltBytesPerKB;
			} else if(fltSize < 100000000.0f){
				strUnit = "MB";
				fltSize/= (fltBytesPerKB*fltBytesPerKB);
			} else {
				strUnit = "GB";
				fltSize/= (fltBytesPerKB*fltBytesPerKB*fltBytesPerKB);
			}

			strOut.Format("Estimated file size: %.2f ", fltSize);
			strOut = strOut + strUnit;
			if(!flgUsingSnaps) 
				strOut = strOut + " per slide";
					
		}
		st27->SetWindowText(strOut);		
	}
}

/*-------------Function to retrieve and validate the data of the controls-----------------*/
CString dlgCreateMold::updtVariables() {

	CString ctrlText;
	
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2) == IDC_RADIO1)
		flgUsingSnaps = true;
	else
		flgUsingSnaps = false;

	/*//minx------------------------------------------------------------
	GetDlgItemText(IDC_EDIT1, ctrlText);
	minx = (float)atof(ctrlText);
	if(minx < -500.0f || minx > 500.0f) 
		return "Minx should be between -500.0 and 500.0";
	
	//maxx------------------------------------------------------------
	GetDlgItemText(IDC_EDIT2, ctrlText);
	maxx = (float)atof(ctrlText);
	if(maxx < -500.0f || maxx > 500.0f) 
		return "Maxx should be between -500.0 and 500.0";

	if((maxx - minx) < 1.0f) 
		return "Maxx should be greater than minx by at least 1 cm.";*/

	//Point density along X-------------------------------------------
	ptDensityX = GetDlgItemInt(IDC_EDIT3);		
	if(ptDensityX < 2 || ptDensityX >  UINT_MAX_AXES_POINTS) {
		CString str;
		str.Format("%d", UINT_MAX_AXES_POINTS);
		return "Point density along X should be between 2 and " + str;
	}
	
	/*//miny------------------------------------------------------------
	GetDlgItemText(IDC_EDIT4, ctrlText);
	miny = (float)atof(ctrlText);
	if(miny < -500.0f || miny > 500.0f) 
		return "Miny should be between -500.0 and 500.0";

	//maxy------------------------------------------------------------
	GetDlgItemText(IDC_EDIT5, ctrlText);
	maxy = (float)atof(ctrlText);
	if(maxy < -500.0f || maxy > 500.0f) 
		return "Maxy should be between -500.0 and 500.0";
	
	if((maxy - miny) < 1.0f) 
		return "Maxy should be greater than miny by at least 1 cm.";*/
	
	//Point density along Y-------------------------------------------	
	ptDensityY = GetDlgItemInt(IDC_EDIT6);
	if(ptDensityY < 2 || ptDensityY >  UINT_MAX_AXES_POINTS) {
		CString str;
		str.Format("%d", UINT_MAX_AXES_POINTS);
		return "Point density along Y should be between 2 and " + str;

	}
	/*//minz------------------------------------------------------------
	GetDlgItemText(IDC_EDIT7, ctrlText);
	minz = (float)atof(ctrlText);		
	if(minz < -500.0f || minz > 500.0f) 
		return "Minz should be between -500.0 and 500.0";

	//maxz------------------------------------------------------------
	GetDlgItemText(IDC_EDIT8, ctrlText);
	maxz = (float)atof(ctrlText);
	if(maxz < -500.0f || maxz > 500.0f) 
		return "Maxz should be between -500.0 and 500.0";

	if((maxz - minz) < 1.0f) 
		return "Maxz should be greater than minz by at least 1 cm.";*/

	
	if(flgUsingSnaps) {

		//Point density along Z-------------------------------------------	
		ptDensityZ = GetDlgItemInt(IDC_EDIT9);
		if(ptDensityZ < 2 || ptDensityZ > UINT_MAX_AXES_POINTS) {
			CString str;
			str.Format("%d", UINT_MAX_AXES_POINTS);
			return "Point density along Z should be between 2 and " + str;
		}
	} 

	return "";		
}

/*-----------------------If OK button is clicked--------------------------------------*/
void dlgCreateMold::OnOK() {
	
	double size;
	CString strMessage;
	CString strUnit;
	
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2) == IDC_RADIO1)
		flgUsingSnaps = true;
	else
		flgUsingSnaps = false;

	if(!UpdateData(TRUE))
		return;	
	
	if((maxx - minx) < 1.0f) {
		MessageBox("Maxx should be greater than minx by at least 1 cm.");
		edit2->SetSel(0,-1);
		edit2->SetFocus();
		return;
	}

	if((maxy - miny) < 1.0f) {
		MessageBox("Maxy should be greater than miny by at least 1 cm.");
		edit5->SetSel(0,-1);
		edit5->SetFocus();
		return;
	}

	if((maxz - minz) < 1.0f) {
		MessageBox("Maxz should be greater than minz by at least 1 cm.");
		edit8->SetSel(0,-1);
		edit8->SetFocus();
		return;
	}
	
	if(flgUsingSnaps) {

		float totPoints = (float)(ptDensityX) * (float)(ptDensityY * ptDensityZ);
		if(totPoints  > 4000000000.0f) {
			CString str;
			str.Format("Total points defining the mold (approx %.0f) exceeds the maximum limit of 4, 000, 000, 000 points.\nPlease reduce the number of points along the axes.", totPoints);
			MessageBox(str);
			edit3->SetSel(0,-1);
			edit3->SetFocus();
			return;		
		}

		size = totPoints/8.0f; //Convert to bytes
		
	} else {
		size =  (float)(ptDensityX * ptDensityY);  	
		size/= 8.0f; //Convert to bytes		

	}

	//Generate message for storage size prompt.
	if(size < 1000.0f) 
		strUnit = "Bytes";
	else if(size < 100000.0f) {
		strUnit = "KB";
		size/= 1000.0f;
	} else if(size < 100000000.0f){
		strUnit = "MB";
		size/= 1000000.0f;
	} else {
		strUnit = "GB";
		size/= 1000000000.0f;
	}	
	strMessage.Format("\n\nStorage space required by the mold file is %.2f ", size);
	strMessage = strMessage + strUnit;
	if(!flgUsingSnaps) 
		strMessage = strMessage + " per slide";
	strMessage = strMessage + ".\nDo you wish to continue?";
	
	dlgYesNo d(IDD_DIALOG2, strMessage, TRUE);
	if(d.DoModal() != IDOK) 
		return;
	
	flgOKBtn = true;
	CDialog::OnOK();


}

/*------------------------If Cancel button is clicked--------------------------------------*/
void dlgCreateMold::OnCancel() {
	CDialog::OnCancel();
}

/******************************Yes/No dialog box*********************************************/
dlgYesNo::dlgYesNo(int n, CString m, BOOL inBlnYesBtn) : CDialog(n) {
	message = m;	
	blnYesBtn = inBlnYesBtn;
}

dlgYesNo::OnInitDialog() {
	CDialog::OnInitDialog();
	st = (CStatic *)GetDlgItem(IDC_STATIC2);
	st->SetWindowText(message);
	return TRUE;
}

void dlgYesNo::OnPaint() {
	CDialog::OnPaint();
	if(!blnYesBtn) {
		(GetDlgItem(IDCANCEL))->SetFocus();
		((CButton*)(GetDlgItem(IDCANCEL)))->SetButtonStyle(BS_DEFPUSHBUTTON, TRUE);   
		blnYesBtn = TRUE; //Set Focus should not be given after the first onpaint call.
	}
	
}
BEGIN_MESSAGE_MAP(dlgYesNo, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


/*************************Dialog box to alter the size of marker*****************************/
dlgChangeMarkerSize::dlgChangeMarkerSize(int n, UINT mkSize) : CDialog(n) {
	size = mkSize;
}

dlgChangeMarkerSize::OnInitDialog() {
	
	//Dialog Height 284, Dialog Width = 311	
	CDialog::OnInitDialog();

	prWin.markerSize = 1;
	prWin.CreateEx(WS_EX_WINDOWEDGE, "SplashViewClass", "", WS_CHILD | WS_VISIBLE, 
											CRect(63, 35, 248, 220),this, 1); 	

	scr.Create(SBS_HORZ | WS_CHILD | WS_VISIBLE, CRect(10, 225, 300, 240), this, 2);
	
	scr.SetScrollRange(0, 92);
	scr.SetScrollPos(0);	

	stPixelSize	= (CStatic *)GetDlgItem(IDC_STATIC1);	
	updateDisplay();
	return TRUE;
}
void dlgChangeMarkerSize::OnHScroll(UINT code, UINT pos, CScrollBar *scroll) {
	
	if(code == SB_THUMBTRACK)
		size = ((int)pos *2) + 1;
	if(code == SB_LINEDOWN && size <= 183)
		size+= 2;
	if(code == SB_LINEUP && size >= 3)
		size-= 2;
	
	updateDisplay();
}

void dlgChangeMarkerSize::updateDisplay() {
	
	//Set the scroll position 
	scr.SetScrollPos((size -1)/2, 1);
	
	//Update the display of pixel size in stPixelSize
	CString str;
	str.Format("Marker size: %d pixels", size);
	stPixelSize->SetWindowText(str);

	//Draw the marker with the new size
	prWin.markerSize = size;
	prWin.Invalidate(TRUE);
}

UINT dlgChangeMarkerSize::getMarkerSize() {
	return size;
}

BEGIN_MESSAGE_MAP(dlgChangeMarkerSize, CDialog)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()
/***************************Class used by dlgChangeMarkerSize********************************/
void previewWindow::OnPaint() {
		
	int width, posX, posY;
	CPaintDC d(this);
	COLORREF clr(RGB(255, 0, 0));
	CRect r, r1;
	

	GetClientRect(&r);
	
	width = r.right - markerSize;
	if(width%2 == 0 ) 
		posX = width/2;
	else
		posX = (width -1 ) /2;
	width = r.bottom - markerSize;
	
	if(width%2 == 0 ) 
		posY = width/2;
	else
		posY = (width -1 ) /2;

	r1.top = posY;
	r1.bottom = posY + markerSize;
	r1.left = posX ;
	r1.right = posX + markerSize;

	
	//Background
	CBrush pBrush1;
	pBrush1.CreateSolidBrush(RGB(255, 100, 100));
	d.FillRect(r, &pBrush1);
	
	CBrush pBrush;
	pBrush.CreateSolidBrush(RGB(255, 255, 255));
	d.FillRect(r1, &pBrush);
		
}

BEGIN_MESSAGE_MAP(previewWindow, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

//***************Class used to select a range of colors to set as background*****************

dlgSelectBkColors::dlgSelectBkColors(int n, BYTE inBtRed, BYTE inBtGreen, BYTE inBtBlue) 
	: CDialog(n) {
	
	prWin1.btRed = btRed1 = inBtRed;
	prWin1.btGreen = btGreen1 = inBtGreen;
	prWin1.btBlue = btBlue1 = inBtBlue;

	prWin1.blnShowEnabled = prWin1.blnShowColor = true;
	prWin2.blnShowEnabled = prWin2.blnShowColor = false;

	blnColor2Selected = false;
	
}

dlgSelectBkColors::OnInitDialog() {
	
	CDialog::OnInitDialog();

	prWin1.CreateEx(WS_EX_WINDOWEDGE, "SplashViewClass", "", WS_CHILD | WS_VISIBLE, 
	   	CRect(12, 35, 128, 145),this, 1); 	

	prWin2.CreateEx(WS_EX_WINDOWEDGE, "SplashViewClass", "", WS_CHILD | WS_VISIBLE, 
		CRect(170, 35, 286, 145),this, 2); 	

	scrSat.Create(SBS_VERT | WS_CHILD | WS_VISIBLE, CRect(292, 35, 312, 243), this, 3);
	
	scrSat.SetScrollRange(0, 240);
	scrSat.EnableScrollBar(ESB_DISABLE_BOTH);

	scrLum.Create(SBS_VERT | WS_CHILD | WS_VISIBLE, CRect(330, 35, 350, 243), this, 4);

	scrLum.SetScrollRange(0, 240);
	scrLum.EnableScrollBar(ESB_DISABLE_BOTH);
	
	GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);

	//Default is to set just the selected color to the background
	((CButton *)(GetDlgItem(IDC_RADIO3)))->SetCheck(1);
	selectionType = NONE;

	return TRUE;
}

void dlgSelectBkColors::defineColorRange() {//Called when users wants to specify a range of 
											//colors rather than a single RGHB value
	blnColor2Selected = true;

	//Disable the first two buttons
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);

	GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO5)->EnableWindow(FALSE);
	
	//Change the display of the first window
	prWin1.blnShowEnabled = false;
	prWin1.Invalidate();	

	//Enable and initialize the radio buttons
	GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
	((CButton *)(GetDlgItem(IDC_RADIO1)))->SetCheck(1);
	
	//The second color window should be displayed 
	prWin2.blnShowEnabled = prWin2.blnShowColor = true;
	
	//Get the Hue, Sat and Lum values
	myHsl.RGB2HSL(btRed1, btGreen1, btBlue1, btHue, btSat1, btLum1);
	
	selectLum(); //Allow Lum variation as default	
	CString str;
	
}
void dlgSelectBkColors::OnVScroll(UINT code, UINT pos, CScrollBar *scroll) {
	
	if(scroll == &scrSat) {	
		if(code == SB_THUMBTRACK)
			btSat2 = pos;
		if(code == SB_LINEDOWN && btSat2 < 240)
			btSat2++;
		if(code == SB_LINEUP && btSat2 > 0)
			btSat2--;
		scrSat.SetScrollPos(btSat2, TRUE);
	} else if(scroll == &scrLum) {	
		if(code == SB_THUMBTRACK)
			btLum2 = pos;
		if(code == SB_LINEDOWN && btLum2 < 240)
			btLum2++;
		if(code == SB_LINEUP && btLum2 > 0)
			btLum2--;
		scrLum.SetScrollPos(btLum2, TRUE);
	} 
	updateColor2Display();
}

void dlgSelectBkColors::updateColor2Display() {
	
	myHsl.HSL2RGB(prWin2.btRed, prWin2.btGreen, prWin2.btBlue, btHue, btSat2, btLum2);	
	prWin2.Invalidate();

}

void dlgSelectBkColors::selectSat() {	

	selectionType = SAT;
	initializeColor2();
	updateColor2Display();
		
	scrSat.SetScrollPos(btSat2);	
	scrSat.EnableScrollBar();
	scrLum.EnableScrollBar(ESB_DISABLE_BOTH);
}
void dlgSelectBkColors::selectLum() {	

	selectionType = LUM;
	initializeColor2(); 
	updateColor2Display();

	scrLum.SetScrollPos(btLum2);	
	scrLum.EnableScrollBar();
	scrSat.EnableScrollBar(ESB_DISABLE_BOTH);
}

void dlgSelectBkColors::initializeColor2() {
	btSat2 = btSat1;
	btLum2 = btLum1;

}

void dlgSelectBkColors::changeColor1() {
	// The code below uses CColorDialog::m_cc data member to 
	// customize the settings of CColorDialog. The CColorDialog will
	// be shown as full open and the cuurently selected color will be passed to it
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = RGB(btRed1, btGreen1, btBlue1);

	if(dlg.DoModal()==IDOK) {
		COLORREF clr=dlg.GetColor();

		prWin1.btRed = btRed1 = GetRValue(clr);
		prWin1.btGreen = btGreen1 = GetGValue(clr);
		prWin1.btBlue = btBlue1 = GetBValue(clr);
		prWin1.Invalidate();
	}
}

void dlgSelectBkColors::getSelectedColors(BYTE &outBtRed1,BYTE  &outBtGreen1,BYTE  &outBtBlue1, 
	COLOR_SELECTION_TYPE &outSelectionType, BYTE &outBtSatLumVal) {
	
	outBtRed1 = btRed1;
	outBtGreen1 = btGreen1;
	outBtBlue1 = btBlue1;	
	
	outSelectionType = selectionType;

//	if(blnColor2Selected && (btSat1 != btSat2 || btLum1 != btLum2)) {
		if(outSelectionType == SAT)
			outBtSatLumVal = btSat2;
		else if(outSelectionType == LUM)
			outBtSatLumVal = btLum2;
//	} else 
		//outBtSatLumVal = 0;		
}

BEGIN_MESSAGE_MAP(dlgSelectBkColors, CDialog)
	ON_WM_VSCROLL()
	ON_COMMAND(IDC_BUTTON1, defineColorRange)
	ON_COMMAND(IDC_BUTTON3, changeColor1)
	ON_COMMAND(IDC_RADIO1,  selectLum)
	ON_COMMAND(IDC_RADIO2,  selectSat)
	ON_COMMAND(IDC_RADIO3,  selectClr)
	ON_COMMAND(IDC_RADIO4,  selectBrightClrs)
	ON_COMMAND(IDC_RADIO5,  selectDarkClrs)
END_MESSAGE_MAP()
/***************************Class used by dlgSelectBkColors********************************/
void previewWindow1::OnPaint() {
		
	CPaintDC d(this);
	CRect r;
	
	GetClientRect(&r);	

	if(blnShowColor) {
		//Background
		CBrush pBrush1;
		pBrush1.CreateSolidBrush(RGB(btRed, btGreen, btBlue));
		d.FillRect(r, &pBrush1);
	}
	
	if(blnShowEnabled) 
		d.DrawFocusRect(r);	
}

BEGIN_MESSAGE_MAP(previewWindow1, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


/****************************Dialog to specify camera parameters*****************************/
dlgGetCameraInfo::dlgGetCameraInfo(int n,CAMERA_INFO inCameraInfo, 
								   BOOL inBlnSetParamsOnly) : CDialog(n) {

	outCameraInfo = inCameraInfo;
	blnSetParamsOnly = inBlnSetParamsOnly;

}

int dlgGetCameraInfo::OnInitDialog() {
	
	CDialog::OnInitDialog();
	
	//Get the handle to all the dialog items
	stHtFrame= (CStatic *)GetDlgItem(IDC_STATIC2); 
	stHtText1= (CStatic *)GetDlgItem(IDC_STATIC3);
	stHtText2= (CStatic *)GetDlgItem(IDC_STATIC4);
	stHtText3= (CStatic *)GetDlgItem(IDC_STATIC5);
	stHtText4= (CStatic *)GetDlgItem(IDC_STATIC6);
	stWtFrame= (CStatic *)GetDlgItem(IDC_STATIC8);
	stWtText1= (CStatic *)GetDlgItem(IDC_STATIC28);
	stWtText2= (CStatic *)GetDlgItem(IDC_STATIC29);
	stWtText3= (CStatic *)GetDlgItem(IDC_STATIC30);
	stWtText4= (CStatic *)GetDlgItem(IDC_STATIC7);
	stInfinityHt= (CStatic *)GetDlgItem(IDC_STATIC32);
	stInfinityWt= (CStatic *)GetDlgItem(IDC_STATIC31);
	
	editHtD1= (CEdit *)GetDlgItem(IDC_EDIT1);
	editHt1= (CEdit *)GetDlgItem(IDC_EDIT2);
	editHtD2= (CEdit *)GetDlgItem(IDC_EDIT3);
	editHt2= (CEdit *)GetDlgItem(IDC_EDIT4);
	editWtD1= (CEdit *)GetDlgItem(IDC_EDIT10);
	editWt1= (CEdit *)GetDlgItem(IDC_EDIT11);
	editWtD2= (CEdit *)GetDlgItem(IDC_EDIT12);
	editWt2= (CEdit *)GetDlgItem(IDC_EDIT13);
	editInfinityHt= (CEdit *)GetDlgItem(IDC_EDIT14);
	editInfinityWt= (CEdit *)GetDlgItem(IDC_EDIT15);

	if(blnSetParamsOnly) 
		((CButton *)GetDlgItem(IDOK))->SetWindowText("OK");
	
	/*editHtD1->LimitText(8);
	editHt1->LimitText(8);
	editHtD2->LimitText(8);
	editHt2->LimitText(8);
	editWtD1->LimitText(8);
	editWt1->LimitText(8);
	editWtD2->LimitText(8);
	editWt2->LimitText(8);
	editInfinityHt->LimitText(8);
	editInfinityWt->LimitText(8);*/
		
	chkAtInfinity= (CButton *)GetDlgItem(IDC_CHECK1);
	chkWtSameAsHt= (CButton *)GetDlgItem(IDC_CHECK2);	

	if(outCameraInfo.blnWtSameAsHt)
		chkWtSameAsHt->SetCheck(TRUE);
	else
		chkWtSameAsHt->SetCheck(FALSE);
	
	if(outCameraInfo.blnCameraAtInfinity)
		chkAtInfinity->SetCheck(TRUE);
	else
		chkAtInfinity->SetCheck(FALSE);

	updtStatus_Infinity();//Enable/Disable controls 

	return TRUE;
}

void dlgGetCameraInfo::OnCancel() {
	CDialog::OnCancel();
}

void dlgGetCameraInfo::OnOK() {
	outCameraInfo.blnDataInitialized = true;
	CDialog::OnOK();
}

void dlgGetCameraInfo::loadFromFile() {
	
	CFileDialog fd(1, 0, 0, 0, "Camera Parameters(*.cam)|*.cam|Camera Parameters(*.cam)|*.cam||");
	fd.m_ofn.lpstrTitle = "Load Camera Parameters from file";
	fd.m_ofn.Flags = fd.m_ofn.Flags | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	
	if(fd.DoModal() != IDOK) 
		return;

	CString strPath = fd.GetPathName();
	CFile fp;
	if(!fp.Open(strPath, CFile::modeRead)) {
		MessageBox("Error: Could not open file: \n'" + strPath + "'\n for reading.");
		return ;
	}

	if(fp.GetLength() != 42) {
		MessageBox("Error in reading from file: Invalid format.");
		return ;
	}

	BYTE btCameraAtInfinity, btWtSameAsHt;
	CAMERA_INFO backUpInfo = outCameraInfo;
	
	try {
		
		if(fp.Read(&btCameraAtInfinity, 1) != 1)
			throw new CException; 
		if(fp.Read(&btWtSameAsHt, 1) != 1) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltInfinityHt, 4) != 4) 
			throw new CException;		
		if(fp.Read(&outCameraInfo.fltInfinityWt, 4)!= 4) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltHtDistance1, 4)!= 4) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltHtDistance2, 4)!= 4) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltHeight1, 4)!= 4) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltHeight2,4)!= 4) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltWtDistance1, 4)!= 4) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltWtDistance2, 4)!= 4) 
			throw new CException;
		if(fp.Read(&outCameraInfo.fltWidth1, 4)!= 4)  
			throw new CException;
		if(fp.Read(&outCameraInfo.fltWidth2, 4)!= 4)  
			throw new CException;
	
	} catch(CException *e) {
		e->Delete();				
		outCameraInfo =backUpInfo;
		MessageBox("Error in reading from file: Invalid format.");
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();
		return;		
	}
	
	btCameraAtInfinity == 1? outCameraInfo.blnCameraAtInfinity = true : 
		outCameraInfo.blnCameraAtInfinity = false;
	btWtSameAsHt == 1? outCameraInfo.blnWtSameAsHt = true : 
		outCameraInfo.blnWtSameAsHt = false;
		
	if(outCameraInfo.blnWtSameAsHt) 
		chkWtSameAsHt->SetCheck(TRUE);
	else
		chkWtSameAsHt->SetCheck(FALSE);
	
	if(outCameraInfo.blnCameraAtInfinity)
		chkAtInfinity->SetCheck(TRUE);
	else
		chkAtInfinity->SetCheck(FALSE);
		
	//Update the values in the text fields
	CString str;
	if(outCameraInfo.blnCameraAtInfinity) {

		str.Format("%f", outCameraInfo.fltInfinityHt);
		editInfinityHt->SetWindowText(str);
		str.Format("%f", outCameraInfo.fltInfinityWt);
		editInfinityWt->SetWindowText(str);
	
	} else {
		
		str.Format("%f", outCameraInfo.fltHtDistance1);
		editHtD1->SetWindowText(str);
		str.Format("%f", outCameraInfo.fltHeight1);
		editHt1->SetWindowText(str);
		str.Format("%f", outCameraInfo.fltHtDistance2);
		editHtD2->SetWindowText(str);
		str.Format("%f", outCameraInfo.fltHeight2);
		editHt2->SetWindowText(str);

		if(!outCameraInfo.blnWtSameAsHt) {
			str.Format("%f", outCameraInfo.fltWtDistance1);
			editWtD1->SetWindowText(str);
			str.Format("%f", outCameraInfo.fltWidth1);
			editWt1->SetWindowText(str);
			str.Format("%f", outCameraInfo.fltWtDistance2);
			editWtD2->SetWindowText(str);
			str.Format("%f", outCameraInfo.fltWidth2);
			editWt2->SetWindowText(str);
		}
	}
	
	updtStatus_Infinity();//Enable/Disable controls 

	MessageBox("Camera Parameters successfully loaded from file.");
}

void dlgGetCameraInfo::DoDataExchange(CDataExchange *pdx){
	CDialog::DoDataExchange(pdx);

	if(outCameraInfo.blnCameraAtInfinity) {
		DDX_Text(pdx, IDC_EDIT14, outCameraInfo.fltInfinityHt);
		DDV_MinMaxFloat(pdx, outCameraInfo.fltInfinityHt, 1.00f, 50000.00f);
		
		DDX_Text(pdx, IDC_EDIT15, outCameraInfo.fltInfinityWt);
		DDV_MinMaxFloat(pdx, outCameraInfo.fltInfinityWt, 1.00f, 50000.00f);

	} else {
		DDX_Text(pdx, IDC_EDIT1, outCameraInfo.fltHtDistance1);
		DDV_MinMaxFloat(pdx, outCameraInfo.fltHtDistance1, 0.0001f, 50000.00f);

		DDX_Text(pdx, IDC_EDIT2, outCameraInfo.fltHeight1);
		DDV_MinMaxFloat(pdx, outCameraInfo.fltHeight1, 0.0001f, 50000.00f);

		DDX_Text(pdx, IDC_EDIT3, outCameraInfo.fltHtDistance2);
		DDV_MinMaxFloat(pdx, outCameraInfo.fltHtDistance2, 0.0001f, 50000.00f);

		DDX_Text(pdx, IDC_EDIT4, outCameraInfo.fltHeight2);
		DDV_MinMaxFloat(pdx, outCameraInfo.fltHeight2, 0.0001f, 50000.00f);

		if(!outCameraInfo.blnWtSameAsHt) {
			DDX_Text(pdx, IDC_EDIT10, outCameraInfo.fltWtDistance1);
			DDV_MinMaxFloat(pdx, outCameraInfo.fltWtDistance1, 0.0001f, 50000.00f);

			DDX_Text(pdx, IDC_EDIT11, outCameraInfo.fltWidth1);
			DDV_MinMaxFloat(pdx, outCameraInfo.fltWidth1, 0.0001f, 50000.00f);

			DDX_Text(pdx, IDC_EDIT12, outCameraInfo.fltWtDistance2);
			DDV_MinMaxFloat(pdx, outCameraInfo.fltWtDistance2, 0.0001f, 50000.00f);

			DDX_Text(pdx, IDC_EDIT13, outCameraInfo.fltWidth2);
			DDV_MinMaxFloat(pdx, outCameraInfo.fltWidth2, 0.0001f, 50000.00f);
		}
	}
}

void dlgGetCameraInfo::updtStatus_Infinity() {
	
	if(chkAtInfinity->GetCheck()) {
	
		if(!outCameraInfo.blnCameraAtInfinity) {
			//Status has changed, so initialize values
			editInfinityHt->SetWindowText("1");
			editInfinityWt->SetWindowText("1");
			outCameraInfo.blnCameraAtInfinity = true;
		}

		//Enable/Disable controls
		stInfinityHt->EnableWindow(TRUE);
		stInfinityWt->EnableWindow(TRUE);

		editInfinityHt->EnableWindow(TRUE);
		editInfinityWt->EnableWindow(TRUE);

		stHtFrame->EnableWindow(FALSE); 
		stHtText1->EnableWindow(FALSE);
		stHtText2->EnableWindow(FALSE);
		stHtText3->EnableWindow(FALSE);
		stHtText4->EnableWindow(FALSE);
		stWtFrame->EnableWindow(FALSE);
		stWtText1->EnableWindow(FALSE);
		stWtText2->EnableWindow(FALSE);
		stWtText3->EnableWindow(FALSE);
		stWtText4->EnableWindow(FALSE);

		editHtD1->EnableWindow(FALSE);
		editHt1->EnableWindow(FALSE);
		editHtD2->EnableWindow(FALSE);
		editHt2->EnableWindow(FALSE);
		editWtD1->EnableWindow(FALSE);
		editWt1->EnableWindow(FALSE);
		editWtD2->EnableWindow(FALSE);
		editWt2->EnableWindow(FALSE);
		
		chkWtSameAsHt->EnableWindow(FALSE);

	} else {

		if(outCameraInfo.blnCameraAtInfinity) {
			//Status has changed, so initialize values
			editWtD1->SetWindowText("1");
			editWt1->SetWindowText("1");
			editWtD2->SetWindowText("1");
			editWt2->SetWindowText("1");
			editHtD1->SetWindowText("1");
			editHt1->SetWindowText("1");
			editHtD2->SetWindowText("1");
			editHt2->SetWindowText("1");
			
			outCameraInfo.blnCameraAtInfinity = false;
		}
		
		//Enable/Disable controls
		stInfinityHt->EnableWindow(FALSE);
		stInfinityWt->EnableWindow(FALSE);

		editInfinityHt->EnableWindow(FALSE);
		editInfinityWt->EnableWindow(FALSE);

		stHtFrame->EnableWindow(TRUE); 
		stHtText1->EnableWindow(TRUE);
		stHtText2->EnableWindow(TRUE);
		stHtText3->EnableWindow(TRUE);
		stHtText4->EnableWindow(TRUE);
		
		editHtD1->EnableWindow(TRUE);
		editHt1->EnableWindow(TRUE);
		editHtD2->EnableWindow(TRUE);
		editHt2->EnableWindow(TRUE);

		chkWtSameAsHt->EnableWindow(TRUE);

		updtStatus_DiffWidth(); //Enable/Disable controls for horizontal coverage 

	}

}

void dlgGetCameraInfo::updtStatus_DiffWidth() {
	

	if(chkWtSameAsHt->GetCheck()) {
		outCameraInfo.blnWtSameAsHt = true;

		//Enable/Disable controls
		stWtFrame->EnableWindow(FALSE);
		stWtText1->EnableWindow(FALSE);
		stWtText2->EnableWindow(FALSE);
		stWtText3->EnableWindow(FALSE);
		stWtText4->EnableWindow(FALSE);

		editWtD1->EnableWindow(FALSE);
		editWt1->EnableWindow(FALSE);
		editWtD2->EnableWindow(FALSE);
		editWt2->EnableWindow(FALSE);

	} else {
		
		if(outCameraInfo.blnWtSameAsHt) {
			
			//Status has changed so initialize values
			editWtD1->SetWindowText("1");
			editWt1->SetWindowText("1");
			editWtD2->SetWindowText("1");
			editWt2->SetWindowText("1");

			outCameraInfo.blnWtSameAsHt = false;	
		}

		//Enable/Disable controls
		stWtFrame->EnableWindow(TRUE);
		stWtText1->EnableWindow(TRUE);
		stWtText2->EnableWindow(TRUE);
		stWtText3->EnableWindow(TRUE);
		stWtText4->EnableWindow(TRUE);

		editWtD1->EnableWindow(TRUE);
		editWt1->EnableWindow(TRUE);
		editWtD2->EnableWindow(TRUE);
		editWt2->EnableWindow(TRUE);
	}
}

BEGIN_MESSAGE_MAP(dlgGetCameraInfo, CDialog)
	ON_COMMAND(IDC_CHECK1, updtStatus_Infinity)	
	ON_COMMAND(IDC_CHECK2, updtStatus_DiffWidth)	
	ON_COMMAND(IDC_BUTTON1, loadFromFile)		
END_MESSAGE_MAP()

/*****************************Dialog class to specify the range of mold points for the model*/

pointRangeDlg::pointRangeDlg(int n, UINT inIntXPts, UINT inIntYPts, UINT inIntZPts) : 
	CDialog(n) {
	  
	  intXPts = intMaxx = inIntXPts;
	  intYPts = intMaxy = inIntYPts;   
	  intZPts = intMaxz = inIntZPts;

	  intMinx = intMiny = intMinz = 1;
	  
	  blnProcessAllX = blnProcessAllY = blnProcessAllZ = true;
}

int pointRangeDlg::OnInitDialog() {

	CDialog::OnInitDialog();
	
	//Set process all as default
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(true);
	((CButton *)GetDlgItem(IDC_RADIO4))->SetCheck(true);
	((CButton *)GetDlgItem(IDC_RADIO7))->SetCheck(true);
		
	edit1 = (CEdit *)GetDlgItem(IDC_EDIT1);
	edit10 = (CEdit *)GetDlgItem(IDC_EDIT10);
	edit15 = (CEdit *)GetDlgItem(IDC_EDIT15);
	edit16 = (CEdit *)GetDlgItem(IDC_EDIT16);
	edit17 = (CEdit *)GetDlgItem(IDC_EDIT17);
	edit18 = (CEdit *)GetDlgItem(IDC_EDIT18);		

	edit1->SetWindowText("1");
	edit15->SetWindowText("1");
	edit17->SetWindowText("1");

	CString str;
	str.Format("%d", intXPts);
	((CStatic *)GetDlgItem(IDC_STATIC3))->SetWindowText(str);
	edit10->SetWindowText(str);
	str.Format("%d", intYPts);
	((CStatic *)GetDlgItem(IDC_STATIC6))->SetWindowText(str);
	edit16->SetWindowText(str);
	str.Format("%d", intZPts);
	((CStatic *)GetDlgItem(IDC_STATIC9))->SetWindowText(str);
	edit18->SetWindowText(str);
	
	st4 = (CStatic *)GetDlgItem(IDC_STATIC4);
	st5 = (CStatic *)GetDlgItem(IDC_STATIC5);
	
	st30 = (CStatic *)GetDlgItem(IDC_STATIC30);
	st29 = (CStatic *)GetDlgItem(IDC_STATIC29);
	
	st42 = (CStatic *)GetDlgItem(IDC_STATIC42);
	st41 = (CStatic *)GetDlgItem(IDC_STATIC41);

	setControlsX(false);
	setControlsY(false);
	setControlsZ(false);
	
	return TRUE;

}

void pointRangeDlg::DoDataExchange(CDataExchange *pdx) {
	
	CDialog::DoDataExchange(pdx);

	if(!blnProcessAllX) {
		DDX_Text(pdx, IDC_EDIT1, intMinx);
		DDV_MinMaxInt(pdx, intMinx, 1, intXPts);
		DDX_Text(pdx, IDC_EDIT10, intMaxx);
		DDV_MinMaxInt(pdx, intMaxx, 1, intXPts);		
	} 
	
	if(!blnProcessAllY) {
		DDX_Text(pdx, IDC_EDIT15, intMiny);
		DDV_MinMaxInt(pdx, intMiny, 1, intYPts);
		DDX_Text(pdx, IDC_EDIT16, intMaxy);
		DDV_MinMaxInt(pdx, intMaxy, 1, intYPts);			
	}

	if(!blnProcessAllZ) {
		DDX_Text(pdx, IDC_EDIT17, intMinz);
		DDV_MinMaxInt(pdx, intMinz, 1, intZPts);
		DDX_Text(pdx, IDC_EDIT18, intMaxz);
		DDV_MinMaxInt(pdx, intMaxz, 1, intZPts);		
	}	
}

BOOL pointRangeDlg::validateRange() {
	//This function checks for at least 2 points per axes.
	//It throws an error message, if the point range is invalid.
	//If it encounters an error that can be handled by DoDataExchange it returns true
	//leaving it on DoDataExchange to handle it.
	CString str;
	if(!blnProcessAllX) {
		edit1->GetWindowText(str);
		int intX1 = atoi(str);
		if(intX1 <= 0)
			return true;
		edit10->GetWindowText(str);
		int intX2 = atoi(str);
		if(intX2 <= 0)
			return true;
		if( (intX2 - intX1) < 2 ) {
			MessageBox("X2 should be greater than X1 by at least 2.", "Error:");
			edit10->SetSel(0, str.GetLength());
			edit10->SetFocus();
			return false;
		}
	}
	if(!blnProcessAllY) {
		edit15->GetWindowText(str);
		int intY1 = atoi(str);
		if(intY1 <= 0)
			return true;
		edit16->GetWindowText(str);
		int intY2 = atoi(str);
		if(intY2 <= 0)
			return true;
		if( (intY2 - intY1) < 2 ) {
			MessageBox("Y2 should be greater than Y1 by at least 2.", "Error:");
			edit16->SetSel(0, str.GetLength());
			edit16->SetFocus();
			return false;
		}
	}
	
	if(!blnProcessAllZ) {
		edit17->GetWindowText(str);
		int intZ1 = atoi(str);
		if(intZ1 <= 0)
			return true;
		edit18->GetWindowText(str);
		int intZ2 = atoi(str);
		if(intZ2 <= 0)
			return true;
		if( (intZ2 - intZ1) < 2 ) {
			MessageBox("Z2 should be greater than Z1 by at least 2.", "Error:");
			edit18->SetSel(0, str.GetLength());
			edit18->SetFocus();
			return false;
		}		
	}
	return true;
}
void pointRangeDlg::OnOK() {
	//Do the validations that are not possible through DoDataExchange
	if(!validateRange())
		return;
	
	CDialog::OnOK();
}
void pointRangeDlg::getPointRange(UINT &outIntMinx, UINT &outIntMaxx, UINT &outIntMiny, 
		UINT &outIntMaxy, UINT &outIntMinz, UINT &outIntMaxz) {

	//If the specified range is max, then it's equivalent to processing all
	if(intMinx == 1 && intMaxx == intXPts)
		blnProcessAllX = true;
	if(intMiny == 1 && intMaxy == intYPts)
		blnProcessAllY = true;
	if(intMinz == 1 && intMaxz == intZPts)
		blnProcessAllZ = true;
	//---------------------------------------------------------------------

	outIntMinx= blnProcessAllX? 0 : intMinx;  
	outIntMaxx= blnProcessAllX? 0 : intMaxx;
	outIntMiny= blnProcessAllY? 0 : intMiny;
	outIntMaxy= blnProcessAllY? 0 : intMaxy;
	outIntMinz= blnProcessAllZ? 0 : intMinz;
	outIntMaxz= blnProcessAllZ? 0 : intMaxz;
}

void pointRangeDlg::setControlsX(BOOL blnStatus) {
	st4->EnableWindow(blnStatus);
	st5->EnableWindow(blnStatus);
	edit1->EnableWindow(blnStatus);
	edit10->EnableWindow(blnStatus);
}

void pointRangeDlg::setControlsY(BOOL blnStatus) {
	st30->EnableWindow(blnStatus);
	st29->EnableWindow(blnStatus);
	edit15->EnableWindow(blnStatus);
	edit16->EnableWindow(blnStatus);
}
void pointRangeDlg::setControlsZ(BOOL blnStatus) {
	st42->EnableWindow(blnStatus);
	st41->EnableWindow(blnStatus);
	edit17->EnableWindow(blnStatus);
	edit18->EnableWindow(blnStatus);
}


void pointRangeDlg::setProcessAllX() {
	setControlsX(false);
	blnProcessAllX = true;
}
void pointRangeDlg::setPointRangeX() {
	setControlsX(true);
	blnProcessAllX = false;

}
void pointRangeDlg::setProcessAllY() {
	setControlsY(false);
	blnProcessAllY = true;
}
void pointRangeDlg::setPointRangeY() {
	setControlsY(true);
	blnProcessAllY = false;
}
void pointRangeDlg::setProcessAllZ() {
	setControlsZ(false);
	blnProcessAllZ = true;
}
void pointRangeDlg::setPointRangeZ() {
	setControlsZ(true);
	blnProcessAllZ = false;
}

BEGIN_MESSAGE_MAP(pointRangeDlg, CDialog)
	ON_COMMAND(IDC_RADIO1, setProcessAllX)
	ON_COMMAND(IDC_RADIO2, setPointRangeX)
	ON_COMMAND(IDC_RADIO4, setProcessAllY)
	ON_COMMAND(IDC_RADIO6, setPointRangeY)
	ON_COMMAND(IDC_RADIO7, setProcessAllZ)
	ON_COMMAND(IDC_RADIO8, setPointRangeZ)
END_MESSAGE_MAP()
	
//***************Class used to select the model color*****************************************

dlgModelColor::dlgModelColor(int n, RGBCOLOR inClr) : CDialog(n) {
	prWin.btRed = clr.red = inClr.red;
	prWin.btGreen = clr.green = inClr.green;
	prWin.btBlue = clr.blue = inClr.blue;

	prWin.blnShowEnabled = true;
	prWin.blnShowColor = true;

}
int dlgModelColor::OnInitDialog() {
	
	CDialog::OnInitDialog();
	
	prWin.CreateEx(WS_EX_WINDOWEDGE, "SplashViewClass", "", WS_CHILD | WS_VISIBLE, 
	   	CRect(23, 12, 150, 108),this, 1); 

	return TRUE;
}

void dlgModelColor::getColorValues(RGBCOLOR &outClr) {
	outClr.red = clr.red;
	outClr.green = clr.green;
	outClr.blue = clr.blue;
}

void dlgModelColor::chooseColor()  {
	// The code below uses CColorDialog::m_cc data member to 
	// customize the settings of CColorDialog. The CColorDialog will
	// be shown as full open and the cuurently selected color will be passed to it
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = RGB(clr.red, clr.green, clr.blue);

	if(dlg.DoModal()==IDOK) {
		COLORREF color=dlg.GetColor();

		prWin.btRed = clr.red = GetRValue(color);
		prWin.btGreen = clr.green = GetGValue(color);
		prWin.btBlue = clr.blue = GetBValue(color);
		prWin.Invalidate();
	}
}

BEGIN_MESSAGE_MAP(dlgModelColor, CDialog)
	ON_COMMAND(IDC_BUTTON1, chooseColor)
END_MESSAGE_MAP()


//***************Dialog class to change the rotation speed of the model***********************
dlgModelRtnSpeed::dlgModelRtnSpeed(int n, BYTE inBtSeconds) : CDialog(n) {
	btSeconds = inBtSeconds;
}
int dlgModelRtnSpeed::OnInitDialog() {
	CDialog::OnInitDialog();
	list = (CListBox *)GetDlgItem(IDC_LIST1);

	list->AddString("100");
	list->AddString("200");
	list->AddString("300");
	list->AddString("400");
	list->AddString("500");

	list->SetCurSel(btSeconds - 1);

	return TRUE;
}

void dlgModelRtnSpeed::OnOK() { 
	btSeconds = list->GetCurSel() + 1;
	CDialog::OnOK();
}
BYTE dlgModelRtnSpeed::getSeconds() {
	return btSeconds;
}

//***************Dialog class to change the view distance while viewing the model************
dlgModelViewDt::dlgModelViewDt(int n, float inFltViewDt, float inFltMaxCoodVal) : 
	CDialog(n) {
	
	//Round off all input values to two decimal values
	fltViewDt = ((float)(int)(inFltViewDt * 100.0f))/100.0f;
	fltMaxCoodVal = ((float)(int)(inFltMaxCoodVal * 100.0f))/100.0f;

	/*
	Note: Rounding off values is really important. It would be better to do this for all
	inputs to a dialog. Here, I was facing a problem in the DDV_MinMaxFloat routine which was
	throwing an error of "Enter a number between 12 and .." even if 12 is entered.  Actually
	the variable fltMaxCoodVal was storing 12 as 12.0000087 internally.
	*/


}
/*
int dlgModelViewDt::OnInitDialog()	{
	return TRUE;
}
*/
void dlgModelViewDt::DoDataExchange(CDataExchange *pdx)	 {
	CDialog::DoDataExchange(pdx);	
		
	DDX_Text(pdx, IDC_EDIT1, fltViewDt);
	fltMaxCoodVal = fltMaxCoodVal;
	DDV_MinMaxFloat(pdx, fltViewDt, fltMaxCoodVal, MODEL_FLT_MAX_VIEW_DISTANCE);

}

float dlgModelViewDt::getViewDt()	{ 
	//Return the value rounded to two decimal places
	return ((float)(int)(fltViewDt* 100.0f))/100.0f;
 }

void dlgModelViewDt::setDefault() {

	CString str;
	str.Format("%f", fltMaxCoodVal * MODEL_VIEW_DISTANCE_MULTIPLIER);
	
	((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText(str);	
}

BEGIN_MESSAGE_MAP(dlgModelViewDt, CDialog)
	ON_COMMAND(IDC_BUTTON1, setDefault)
END_MESSAGE_MAP()

//***************************Dialog class to show model info*********************************
dlgModelInfo::dlgModelInfo(int n, MODEL_INFO inModelInfo) : CDialog(n) {
	modelInfo = inModelInfo;
}

int dlgModelInfo::OnInitDialog() {
	CDialog::OnInitDialog();
	
	CString str1;
	((CEdit *)GetDlgItem(IDC_EDIT1))->SetWindowText(modelInfo.strFilePath);
	str1.Format("%d bytes", modelInfo.intFileSize);
	((CStatic *)GetDlgItem(IDC_STATIC4))->SetWindowText(str1);
	str1.Format("Comprises of %d triangles.", modelInfo.intTotTriangles);
	((CStatic *)GetDlgItem(IDC_STATIC6))->SetWindowText(str1);
	str1.Format("Takes up %d bytes to load.", modelInfo.intTotTriangles * 72);
	((CStatic *)GetDlgItem(IDC_STATIC7))->SetWindowText(str1);
	str1.Format("X: %.2f to %.2f", modelInfo.fltMinx, modelInfo.fltMaxx);
	((CStatic *)GetDlgItem(IDC_STATIC9))->SetWindowText(str1);
	str1.Format("Y: %.2f to %.2f", modelInfo.fltMiny, modelInfo.fltMaxy);
	((CStatic *)GetDlgItem(IDC_STATIC10))->SetWindowText(str1);
	str1 = "";
	str1.Format("Z: %.2f  to %.2f", modelInfo.fltMinz, modelInfo.fltMaxz);
	((CStatic *)GetDlgItem(IDC_STATIC11))->SetWindowText(str1);
	str1.Format("%f", modelInfo.fltViewDistance);
	((CStatic *)GetDlgItem(IDC_STATIC14))->SetWindowText(str1);
	((CEdit *)GetDlgItem(IDC_EDIT2))->SetWindowText("Current View: " 
		+ modelInfo.strCurrentView);

	return TRUE;
}
//***************************Dialog class to show keyboard shortcuts**************************
dlgKeyboardMap::dlgKeyboardMap(int n, KEYBOARD_MAP_TYPE inMapType) : CDialog(n) {
	mapType = inMapType;
}

int dlgKeyboardMap::OnInitDialog() {
	CDialog::OnInitDialog();
	list.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect(20, 20, 260, 250), this, 1);
	
	//Add the columns
	list.InsertColumn(0,"Function", LVCFMT_LEFT, 150);
	list.InsertColumn(1,"Key", LVCFMT_LEFT, 90);
	int intCtr;

	CString strList[9][2] = { "View Model Info", "Ctrl + I", 
		"Toggle Rotation", "R", "Toggle Rotation Axis", "A", 
		"Change Rotation Speed", "Ctrl + R",
		"Toggle Axis Display", "D", "Decrease View Distance", "Up Arrow", 
		"Increase View Distance", "Down Arrow", 
		"Keyboard Help", "Ctrl + K",
		"Help", "F1"
	};

	switch(mapType) {
	case MODEL:		
		for(intCtr = 0; intCtr < 9; intCtr++) {
			list.InsertItem(intCtr, strList[intCtr][0], intCtr);
			list.SetItemText(intCtr, 1, strList[intCtr][1]);
		}
		break;
	case MOLD:
		CString strList1[15][2] = { 
			"Run Script", "Alt + R",
			"Load Image", "Ctrl + O",
			"Save Image", "Ctrl + S",
			"Save Image As", "Ctrl + Shift + S",
			"Create Model", "Ctrl + M",
			"Close Mold File", "Ctrl + F4",
			"Undo", "Ctrl + Z", 
			"Redo", "Ctrl + Y",
			"Set Background", "S",
			"Set Region As Background", "Ctrl + B",
			"Set Color As Background", "Ctrl + C",
			"Apply Image To Mold", "Ctrl + A",
  		    "Close Image File", "Ctrl + Shift+F4",
			"Keyboard Help", "Ctrl + K",
			"Help", "F1"
		};
		
		for(intCtr = 0; intCtr < 15; intCtr++) {
			list.InsertItem(intCtr, strList1[intCtr][0], intCtr);
			list.SetItemText(intCtr, 1, strList1[intCtr][1]);
		}
		break;
	}
	
	return TRUE;
}
	
/**************Dialog class to alter the number of points along the X and Y ****************/
/***************************** axes for the slides option **********************************/

alterPointsDlg::alterPointsDlg(int n, UINT inUintCurrentX, UINT inUintCurrentY, 
	UINT inUintImgWidth, UINT inUintImgHeight) : CDialog (n) {

	c_uintCurrentX = inUintCurrentX;
	c_uintCurrentY = inUintCurrentY;
	c_uintImgWidth = inUintImgWidth;
	c_uintImgHeight = inUintImgHeight;
}

int alterPointsDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	combo.CreateEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | 
		CBS_DROPDOWNLIST | WS_VSCROLL, CRect(63, 163, 98, 450), this, 50);

	st4 = (CStatic *)(GetDlgItem(IDC_STATIC4));
	st7 = (CStatic *)(GetDlgItem(IDC_STATIC7));
	st8 = (CStatic *)(GetDlgItem(IDC_STATIC8));
	st9 = (CStatic *)(GetDlgItem(IDC_STATIC9));
	st10 = (CStatic *)(GetDlgItem(IDC_STATIC10));
	st11 = (CStatic *)(GetDlgItem(IDC_STATIC11));
	st13 = (CStatic *)(GetDlgItem(IDC_STATIC13));
	st14 = (CStatic *)(GetDlgItem(IDC_STATIC14));

	CString str;
	str.Format("X = %d, Y = %d",c_uintCurrentX ,c_uintCurrentY);
	st4->SetWindowText(str);
	str.Format("Image width: %d pixels", c_uintImgWidth);
	st8->SetWindowText(str);
	str.Format("Image height: %d  pixels", c_uintImgHeight);
	st9->SetWindowText(str);
	
	edit1 = (CEdit *)(GetDlgItem(IDC_EDIT1));
	edit2 = (CEdit *)(GetDlgItem(IDC_EDIT2));
	
	str.Format("%d", c_uintCurrentX);
	edit1->SetWindowText(str);
	str.Format("%d", c_uintCurrentY);
	edit2->SetWindowText(str);
	
	edit1->LimitText(4);
	edit2->LimitText(4);
	
	UINT uintMaxVal = __max(c_uintImgWidth, c_uintImgHeight);

	if(uintMaxVal + 1 <=  UINT_MAX_AXES_POINTS) {
		combo.AddString("1");
		
		if(uintMaxVal * 2 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("2");
		if(uintMaxVal * 3 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("3");
		if(uintMaxVal * 4 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("4");
		if(uintMaxVal * 5 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("5");
		if(uintMaxVal * 6 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("6");
		if(uintMaxVal * 7 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("7");
		if(uintMaxVal * 8 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("8");
		if(uintMaxVal * 9 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.AddString("9");
		
		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(true);
		setControls(2);
		if(uintMaxVal * 2 + 1 <=  UINT_MAX_AXES_POINTS) 
			combo.SetCurSel(1);
		else
			combo.SetCurSel(0);
		onChangeOfN();

	} else {
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(true);
		setControls(1);
		(CButton *)(GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);		
	}
	return TRUE;
}
void alterPointsDlg::DoDataExchange(CDataExchange *pdx) {
	
	CDialog::DoDataExchange(pdx);	
	
	if(selectionType == THIRD) {
		DDX_Text(pdx, IDC_EDIT1, c_uintNewX);
		DDV_MinMaxUInt(pdx, c_uintNewX, 2,  UINT_MAX_AXES_POINTS);
		
		DDX_Text(pdx, IDC_EDIT2, c_uintNewY);
		DDV_MinMaxUInt(pdx, c_uintNewY, 2,  UINT_MAX_AXES_POINTS);		
	}
}

void alterPointsDlg::getNewPoints(UINT &outUintX, UINT  &outUintY) {
	
	if(selectionType == FIRST) {
		c_uintNewX = c_uintCurrentX;
		c_uintNewY = c_uintCurrentY;
	} 
	
	outUintX = c_uintNewX;
	outUintY = c_uintNewY;
}
void alterPointsDlg::setControls_1() {
	setControls(1);
}

void alterPointsDlg::setControls_2() {
	setControls(2);
}

void alterPointsDlg::setControls_3() {
	setControls(3);
}

void alterPointsDlg::setControls(BYTE inBtOption) {
	
	if(inBtOption == 1 || inBtOption == 3 ) {
		st7->EnableWindow(FALSE);
		st10->EnableWindow(FALSE);
		st11->EnableWindow(FALSE);
		combo.EnableWindow(FALSE);
	}
	if(inBtOption == 1 || inBtOption == 2) {
		st13->EnableWindow(FALSE);
		st14->EnableWindow(FALSE);
		edit1->EnableWindow(FALSE);
		edit2->EnableWindow(FALSE);
	}
	
	switch(inBtOption) {
	case 1:
		selectionType = FIRST;
		break;
	case 2:
		selectionType = SECOND;
		st7->EnableWindow(TRUE);
		st10->EnableWindow(TRUE);
		st11->EnableWindow(TRUE);
		combo.EnableWindow(TRUE);
		break;
	case 3:
		selectionType = THIRD;
		st13->EnableWindow(TRUE);
		st14->EnableWindow(TRUE);
		edit1->EnableWindow(TRUE);
		edit2->EnableWindow(TRUE);
		break;
	}
}

void alterPointsDlg::onChangeOfN() {
	UINT uintIndex = combo.GetCurSel();
	
	c_uintNewX = (uintIndex +1) * c_uintImgWidth + 1;
	c_uintNewY = (uintIndex +1) * c_uintImgHeight + 1;

	CString str;

	str.Format("X = %d",  c_uintNewX);
	st10->SetWindowText(str);

	str.Format("Y = %d",  c_uintNewY);
	st11->SetWindowText(str);
}


BEGIN_MESSAGE_MAP(alterPointsDlg, CDialog)
	ON_COMMAND(IDC_RADIO1, setControls_1)
	ON_COMMAND(IDC_RADIO2, setControls_2)
	ON_COMMAND(IDC_RADIO3, setControls_3)	
	ON_CBN_SELCHANGE(50, onChangeOfN)		
END_MESSAGE_MAP()
	
