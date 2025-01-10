#include <afxext.h>


//Instead of using the CSplitterWnd class directly, we have used a class that overrides 
//the StartTracking method. This will prevent the user from changing the sizes of the two
//windows dynamically--------------------------------------------------------------------
class mySplitter : public CSplitterWnd {
	virtual void StartTracking(int ht) {
	}
};//-------------------------------------------------------------------------------------




class myChildWnd : public CMDIChildWnd {
	DECLARE_DYNCREATE(myChildWnd)
public:
	CStatusBar s;
	unsigned int intStatusInds[2];
	 mySplitter splitWnd;
	virtual BOOL OnCreateClient(LPCREATESTRUCT l, CCreateContext *p);
	DECLARE_MESSAGE_MAP()

};






