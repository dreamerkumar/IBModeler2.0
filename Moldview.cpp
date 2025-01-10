#include "moldView.h"
#include "molddoc.h"


IMPLEMENT_DYNCREATE(moldView, CScrollView)

BEGIN_MESSAGE_MAP(moldView, CScrollView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int moldView::OnCreate(LPCREATESTRUCT lpcs) {
	
	//Call the base class function
	CScrollView::OnCreate(lpcs);
	
	//Set the scrollable height of the window
	SetScrollSizes(MM_TEXT, CSize(0,WINDOW_HEIGHT));		

	//Obtain the dimensions of the window
	CRect rect;
	GetWindowRect(&rect);

	//Create the rich edit control that will display the mold details
	richEditDisplay.Create(WS_VISIBLE | WS_CHILD 
				| ES_MULTILINE    //will traverse more than one line
				| ES_AUTOVSCROLL  
				| ES_READONLY 
				| WS_DISABLED	  //Although we are using an edit control, we wont allow the user to edit it
				, 
				CRect(0,0,
				rect.Width()-17,  //width slightly less than the width of the window so that the vertical scrollbar can be accomodated.
				WINDOW_HEIGHT),	  //Will traverse the entire height of the scrollable window
				this, 1);

	AfxInitRichEdit();            //Should be called after creating the rich edit control      

	CHARFORMAT cf;                //Used to change the font styles
	richEditDisplay.GetDefaultCharFormat(cf);
	
	//Its important to set the dwMask properties for the text settings. Here we specify,
	//whether the next calls to change the font settings are valid.~~~~~~~~~~~~~~~~~~~~~ 
	cf.dwMask = CFM_COLOR | CFM_BOLD;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	cf.dwEffects = CFE_BOLD | CFE_PROTECTED; //Need to remove the CFE_AUTOCOLOR attribute to set the textcolor
	cf.crTextColor = RGB(255, 255, 255);     //Text will be in white
	richEditDisplay.SetDefaultCharFormat(cf);//Set the format 

	richEditDisplay.SetBackgroundColor(false, RGB(102, 153, 255)); //Set the background
	

	return TRUE;
}

void moldView::OnDraw(class CDC *) {
	//Get the document handle
	moldDoc *doc = (moldDoc *) GetDocument();
	//Set the text after obtaining it from the document class.
	richEditDisplay.SetWindowText(doc->getMoldDetails());

}