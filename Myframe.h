#include <afxext.h>

class myFrame : public CMDIFrameWnd {

	DECLARE_DYNCREATE(myFrame)	


	virtual void OnClose();


  //BOOL LoadFrame(UINT nIDResource);//, DWORD dwDefaultStyle,
							 //CWnd* pParentWnd, CCreateContext* pContext);

	void myLoadFrame(UINT nIDResource); //Calls the CFrameWnd LoadFrame function and does 
	//other initializations, when the frame loads. 
				
	void showHideToolBar(BOOL inBlnShowOrHide);
	
	int OnCreate(LPCREATESTRUCT l);

    //Functions used by the document classes to prevent loading of more than one document---
	void setDocLoadedFlag(BOOL blnDocLoaded); 
	BOOL getDocLoadedFlag();
	
	DECLARE_MESSAGE_MAP()

protected:
	BOOL g_blnDocLoaded; //Flag check to prevent more than one document from loading
	
	CToolBar toolbar;
	//CReBar rebar; Not added rebar not needed as there is only one toolbar

};

