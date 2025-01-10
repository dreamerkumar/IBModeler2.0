#include "identifybkgnd.h"


BEGIN_MESSAGE_MAP(identifyBkgnd, CDialog)
	ON_CBN_SELCHANGE(1, onChangeScreenClr)
	ON_EN_CHANGE(IDC_EDIT3,  onChangeR)
	ON_EN_CHANGE(IDC_EDIT4,  onChangeG)
	ON_EN_CHANGE(IDC_EDIT5,  onChangeB)

END_MESSAGE_MAP()



/*
BKGND_SCREEN bkgndScreen;

CEdit *edit1, *edit2, *edit3, *edit4, *edit5;
CStatic *st2, *st3;
CComboBox cmbscreenClr;	
previewWindow1 testViewClr;
*/

identifyBkgnd::identifyBkgnd(int n, BKGND_SCREEN inBkgndScreen) : CDialog(n) {

	bkgndScreen = inBkgndScreen;

}

int identifyBkgnd::OnInitDialog() {
	CDialog::OnInitDialog();
	
	testViewClr.CreateEx(WS_EX_WINDOWEDGE, "SplashViewClass", "", WS_CHILD | WS_VISIBLE, 
	   	CRect(60, 167, 138, 238),this, 2); 	
	testViewClr.btRed = 255;
	testViewClr.btGreen = 0;
	testViewClr.btBlue = 0;

	testViewClr.blnShowEnabled = false; //Dont show surrounding rectangle
	testViewClr.blnShowColor = true;
	
	cmbscreenClr.CreateEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | 
		CBS_DROPDOWNLIST | WS_VSCROLL, CRect(105, 6, 170, 750), this, 1);

	
	st2 = (CStatic *) GetDlgItem(IDC_STATIC2);
	st3 = (CStatic *) GetDlgItem(IDC_STATIC3);
	
	cmbscreenClr.AddString("Red");
	cmbscreenClr.AddString("Green");
	cmbscreenClr.AddString("Blue");

	int intSelection;
	switch(bkgndScreen.screenClr) {
	case RED:
		intSelection = 0;
		st2->SetWindowText("Red value always greater than Green and Blue by at least:");
		st3->SetWindowText("Maximum allowed difference between Green and Blue as a percentage of the above value:");
		break;
	case GREEN:
		intSelection = 1;
		st2->SetWindowText("Green value always greater than Red and Blue by at least:");
		st3->SetWindowText("Maximum allowed difference between Red and Blue as a percentage of the above value:");
		break;
	case BLUE:
		intSelection = 2;
		st2->SetWindowText("Blue value always greater than Green and Red by at least:");
		st3->SetWindowText("Maximum allowed difference between Green and Red as a percentage of the above value:");
		break;
	}
	
	cmbscreenClr.SetCurSel(intSelection);

	(GetDlgItem(IDC_EDIT3))->SetWindowText("255");
	(GetDlgItem(IDC_EDIT4))->SetWindowText("0");
	(GetDlgItem(IDC_EDIT5))->SetWindowText("0");

	return TRUE;
}
void identifyBkgnd::DoDataExchange(CDataExchange *pdx) {

	CDialog::DoDataExchange(pdx);

	DDX_Text(pdx, IDC_EDIT1, bkgndScreen.uintMinDiff);
	DDV_MinMaxUInt(pdx, bkgndScreen.uintMinDiff, 10, 255);

	DDX_Text(pdx, IDC_EDIT2, bkgndScreen.uintMaxDiffPercent);
	DDV_MinMaxUInt(pdx, bkgndScreen.uintMaxDiffPercent, 1,99);
	
}

void identifyBkgnd::OnOK() {
	
	switch(cmbscreenClr.GetCurSel()) {
	case 0:
		bkgndScreen.screenClr = RED;
		break;
	case 1:
		bkgndScreen.screenClr = GREEN;
		break;
	case 2:
		bkgndScreen.screenClr = BLUE;
		break;
	}
	
	CDialog::OnOK();
}

BKGND_SCREEN identifyBkgnd::getBkgndScreen() {
	return bkgndScreen;
}

void identifyBkgnd::onChangeR() {
	if(GetDlgItemInt(IDC_EDIT3) > 255) {
		testViewClr.btRed = 255;
		(GetDlgItem(IDC_EDIT3))->SetWindowText("255");
	} else
		testViewClr.btRed = GetDlgItemInt(IDC_EDIT3);
	testViewClr.Invalidate();

}
void identifyBkgnd::onChangeG() {
	if(GetDlgItemInt(IDC_EDIT4) > 255) {
		testViewClr.btGreen = 255;
		(GetDlgItem(IDC_EDIT4))->SetWindowText("255");
	} else
		testViewClr.btGreen = GetDlgItemInt(IDC_EDIT4);
	testViewClr.Invalidate();
}
void identifyBkgnd::onChangeB() {
	if(GetDlgItemInt(IDC_EDIT5) > 255) {
		testViewClr.btBlue = 255;
		(GetDlgItem(IDC_EDIT5))->SetWindowText("255");
	} else
		testViewClr.btBlue = GetDlgItemInt(IDC_EDIT5);
	testViewClr.Invalidate();
}

void identifyBkgnd::onChangeScreenClr() {
	switch(cmbscreenClr.GetCurSel()) {
	case 0:
		st2->SetWindowText("Red value always greater than Green and Blue by at least:");
		st3->SetWindowText("Maximum allowed difference between Green and Blue as a percentage of the above value:");
		break;
	case 1:
		st2->SetWindowText("Green value always greater than Red and Blue by at least:");
		st3->SetWindowText("Maximum allowed difference between Red and Blue as a percentage of the above value:");
		break;
	case 2:
		st2->SetWindowText("Blue value always greater than Green and Red by at least:");
		st3->SetWindowText("Maximum allowed difference between Green and Red as a percentage of the above value:");
		break;
	}

}

