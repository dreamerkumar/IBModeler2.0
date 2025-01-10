#include <afxext.h>

class modelWnd : public CMDIChildWnd {
	DECLARE_DYNCREATE(modelWnd)
	CToolBar toolbar;
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT l, CCreateContext *p);
	
};

