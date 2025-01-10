#include "molddoc.h"
#include "bitmapview.h"
#include "afxwin.h"
#include "mychildwnd.h"
#include "dialogs.h"
#include "myapp.h"
  

IMPLEMENT_DYNCREATE(bitmapView, CScrollView)

BEGIN_MESSAGE_MAP(bitmapView, CScrollView)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()

END_MESSAGE_MAP()

void bitmapView::setNormalCursor() {
	curCursorType = NORMAL_CURSOR;
}

BOOL bitmapView::OnScroll( UINT nScrollCode, UINT nPos, BOOL bDoScroll) {

	curCursorType = NORMAL_CURSOR;

	moldDoc *doc = (moldDoc *) GetDocument();	

	if(doc->getLpPreviousRect() != NULL) { 
		SIZE sz;
		sz.cx = sz.cy = 1;
		CClientDC d(this);	
		d.DrawDragRect(CRect(-1,-1,-1,-1), sz, doc->getLpPreviousRect(), sz, NULL, NULL);
		doc->setLpPreviousRectToNull();
	}

	return CScrollView::OnScroll( nScrollCode, nPos, bDoScroll );
}

//OnPrepareDC function was giving an assertion failure if SetScrollSizes was not called
//before it. Here, a call was going to this function on giving a call to ShowWindow function
//in the OnCreateClient function of myChildWnd class. So had to override the base class.
void bitmapView::OnPrepareDC (CDC* pDC, CPrintInfo* pInfo) {
	moldDoc *doc = (moldDoc *) GetDocument();
	if(doc->isDocLoaded() && doc->isFirstLoad()) {
		doc->resetFirstLoadFlag();
		SetScrollSizes(MM_TEXT, CSize(doc->getWindowWidth(), doc->getWindowHeight()));		
	}

	
	if(doc->isDocLoaded())
		CScrollView::OnPrepareDC(pDC, pInfo);
}

void bitmapView::OnInitialUpdate() {
	//SetScrollSizes(MM_TEXT, CSize(2048, 2048));	
	flgTpCrnrActive = flgBtmCrnrActive = flgLtCrnrActive = flgRtCrnrActive
					= false;
	curCursorType = NORMAL_CURSOR;

	//Load the custom mouse cursors from files
	curRegionMarker = LoadCursorFromFile("mark_region.cur"); 
	curColorMarker = LoadCursorFromFile("mark_color.cur"); 

	CScrollView::OnInitialUpdate();
}

void bitmapView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) {
	
	moldDoc *doc = (moldDoc *) GetDocument();

	if(doc->isNewImage()) {		
		doc->resetNewImageFlag();
	}

	
	if(!doc->isScrollSizeSet()) {
		doc->setScrollSizeFlag();
		
		SetScrollSizes(MM_TEXT, CSize(doc->getWindowWidth(), doc->getWindowHeight()));		
	}
	
	if(doc->isBitmapLoaded() && doc->isPadModified()) {
		
		int intMg     = doc->getMagnification();
		int intWidth  = doc->getImgWidth();
		int intLtPad  = doc->getLtPadding();
		int intRtPad  = doc->getRtPadding();
		int intTpPad  = doc->getTpPadding();
		int intBtmPad = doc->getBtmPadding();		
		int intHeight = doc->getImgHeight();

		tpLeft.x  = btmLeft.x  = IMAGE_LEFT_INDENT + intMg*intLtPad; 
		tpLeft.y  = tpRight.y  = IMAGE_TOP_INDENT  + intMg*intTpPad;
		tpRight.x = btmRight.x = IMAGE_LEFT_INDENT + intMg*(intWidth - intRtPad) + 1;
		btmLeft.y = btmRight.y = IMAGE_TOP_INDENT  + intMg*(intHeight - intBtmPad) + 1;
		
		doc->resetPadModifiedFlag();
	}
	

	CScrollView::OnUpdate(pSender, lHint, pHint);
}

void bitmapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message ) {
	
	if(curCursorType == TOP_BOTTOM_PADDING_CURSOR) {
		HCURSOR h = LoadCursor(NULL, IDC_SIZENS); 
		SetCursor(h);
	} else if (curCursorType == LEFT_RIGHT_PADDING_CURSOR) {
		HCURSOR h = LoadCursor(NULL, IDC_SIZEWE); 
		SetCursor(h);
	} else if (curCursorType == REGION_MARKER_CURSOR) {
		SetCursor(curRegionMarker);
		curCursorType = NORMAL_CURSOR;
	} else if (curCursorType == COLOR_MARKER_CURSOR) {
		SetCursor(curColorMarker);
		curCursorType = NORMAL_CURSOR;
	} else
		CWnd::OnSetCursor(pWnd, nHitTest, message);	
}

void bitmapView::OnDraw(CDC *p) {
	CString str;
	int intMagnification, intWidth, intHeight;
	moldDoc *doc = (moldDoc *) GetDocument();
	if(!doc->isDocLoaded()) 
		return;

	//Erase any previous rectangle formed due to dragging of the mouse
	if(doc->getLpPreviousRect() != NULL) { 		
		SIZE sz;
		sz.cx = sz.cy = 1;		
		p->DrawDragRect(CRect(-1,-1,-1,-1), sz, doc->getLpPreviousRect(), sz, NULL, NULL);
		doc->setLpPreviousRectToNull();
	}

	if(!doc->isBitmapLoaded() ) {
		
		//Show some help strings till the time an image is loaded
		str = "Press Ctrl + O to open a bitmap image to apply it on the mold.";
		p->TextOut(10, 10, str, strlen(str));
		str = "If you have finished processing the mold using";
		p->TextOut(10, 50, str, strlen(str));
		str = "images of the real world object,";
		p->TextOut(10, 70, str, strlen(str));
		str = "press Ctrl + M to create a model from the mold.";
		p->TextOut(10, 90, str, strlen(str));

		str = "Press F1 for help.";
		p->TextOut(10, 130, str, strlen(str));

		return;
	}	
	
	//Draw the Bitmap on to the client area==============
		
	cancelMouseOperations(); //terminate any previous mouse based drawing operation 
	
	//display bitmap
	
	CDC *pDC = doc->getBitmapDC();
	
	//If the bitmap is not selected on the document class CDC object then select it
	if(pDC->m_hDC == NULL) {
		if (!pDC->CreateCompatibleDC(p)) {	
			TRACE0("CreateCompatibleDC failed\n");
			ASSERT(FALSE);
		}
		doc->selectBitmap(pDC);
	}			

	intMagnification = doc->getMagnification();
	intWidth = doc->getImgWidth();
	intHeight = doc->getImgHeight();

	//Copy from the doc memory DC to display DC
	if (!p->StretchBlt(IMAGE_LEFT_INDENT + 1, IMAGE_TOP_INDENT + 1, 
			intMagnification*intWidth, 
			intMagnification*intHeight, 
			pDC, 0, 0, 
			intWidth, intHeight, SRCCOPY))
		TRACE0("BitBlt failed\n");
		

	//Display boundary lines
	COLORREF clr(RGB(0,0,255));
	CPen mypen(PS_SOLID, 1, clr);
	CPen *prevpen = p->SelectObject(&mypen);
	prevpen->DeleteObject();
	p->MoveTo(tpLeft);
	p->LineTo(tpRight);
	p->LineTo(btmRight);
	p->LineTo(btmLeft);
	p->LineTo(tpLeft);		
	
}

void bitmapView::OnRButtonDown(UINT nFlags, CPoint pt) {
	
}

