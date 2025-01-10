#include "definitions.h"


class myApp : public CWinApp {
private :
	CRuntimeClass *d, *cf, *v;
	CString g_strHelpFilePath;
	CURRENT_WINDOW g_curWindow;
public :
	BOOL InitInstance();
	void createNewMold();
	void openFile();
	void OnClose();
	void showHelp();
	void showAboutDlg();
	CString getHelpFilePath();
	void setCurWindowInfo(CURRENT_WINDOW  inCurWindow);
	BOOL readRegKey(HKEY inHkParentKey, CString inStrSubKey, CString &outStrValue); 
	void enableFileTypeSupport();

	DECLARE_MESSAGE_MAP()
};

