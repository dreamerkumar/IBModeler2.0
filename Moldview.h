#include <afxwin.h>
#include <afxrich.h>

#define WINDOW_HEIGHT 3000 //The total height of the window

class moldView : public CScrollView {
	
	DECLARE_DYNCREATE(moldView)
private:
	CRichEditCtrl richEditDisplay;
public:
	virtual void OnDraw(class CDC *); 
	int OnCreate(LPCREATESTRUCT lpcs);
	DECLARE_MESSAGE_MAP()
};

