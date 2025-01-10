#ifndef IDENTIFY_BKGND_H_
#define IDENTIFY_BKGND_H_

#include "dialogs.h"

enum BKGND_SCREEN_CLR {
	RED, 
	BLUE,
	GREEN
};
struct BKGND_SCREEN {
	BKGND_SCREEN_CLR screenClr;
	unsigned int uintMinDiff;
	unsigned int uintMaxDiffPercent;
	
	BKGND_SCREEN() {
		screenClr = RED;
		uintMinDiff = 50;
		uintMaxDiffPercent = 50;
	}

	void operator = (BKGND_SCREEN inBkgndScreen) {
		screenClr = inBkgndScreen.screenClr;
		uintMinDiff = inBkgndScreen.uintMinDiff;
		uintMaxDiffPercent = inBkgndScreen.uintMaxDiffPercent;
	}

}; 

class identifyBkgnd : public CDialog {

private:
	BKGND_SCREEN bkgndScreen;
	CStatic *st2, *st3;
	CComboBox cmbscreenClr;	
	previewWindow1 testViewClr;

public:
	identifyBkgnd(int n, BKGND_SCREEN inBkgndScreen);

	int OnInitDialog();
	void DoDataExchange(CDataExchange *pdx);
	void OnOK();
	BKGND_SCREEN getBkgndScreen();

	void onChangeR();
	void onChangeG();
	void onChangeB();

	void onChangeScreenClr();

	DECLARE_MESSAGE_MAP()
};

#endif