#include <afxwin.h>
#include <afxext.h>

#include "mychildwnd.h"
#include "moldview.h"
#include "bitmapview.h"
#include "modelview.h"
#include "myframe.h"

IMPLEMENT_DYNCREATE(myChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(myChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()




BOOL myChildWnd::OnCreateClient(LPCREATESTRUCT l, CCreateContext *p) {

	//Don't load if a mold or model file is already loaded.
	if( ((myFrame *)AfxGetMainWnd())->getDocLoadedFlag() ) {
		//Use the mainframe's handle for showing message
		((myFrame *)AfxGetMainWnd())->MessageBox("Please close the current file before loading a new one.", "Cannot open" , MB_OK | MB_ICONWARNING);
		return false;	
	}

	
	s.Create((CWnd *) this);
	intStatusInds[0] = 0;
	intStatusInds[1] = 0;
	s.SetIndicators(intStatusInds,2);
	
	splitWnd.CreateStatic(this, 1, 2);
	//SIZE sz;
	//sz.cx = 170;
	
	//sz.cy = 200;

	//splitWnd.Create( this, 1, 2, sz, p,  WS_CHILD | WS_VISIBLE |WS_HSCROLL | WS_VSCROLL | SPLS_DYNAMIC_SPLIT, AFX_IDW_PANE_FIRST );


	splitWnd.CreateView(0,0,RUNTIME_CLASS(moldView),CSize(170,0), p);
	splitWnd.CreateView(0,1,RUNTIME_CLASS(bitmapView),CSize(0,0), p);
	
	splitWnd.SetColumnInfo(0, 170, 170);

	//This statement ensures that the created window is shown maximized.
	//In this mode, the window takes up the entire client area and also,
	//the minimize, restore and maximize buttons for the child window 
	//disappear.
	this->ShowWindow(SW_SHOWMAXIMIZED);
	//The statement has been commented because it does not work if the splitter
	//window is using a scrollview.

	return TRUE;
	
}

