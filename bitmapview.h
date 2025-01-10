
#include <afxwin.h>
#include "molddoc.h"

class bitmapView : public CScrollView {
	DECLARE_DYNCREATE(bitmapView)
private:
	HCURSOR curRegionMarker, curColorMarker;
	CPoint prevStrt, prevEnd; //Used for defining the corners of images 
							 //while setting the padded region

	//Variables defining the corners of the actual image
	CPoint tpLeft, tpRight, btmLeft, btmRight; 

	//Flags for defining the corners
	BOOL flgTpCrnrActive, flgBtmCrnrActive, flgLtCrnrActive, flgRtCrnrActive;

	LOADED_CURSOR_TYPE curCursorType; //Decides the type of cursor icon to be on display

		
public:
	void OnDraw(CDC *p);
	void OnInitialUpdate();
	void OnPrepareDC (CDC* pDC, CPrintInfo* pInfo);
	//void setScrollSizes(moldDoc *doc);
	virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );

	void OnLButtonDown(UINT nFlags, CPoint pt) ;
	void OnMouseMove(UINT nFlags, CPoint pt) ;
	void OnLButtonUp(UINT nFlags, CPoint pt) ;
	void OnRButtonDown(UINT nFlags, CPoint pt);
	void OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message );

	void cancelMouseOperations();

	int displayToBitmapCoord(int inPt, int inMg, BOOL flgXCoord);	
	
	virtual BOOL OnScroll( UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE ) ;
	
	void setNormalCursor(); 

	DECLARE_MESSAGE_MAP()
};