void bitmapView::OnLButtonDown(UINT nFlags, CPoint pt) {
	


	CPoint initialPt, newWinPt, mainWinPt;
	initialPt = pt;
	
	moldDoc *doc = (moldDoc *) GetDocument();

	if(!doc->isBitmapLoaded()) 
		return;

		
	//Change from window to scrollview coordinates--
	CClientDC d(this);
	CScrollView::OnPrepareDC(&d, NULL);
	d.DPtoLP(&pt);//----------------------------------

	//Erase any previous rectangle formed due to dragging of the mouse
	if(doc->getLpPreviousRect() != NULL) { 
		SIZE sz;
		sz.cx = sz.cy = 1;
		d.DrawDragRect(CRect(-1,-1,-1,-1), sz, doc->getLpPreviousRect(), sz, NULL, NULL);
		doc->setLpPreviousRectToNull();
	}

	if ((tpLeft.x <= pt.x && pt.x <= tpRight.x) &&
		(tpLeft.y >= pt.y && pt.y >= tpLeft.y - BORDER_WIDTH)) {
		 
		 flgTpCrnrActive = true;//Top boundary is to be moved
		 //Move mouse to tpLeft.y========== 						 
		 newWinPt = tpLeft;//Scrollview to
		 d.LPtoDP(&newWinPt);//window coordinates
		 if(!GetCursorPos(&mainWinPt)) {//main window cursor pos
			 MessageBox("Error occured while getting cursor position");
			 return;
		 }
		 mainWinPt.y = newWinPt.y + (mainWinPt.y - initialPt.y);//window to main window
		 SetCursorPos(mainWinPt.x, mainWinPt.y);//=====================================
		 
		 curCursorType = TOP_BOTTOM_PADDING_CURSOR;
		 SetCapture(); //Even if the mouse goes out of window, its messages would be captured.
		 
		 prevStrt.x = tpLeft.x;  prevEnd.x = tpRight.x;
		 prevStrt.y = prevEnd.y = tpLeft.y;	
	} else if ((tpLeft.x <= pt.x && pt.x <= tpRight.x) &&
		(btmLeft.y <= pt.y && pt.y <= btmLeft.y + BORDER_WIDTH)) {
		 
		 flgBtmCrnrActive = true;//Bottom boundary is to be moved
			 
		 //Move mouse to btmLeft.y========== 						 
		 newWinPt = btmLeft;//Scrollview to
		 d.LPtoDP(&newWinPt);//window coordinates
		 if(!GetCursorPos(&mainWinPt)) {//main window cursor pos
			 MessageBox("Error occured while getting cursor position");
			 return;
		 }
		 mainWinPt.y = newWinPt.y + (mainWinPt.y - initialPt.y);//window to main window
		 SetCursorPos(mainWinPt.x, mainWinPt.y);//=====================================
		 
		 curCursorType = TOP_BOTTOM_PADDING_CURSOR;
		 SetCapture(); //Even if the mouse goes out of window, its messages would be captured.
		 
		 prevStrt.x = btmLeft.x;  prevEnd.x = btmRight.x;
		 prevStrt.y = prevEnd.y = btmLeft.y;	
	} else if ((tpLeft.y <= pt.y && pt.y <= btmLeft.y) &&
		(btmLeft.x >= pt.x && pt.x >= btmLeft.x - BORDER_WIDTH)) {
		 
		 flgLtCrnrActive = true;//Left boundary is to be moved
			 
		 //Move mouse to btmLeft.x========== 						 
		 newWinPt = btmLeft;//Scrollview to
		 d.LPtoDP(&newWinPt);//window coordinates
		 if(!GetCursorPos(&mainWinPt)) {//main window cursor pos
			 MessageBox("Error occured while getting cursor position");
			 return;
		 }
		 mainWinPt.x = newWinPt.x + (mainWinPt.x - initialPt.x);//window to main window
		 SetCursorPos(mainWinPt.x, mainWinPt.y);//=====================================
		 
		 curCursorType = LEFT_RIGHT_PADDING_CURSOR;
		 SetCapture(); //Even if the mouse goes out of window, its messages would be captured.
		 
		 prevStrt.y = tpLeft.y;  prevEnd.y = btmLeft.y;
		 prevStrt.x = prevEnd.x = tpLeft.x;	
	} else if ((tpLeft.y <= pt.y && pt.y <= btmLeft.y) &&
		(btmRight.x <= pt.x && pt.x <= btmRight.x + BORDER_WIDTH)) {
		 
		 flgRtCrnrActive = true;//Right boundary is to be moved
			 
		 //Move mouse to btmLeft.x========== 						 
		 newWinPt = btmRight;//Scrollview to
		 d.LPtoDP(&newWinPt);//window coordinates
		 if(!GetCursorPos(&mainWinPt)) {//main window cursor pos
			 MessageBox("Error occured while getting cursor position");
			 return;
		 }
		 mainWinPt.x = newWinPt.x + (mainWinPt.x - initialPt.x);//window to main window
		 SetCursorPos(mainWinPt.x, mainWinPt.y);//=====================================
		 
		 curCursorType = LEFT_RIGHT_PADDING_CURSOR;
		 SetCapture(); //Even if the mouse goes out of window, its messages would be captured.
		 
		 prevStrt.y = tpLeft.y;  prevEnd.y = btmLeft.y;
		 prevStrt.x = prevEnd.x = tpRight.x;	

	} else if(tpLeft.x < pt.x && tpRight.x > pt.x && btmLeft.y > pt.y && tpLeft.y < pt.y) {
		
		CDC *cdcPtr = doc->getBitmapDC(); //Get handle to the DC containing the bitmap	
		
		int mg; //Get the zoom size for the image display
		mg = doc->getMagnification();		

		if(doc->getMarkerType() == SETS_REGION_AS_BACKGROUND) {
			
			//Call the document class function to take backup for the undo option
			doc->setModStatus_RgnToBk();
			
			//Mark the region with background color if the marker type is set to  do it

			RGBCOLOR color;
			if(doc->getMarkerType() == SETS_REGION_AS_BACKGROUND)
				color = doc->getBackgroundColor();
			else 
				return; 		

			COLORREF clr(RGB(color.red, color.green, color.blue));//Format accepted by CDC

			int size; //Get the size of the marker
			size = doc->getMarkerSize();

	
		
			
			if(mg == 1 && size == 1) //Use set pixel
				d.SetPixel( pt, clr); 
			else { 
				
				int left, top, right, bottom;//Corners of the rectangle to be drawn
				
				int shift;
				size > 1 ?
					shift = mg * (size - 1) /2				
				:	shift = mg;
				
				pt.x - shift > tpLeft.x ? 
					left = pt.x - shift 
				:	left = tpLeft.x + 1;				

				pt.x + shift + 1 < tpRight.x ?
					right = pt.x + shift + 1 
				:	right = tpRight.x;

				pt.y - shift > tpLeft.y ?
					top = pt.y - shift 
				:	top = tpLeft.y + 1;

				pt.y + shift + 1 < btmLeft.y ?
					bottom = pt.y + shift + 1 
				:	bottom = btmLeft.y;

				//TODO: CASE OF RECTANGLE BORDER EXCEEDING CLIENT AREA WHEN SCROLLING IS PRESENT 
				//FOR THE IMAGE. RIGHT NOW I HAVE ALLOWED DRAWING TO THE HIDDEN AREAS OF THE IMAGE

				//AT SMALL MARKER SIZES, THE DRAWINGS FOR ZOOMED IMAGES ARE NOT MATCHING
				//WITH THAT OF THE ACTUAL IMAGE. 
				//RIGHT NOW I HAVE REMOVED THE OPTION OF ALLOWING ZOOM. HENCE mg WILL ALWAYS BE 1

				CPen mypen;
				mypen.CreatePen(PS_SOLID, 1, clr);

				CBrush mybrush;
				mybrush.CreateSolidBrush(clr);

				d.SelectObject(&mypen);
				d.SelectObject(&mybrush);
				
				d.Rectangle(left, top, right, bottom);
				
				
				if(size != 1 ) { //Draw rectangle on the actual bitmap----------

					cdcPtr->SelectObject(&mypen);
					cdcPtr->SelectObject(&mybrush);
				
					cdcPtr->Rectangle(displayToBitmapCoord(left, mg, true), 
						displayToBitmapCoord(top, mg, false), 
						displayToBitmapCoord(right, mg, true),  
						displayToBitmapCoord(bottom, mg, false) );
				} //------------------------------------------------------------
				mybrush.DeleteObject();
				mypen.DeleteObject();

			}//end of condition mg==1 && size ==1 		
			
			if(size == 1) { //Use setpixel for the actual bitmap			
				cdcPtr->SetPixel(CPoint(displayToBitmapCoord(pt.x, mg, true), 
					displayToBitmapCoord(pt.y, mg, false)), clr);
				
			
			} 

			curCursorType = REGION_MARKER_CURSOR;
		
		}//Marker type is set_region_as_background
		 else {
			COLORREF clr = 
				
				//d.GetPixel(pt); //DONT GET IT FROM THE DISPLAY, BECAUSE IF 
				//TRUE COLOR(24 or 32 bit) IS NOT SET, THEN THE RGB VALUES 
				//RETURNED WOULD BE DIFFERENT FROM THAT OF THE ACTUAL IMAGE

				//TAKE THE PIXEL VALUE DIRECTLY FROM THE BITMAP				
				cdcPtr->GetPixel(CPoint(displayToBitmapCoord(pt.x, mg, true), 
					displayToBitmapCoord(pt.y, mg, false)));

			dlgSelectBkColors dlg(IDD_DIALOG5, GetRValue(clr), GetGValue(clr), 
				GetBValue(clr));
			
			//Set the value for displaying the help info
			((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_BACK_COLOR);
			int intRtnVal = dlg.DoModal();
			((myApp*)AfxGetApp())->setCurWindowInfo(CURRENT_WINDOW_MOLD_IMAGE);

			if(intRtnVal == IDOK) {
				BYTE btR, btG, btB, btSatLumVal;
				COLOR_SELECTION_TYPE selType;
				dlg.getSelectedColors(btR, btG, btB, selType, btSatLumVal);
				//Call the document class function to set the colors as background
				doc->setColorsToBkgrnd(btR, btG, btB, selType, btSatLumVal);
			}	
			
			delete dlg;

			curCursorType = COLOR_MARKER_CURSOR;
		 } //Marker type is set_color_as_background

	}//clicked inside the bitmap	

}//end of mouse down function

void bitmapView::OnMouseMove(UINT nFlags, CPoint pt) {

	BOOL blnEnclosingRectDrawn = false; //flag used at the end of the function
	
	CPoint initialPt, resetPt, tempPt, tempPt1;
	
	moldDoc *doc = (moldDoc *) GetDocument();
	
	if(!doc->isBitmapLoaded()) {
		//No image loaded
		curCursorType = NORMAL_CURSOR;
		return;
	}
	
	CClientDC d(this);
	CScrollView::OnPrepareDC(&d, NULL);


	if(flgTpCrnrActive || flgBtmCrnrActive || flgLtCrnrActive || flgRtCrnrActive ) {
		
		//The user is changing the padded regions along the borders
					
		int intMg = doc->getMagnification();
		initialPt = pt; //Store value before manipulating it
		BOOL flgResetPos = false;		
		
		//If the mouse has moved out then bring it inside the client area
		CRect r;
		GetClientRect(&r);
		if(pt.x < 0 ) {
			pt.x = intMg;
			flgResetPos = true;
		} else if( pt.x >= r.right) {
			pt.x = r.right - intMg;
			flgResetPos = true;
		}
		if(pt.y < 0 ) {
			pt.y = intMg;
			flgResetPos = true;
		} else if(pt.y >= r.bottom) {
			pt.y = r.bottom - intMg;
			flgResetPos = true;
		}//==============================================================
		
		d.DPtoLP(&pt);//Change the coordinates to scrollview coordinates
	
		if(flgTpCrnrActive) {
			//Calculate new cursor position
			resetPt = pt; //initialize
			if(pt.x < tpLeft.x) {
				resetPt.x = tpLeft.x;  flgResetPos = true;
			} else if(pt.x > tpRight.x) {  
				resetPt.x = tpRight.x; flgResetPos = true;
			} 

			if(pt.y < tpLeft.y ) {
				resetPt.y = tpLeft.y;  flgResetPos = true;
			} else if(pt.y > (btmLeft.y - 1 - intMg)) {
				resetPt.y = btmLeft.y - 1 - intMg;  
				flgResetPos = true;
			} else {
				if(intMg > 1) { //Position be a multiple of magnification
					int intRem = (pt.y - tpLeft.y) % intMg;
					if(intRem != 0 ) {
						resetPt.y = pt.y +  intMg - intRem;
						flgResetPos = true;
					}
				}
			}
		} else if (flgBtmCrnrActive) {
			//Calculate new cursor position
			resetPt = pt; //initialize
			if(pt.x < tpLeft.x) {
				resetPt.x = tpLeft.x;  flgResetPos = true;
			} else if(pt.x > tpRight.x) {  
				resetPt.x = tpRight.x; flgResetPos = true;
			} 

			if(pt.y > btmLeft.y ) {
				resetPt.y = btmLeft.y;  flgResetPos = true;
			} else if(pt.y < (tpLeft.y + 1 + intMg)) {
				resetPt.y = tpLeft.y + 1 + intMg;  
				flgResetPos = true;
			} else {
				if(intMg > 1) { //Position be a multiple of magnification
					int intRem = (btmLeft.y - pt.y) % intMg;
					if(intRem != 0 ) {
						resetPt.y = pt.y -  intMg + intRem;
						flgResetPos = true;
					}
				}
			}
		} else if(flgLtCrnrActive) {
			//Calculate new cursor position
			resetPt = pt; //initialize
			if(pt.y < tpLeft.y) {
				resetPt.y = tpLeft.y;  flgResetPos = true;
			} else if(pt.y > btmLeft.y) {  
				resetPt.y = btmLeft.y; flgResetPos = true;
			} 

			if(pt.x < tpLeft.x ) {
				resetPt.x = tpLeft.x;  flgResetPos = true;
			} else if(pt.x > (tpRight.x - 1 - intMg)) {
				resetPt.x = tpRight.x - 1 - intMg;  
				flgResetPos = true;
			} else {
				if(intMg > 1) { //Position be a multiple of magnification
					int intRem = (pt.x - tpLeft.x) % intMg;
					if(intRem != 0 ) {
						resetPt.x = pt.x +  intMg - intRem;
						flgResetPos = true;
					}
				}
			}
		} else if(flgRtCrnrActive) {
			//Calculate new cursor position
			resetPt = pt; //initialize
			if(pt.y < tpLeft.y) {
				resetPt.y = tpLeft.y;  flgResetPos = true;
			} else if(pt.y > btmLeft.y) {  
				resetPt.y = btmLeft.y; flgResetPos = true;
			} 

			if(pt.x > tpRight.x ) {
				resetPt.x = tpRight.x;  flgResetPos = true;
			} else if(pt.x < (tpLeft.x + 1 + intMg)) {
				resetPt.x = tpLeft.x + 1 + intMg;  
				flgResetPos = true;
			} else {
				if(intMg > 1) { //Position be a multiple of magnification
					int intRem = (tpRight.x - pt.x) % intMg;
					if(intRem != 0 ) {
						resetPt.x = pt.x -  intMg + intRem;
						flgResetPos = true;
					}
				}
			}
		} 
		
		//Reset the position of the mouse
		if(flgResetPos) {
			tempPt = resetPt;			
			d.LPtoDP(&tempPt); //Scrollview to window coordinates
			
			if(!GetCursorPos(&tempPt1)) {//main window cursor pos
				 MessageBox("Error occured while getting cursor position");
				 return;
			}
			tempPt1.y = tempPt.y + (tempPt1.y - initialPt.y);//Right window to
			tempPt1.x = tempPt.x + (tempPt1.x - initialPt.x);//main window co-ordinates
			
			SetCursorPos(tempPt1.x, tempPt1.y);			
		}//==========================================================
		
		//Show moving boundaries=====================================
		d.SetROP2(R2_NOTXORPEN); 
		
		//Erase previous
		d.MoveTo(prevStrt);
		d.LineTo(prevEnd);
		
		if(flgTpCrnrActive || flgBtmCrnrActive) {
			prevStrt.y = prevEnd.y = resetPt.y;
		} else if(flgLtCrnrActive || flgRtCrnrActive) {
			prevStrt.x = prevEnd.x = resetPt.x;
		}

		//Draw new
		d.MoveTo(prevStrt);
		d.LineTo(prevEnd);//=========================================		
	
	} else //End of the condition where padding is being set
		{
		
		CPoint tempPt = pt;
		
		d.DPtoLP(&pt);//Change the coordinates to scrollview coordinates

		//Image is displayed but no action selected by the user 
		
		if (tpLeft.x <= pt.x && pt.x <= tpRight.x
			&& ((tpLeft.y - BORDER_WIDTH <= pt.y && pt.y <= tpLeft.y) 
				|| (btmLeft.y <= pt.y && pt.y <= btmLeft.y + BORDER_WIDTH)))
			curCursorType = TOP_BOTTOM_PADDING_CURSOR;
		
		else if (//curCursorType != LEFT_RIGHT_PADDING_CURSOR 
			tpLeft.y <= pt.y && pt.y <= btmLeft.y 
			&& ((btmLeft.x - BORDER_WIDTH <= pt.x && pt.x <= btmLeft.x ) 
				|| (btmRight.x <= pt.x && pt.x <= btmRight.x + BORDER_WIDTH)))
			curCursorType = LEFT_RIGHT_PADDING_CURSOR;
		
		
		//Check whether the mouse pointer is within the image
		else if(tpLeft.x < pt.x && tpRight.x > pt.x && btmLeft.y > pt.y && tpLeft.y < pt.y) {
			if(doc->getMarkerType() == SETS_REGION_AS_BACKGROUND) {
				
				curCursorType = REGION_MARKER_CURSOR;

				//VK 25-Jul-04  Draw enclosing rectangle  - Start 
				int size; //Get the size of the marker
				size = doc->getMarkerSize();
				
				if(size > 1) { //No point in drawing rectangle if size is only one
					
					CRect rect;
					//int rect.left, rect.top, rect.right, rect.bottom;//Corners of the rectangle to be drawn
					int shift = (size - 1) /2;				
					
				    //pt = tempPt;
					
				
					pt.x - shift > tpLeft.x ? 
						rect.left = pt.x - shift 
					:	rect.left = tpLeft.x + 1;				

					pt.x + shift + 1 < tpRight.x ?
						rect.right = pt.x + shift + 1 
					:	rect.right = tpRight.x;

					pt.y - shift > tpLeft.y ?
						rect.top = pt.y - shift 
					:	rect.top = tpLeft.y + 1;

					pt.y + shift + 1 < btmLeft.y ?
						rect.bottom = pt.y + shift + 1 
					:	rect.bottom = btmLeft.y;
					/*
					rect.left = pt.x - shift ;
					rect.right = pt.x + shift + 1 ;
					rect.top = pt.y - shift ;
					rect.bottom = pt.y + shift + 1; 
					*/
					SIZE sz;
					sz.cx = sz.cy = 1;
					
					
					CPoint pt1, pt2;
					pt1.x = rect.left;
					pt1.y = rect.top;

					pt2.x = rect.right;
					pt2.y = rect.bottom;

					d.LPtoDP(&pt1);

					d.LPtoDP(&pt2);

					rect.left = pt1.x;
					rect.top = pt1.y;

					rect.right = pt2.x;
					rect.bottom = pt2.y;


					d.DrawDragRect(&rect, sz, doc->getLpPreviousRect(), sz, NULL, NULL);
					doc->setLpPreviousRect(rect);
					
					blnEnclosingRectDrawn = true; //flag used at the end of the function
					
				}
				
				//VK 25-Jul-04  Draw enclosing rectangle  -  End 

			} else
				curCursorType = COLOR_MARKER_CURSOR;
		
		} else 
			curCursorType = NORMAL_CURSOR;
	} 

	//Erase any previous rectangle formed due to dragging of the mouse
	if(blnEnclosingRectDrawn == false && doc->getLpPreviousRect() != NULL) { 
		SIZE sz;
		sz.cx = sz.cy = 1;
		d.DrawDragRect(CRect(-1,-1,-1,-1), sz, doc->getLpPreviousRect(), sz, NULL, NULL);
		doc->setLpPreviousRectToNull();
	}
}

void bitmapView::OnLButtonUp(UINT nFlags, CPoint pt) {
	
	CClientDC d(this);
	CScrollView::OnPrepareDC(&d, NULL);
	d.DPtoLP(&pt);	//Window to ScrollView Coordinates
	
	if(flgTpCrnrActive) {
		if(pt.y > tpLeft.y) {
			moldDoc *doc = (moldDoc *) GetDocument();
			int intMg = doc->getMagnification();
			int intVal = (pt.y - tpLeft.y);
			int intRem = intVal%intMg;
			if(intRem != 0) {
				TRACE0("Error occured while padding image\n");
				ASSERT(FALSE);
			}
			doc->setTpPadding(intVal/intMg);
		}
		flgTpCrnrActive = false;
		ReleaseCapture();
	
	} else if(flgBtmCrnrActive) {
		if(pt.y < btmLeft.y) {
			moldDoc *doc = (moldDoc *) GetDocument();
			int intMg = doc->getMagnification();
			int intVal = (btmLeft.y - pt.y);
			int intRem = intVal%intMg;
			if(intRem != 0) {
				TRACE0("Error occured while padding image\n");
				ASSERT(FALSE);
			}
			doc->setBtmPadding(intVal/intMg);
		}
		flgBtmCrnrActive = false;
		ReleaseCapture();
	} else if(flgLtCrnrActive) {
		if(pt.x > btmLeft.x) {
			moldDoc *doc = (moldDoc *) GetDocument();
			int intMg = doc->getMagnification();
			int intVal = (pt.x - btmLeft.x);
			int intRem = intVal%intMg;
			if(intRem != 0) {
				TRACE0("Error occured while padding image\n");
				ASSERT(FALSE);
			}
			doc->setLtPadding(intVal/intMg);
		}
		flgLtCrnrActive = false;
		ReleaseCapture();
	} else if(flgRtCrnrActive) {
		if(pt.x < btmRight.x) {
			moldDoc *doc = (moldDoc *) GetDocument();
			int intMg = doc->getMagnification();
			int intVal = (tpRight.x - pt.x);
			int intRem = intVal%intMg;
			if(intRem != 0) {
				TRACE0("Error occured while padding image\n");
				ASSERT(FALSE);
			}
			doc->setRtPadding(intVal/intMg);
		}
		flgRtCrnrActive = false;
		ReleaseCapture();	
	} 
}

////////////////////////////////////////////////////////////////////////////////////////////
void bitmapView::cancelMouseOperations() {
	
	
	if(flgTpCrnrActive || flgBtmCrnrActive || flgLtCrnrActive || flgRtCrnrActive ) {
		flgTpCrnrActive = flgBtmCrnrActive = flgLtCrnrActive = flgRtCrnrActive
					= false;
		curCursorType = NORMAL_CURSOR;
		ReleaseCapture(); //If release capture is called at every ondraw, then vertical and 
		//horizontal scrolling could have the following bug:
		//If we move down the image by moving down the vertical scrollbar and then click
		//on this scrollbar; hell breaks loose. Similar problem happens with horizontal scroll.
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
int bitmapView::displayToBitmapCoord(int inCoord, int inMg, BOOL flgXCoord) {

	int outCoord;

	if(flgXCoord) 
		outCoord = inCoord - (IMAGE_LEFT_INDENT + 1); 
	else
		outCoord = inCoord - (IMAGE_TOP_INDENT + 1);		

	if(inMg > 1) { //Align the point to the actual size image					
		int intRem;				
		intRem = outCoord % inMg;							
		if(intRem > 0 ) 
			outCoord = outCoord + inMg - intRem;
		else
			outCoord = outCoord;
		outCoord = outCoord/inMg ;
	}
	return outCoord;
}