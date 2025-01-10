#ifndef SCR_READER_H
#define SCR_READER_H

#include <afxwin.h>
#include "definitions.h"

enum SCR_INSTRUCTION_TYPE {
	SCR_LOADIMAGE,
	SCR_SETBACKGROUNDSCREEN,
	SCR_SETBACKGROUNDCOLOR,
	SCR_SETCAMERAPARAMS,
	SCR_SETCAMERAPARAMS_INFINITY,
	SCR_APPLYIMAGE_SNAPS,
	SCR_APPLYIMAGE_SLIDES,
	SCR_SAVEIMAGE,
	SCR_SAVEIMAGEAS,
	SCR_CLOSEIMAGE,
	SCR_CREATEMODEL,
	SCR_CREATEMODELPART,
};

enum SCR_SCREENTYPE {
	SCR_SCREENTYPE_RED,
	SCR_SCREENTYPE_GREEN,
	SCR_SCREENTYPE_BLUE
};

struct SCR_IMAGEPATH_PARAMS { 
	CString imagepath;
};

struct SCR_SETBACKGROUNDSCREEN_PARAMS { 
	SCR_SCREENTYPE screenType;
	unsigned char ucharMinColorOffset;
	unsigned char ucharMaxDiffPercent;
};

struct SCR_SETBACKGROUNDCOLOR_PARAMS { 
	unsigned char ucharBgColorR;
	unsigned char ucharBgColorG;
	unsigned char ucharBgColorB;
	COLOR_SELECTION_TYPE colorSelType;
	unsigned char ucharVarianceVal;

};

struct SCR_SETCAMERAPARAMS_PARAMS { 
	float fltH1;
	float fltH1Distance;
	float fltH2;
	float fltH2Distance;
	float fltW1;
	float fltW1Distance;
	float fltW2;
	float fltW2Distance;

};

struct SCR_SETCAMERAPARAMS_INFINITY_PARAMS { 

	float fltHeight;
	float fltWidth;

};

struct SCR_APPLYIMAGE_SNAPS_PARAMS { 
	float fltCameraLocationX;
	float fltCameraLocationY;
	float fltCameraLocationZ;
	float fltPointingToX;
	float fltPointingToY;
	float fltPointingToZ;

};

struct SCR_APPLYIMAGE_SLIDES_PARAMS { 
	int intModifyPtsVal;
};

struct SCR_CREATEMODELPART_PARAMS { 
	unsigned char ucharModelClrR;
	unsigned char ucharModelClrG;
	unsigned char ucharModelClrB;
	int intMinXIndex;
	int intMaxXIndex;
	int intMinYIndex;
	int intMaxYIndex;
	int intMinZIndex;
	int intMaxZIndex;
	CString strFilePath;
}; 

class SCR_INSTRUCTION {
	
private:
	VOID *ptrParams;
public:
	
	SCR_INSTRUCTION_TYPE instructionType;
	
	SCR_INSTRUCTION() {
		ptrParams = NULL;
	}
	
	void assignAddress(VOID *inPtrNewAddress) {
		if(ptrParams != NULL)
			delete ptrParams;
		ptrParams = inPtrNewAddress;		
	}

	void clearMemory() {
		if(ptrParams != NULL)
			delete ptrParams;
		ptrParams = NULL;
	}

	VOID * getParams() {
		return ptrParams;
	}
	
	~SCR_INSTRUCTION() {
		if(ptrParams != NULL)
			delete ptrParams;
	}
};

//The following class splits the parameters on ',' and gives easy access to each individual
//parameter
class parameterList {
private:
	unsigned char c_ucharParameterCount;
	CString c_arrStrParameters[10];
	
public:
	parameterList(CString inStrParameters); 
	CString getParameter(unsigned char inUcharIndex);
	unsigned char getParameterCount();

	//Validates the string for it's presence within double quotes and absence of invalid characters
	//Also removes the double quotes from the start and end after validating
	BOOL getFilePathParam(unsigned char inUcharIndex, CString &outStrVal); 
	
	//Validates and returns a int value
	BOOL getParameter(unsigned char inUcharIndex, int &outIntVal, BOOL inBlnValidateLimits = FALSE,  
								 int inIntMinLimit = 0, int inIntMaxLimit = 0) ;
	
	//Validates and returns a float value
	BOOL getParameter(unsigned char inUcharIndex, float &outFltVal, BOOL inBlnNonZero = false, 
								 BOOL inBlnValidateLimits = FALSE,  float inFltMinLimit = 0.0f, float inFltMaxLimit = 0.0f) ;

	//Uses the function that returns an int value internally.
	BOOL getParameter(unsigned char inUcharIndex, unsigned char &outUcharVal, BOOL inBlnValidateLimits = FALSE,  
								 int inIntMinLimit = 0, int inIntMaxLimit = 0);


};

class scriptReader {
private:	
	CStdioFile c_ptrFile;
	UINT c_uintLineNumber;
public:
	scriptReader();
	BOOL openFile(CString inStrFilePath, CString &outStrErrMsg);
	BOOL readNextValue(SCR_INSTRUCTION &outInstruction, CString &outStrMessage);
	UINT getLineNumber();
	void closeFile();
	~scriptReader();
	
};

#endif