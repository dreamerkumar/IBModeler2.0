#include <afxwin.h>
#include "resource.h"
#include "myframe.h"
#include "dialogs.h"



IMPLEMENT_DYNCREATE(myFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(myFrame, CMDIFrameWnd)
//	ON_COMMAND(ID_FILE_CLOSE, OnClose)
	ON_WM_CREATE()


END_MESSAGE_MAP()


void myFrame::OnClose() {
	CMDIFrameWnd::OnClose();
}


void myFrame::myLoadFrame(UINT nIDResource) {

	CMDIFrameWnd::LoadFrame(nIDResource);	
}


void myFrame::showHideToolBar(BOOL inBlnShowOrHide) {
	ShowControlBar(&toolbar, inBlnShowOrHide, FALSE);
	
}

int myFrame::OnCreate(LPCREATESTRUCT l) {

	g_blnDocLoaded = false; //No document is loaded at start
	
	CFrameWnd::OnCreate(l);
		
	//toolbar.CreateEx(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS );

	toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS );

	toolbar.LoadToolBar(IDR_TOOLBAR1);

	//rebar.Create(this);
	//rebar.AddBar(&toolbar); No need to add to the rebar as there is only one toolbar
	return TRUE;
}


//Functions used by the document classes to prevent loading of more than one document------
void myFrame::setDocLoadedFlag(BOOL blnDocLoaded) {
	g_blnDocLoaded = blnDocLoaded;
}


BOOL myFrame::getDocLoadedFlag() {
	return g_blnDocLoaded;
}
//-----------------------------------------------------------------------------------------


