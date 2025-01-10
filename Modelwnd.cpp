//#include <afxwin.h>
//#include <afxext.h>

#include "modelwnd.h"
#include "resource.h"
#include "myframe.h"

//#include "moldview.h"
//#include "bitmapview.h"
//#include "modelview.h"

IMPLEMENT_DYNCREATE(modelWnd, CMDIChildWnd)



BOOL modelWnd::OnCreateClient(LPCREATESTRUCT l, CCreateContext *p) {

	//Don't load if a mold or model file is already loaded.
	if( ((myFrame *)AfxGetMainWnd())->getDocLoadedFlag() ) {
		//Use the mainframe's handle for showing message
		((myFrame *)AfxGetMainWnd())->MessageBox("Please close the current file before loading a new one.", "Cannot open" , MB_OK | MB_ICONWARNING);
		return false;	
	}

	CMDIChildWnd::OnCreateClient(l, p);
//	s.Create((CWnd *) this);
	//intStatusInds[0] = 0;
//	intStatusInds[1] = 0;
//	s.SetIndicators(intStatusInds,2);
	
//	splitWnd.CreateStatic(this, 1, 2);
		
//	splitWnd.CreateView(0,0,RUNTIME_CLASS(moldView),CSize(170,0), p);
//	splitWnd.CreateView(0,1,RUNTIME_CLASS(bitmapView),CSize(0,0), p);
	//
	//splitWnd.SetColumnInfo(0, 170, 170);

	//This statement ensures that the created window is shown maximized.
	//In this mode, the window takes up the entire client area and also,
	//the minimize, restore and maximize buttons for the child window 
	//disappear.
	this->ShowWindow(SW_SHOWMAXIMIZED);


//	toolbar.CreateEx(AfxGetMainWnd(), TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
//		| CBRS_GRIPPER | CBRS_TOOLTIPS );

//	toolbar.LoadToolBar(IDR_TOOLBAR4);

	return TRUE;
	
}

