#include "scriptreader.h"

parameterList::parameterList(CString inStrParameters) {

	c_ucharParameterCount = 0; //0 means no parameter
	
	inStrParameters.TrimLeft();
	inStrParameters.TrimRight();

	if(inStrParameters == "") //No parameter exists.
		return; 

	
	//~~~Logic for replacing a comma in between double quotes (within a string) with character "|"
	
	int intLength = inStrParameters.GetLength();
	BOOL blnQuotesStarted = false;
	int intStrtIndex;
	CString strTempString;
	
	for(int intCounter = 0; intCounter < intLength; intCounter++) {
		if(inStrParameters.GetAt(intCounter) == '"') {
			
			if(!blnQuotesStarted) {
				blnQuotesStarted = true;
				intStrtIndex  = intCounter;

			} else {
					
				strTempString = inStrParameters.Mid(intStrtIndex, intCounter + 1 - intStrtIndex);
				strTempString.Replace(",","|");
				inStrParameters = inStrParameters.Left(intStrtIndex) + strTempString
					+ inStrParameters.Right(intLength - intCounter -1);
				
				
				blnQuotesStarted = false;				
			}	
		}
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	
	
	//~~Parameter separation logic~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	int intParamEndIndex;
	do {
		
		//Move from left and keep separating out the parameters one by one, from the main string

		
		//Find the next instance of comma from where the next parameter will start
		intParamEndIndex = inStrParameters.Find(","); 
		
		
		//If the parameters have not exceeded the maximum limit, assign the next parameter
		if(c_ucharParameterCount <= 9) {

			if(intParamEndIndex == -1) //No more parameters after this
			
				c_arrStrParameters[c_ucharParameterCount] = inStrParameters;
			
			else {
				
				if(intParamEndIndex == 0)  //only comma, no values
					c_arrStrParameters[c_ucharParameterCount] = "";
				else 				
					c_arrStrParameters[c_ucharParameterCount] = inStrParameters.Left(intParamEndIndex);
			}

			//Trim any spaces
			c_arrStrParameters[c_ucharParameterCount].TrimLeft();
			c_arrStrParameters[c_ucharParameterCount].TrimRight();
			//If there are any instances of character '|', this must have been set at the 
			//start of this function. They should be replaced back to ',"
			c_arrStrParameters[c_ucharParameterCount].Replace("|", ",");
		}

		
		if(intParamEndIndex  != -1 ) { //This is not the last parameter

			//Subtract the current parameter and comma from the main string
			if(inStrParameters.GetLength() == intParamEndIndex + 1) //parameters end at the comma
				inStrParameters = "";
			else
				inStrParameters = inStrParameters.Right(inStrParameters.GetLength() - intParamEndIndex - 1);
		}

		c_ucharParameterCount++; 
	
	} while(intParamEndIndex != -1);

}
 
CString parameterList::getParameter(unsigned char inUcharIndex) { //Index value starts from 1
	
	//Parameter index should not exceed 10, 
	//and if it is less than ten then it should not exceed the maximum supplied parameters

	if(inUcharIndex <= c_ucharParameterCount && inUcharIndex <= 10) 
		return c_arrStrParameters[inUcharIndex - 1];	//array value starts from zero
	else
		return "";
}


//Validates the string for it's presence within double quotes and absence of invalid characters
//Also removes the double quotes from the start and end after validating
BOOL parameterList::getFilePathParam(unsigned char inUcharIndex, CString &outStrVal) {
	
	outStrVal = getParameter(inUcharIndex);

	//Should be present within double quotes
	if(outStrVal.Left(1) != "\"" || outStrVal.Right(1) != "\"") 
		return FALSE;
	
	outStrVal = outStrVal.Mid(1); //Removes " from the start
	outStrVal = outStrVal.Left(outStrVal.GetLength() - 1); //Removes " from the end

	outStrVal.TrimLeft();
	outStrVal.TrimRight();
	
	//Now check for invalid characters
	if(outStrVal.Find("\"") != -1 ||
		outStrVal.Find("|") != -1 ||
		outStrVal.Find("*") != -1 ||
		outStrVal.Find("?") != -1 ||
		outStrVal.Find("<") != -1 ||
		outStrVal.Find(">") != -1 
	)
		return FALSE;
	else
		return TRUE;
}
	


BOOL parameterList::getParameter(unsigned char inUcharIndex, unsigned char &outUcharVal, BOOL inBlnValidateLimits,  
								 int inIntMinLimit, int inIntMaxLimit) {

	//Uses the function that returns an int value internally.

	int intVal;

	if(!getParameter(inUcharIndex, intVal, inBlnValidateLimits, inIntMinLimit, inIntMaxLimit))
		return FALSE;
	
	if(intVal < 0 || intVal > 255) //Unsigned char values should  be between 0 and 255
		return FALSE;

	outUcharVal = (unsigned char)intVal;

	return TRUE;
}


BOOL parameterList::getParameter(unsigned char inUcharIndex, int &outIntVal, BOOL inBlnValidateLimits,  
								 int inIntMinLimit, int inIntMaxLimit) {

	CString strParameter = getParameter(inUcharIndex);
	
	if(strParameter == "")
		return FALSE;

	CString strCheckInteger = "0123456789";
			
	
	int intParamLength = strParameter.GetLength();
	
	for(int intCtr = 0; intCtr < intParamLength; intCtr++) { //Loop through to check for integer values
		if(strCheckInteger.Find(strParameter.GetAt(intCtr)) == -1)  //Not in the list
			return FALSE;
	}

	outIntVal = atoi(strParameter);

	if(inBlnValidateLimits) {
		if(outIntVal < inIntMinLimit || outIntVal > inIntMaxLimit)
			return FALSE;
	}

	return TRUE;
}

BOOL parameterList::getParameter(unsigned char inUcharIndex, float &outFltVal, BOOL inBlnNonZero, 
								 BOOL inBlnValidateLimits, float inFltMinLimit, float inFltMaxLimit) {
	
	CString strParameter = getParameter(inUcharIndex);
	
	if(strParameter == "")
		return FALSE;

	CString strCheckFloat = "-.0123456789";
			
	
	int intParamLength = strParameter.GetLength();
	
	int intPointCtr = 0;

	for(int intCtr = 0; intCtr < intParamLength; intCtr++) { //Loop through to check for float values
		char chrVal = strParameter.GetAt(intCtr);
		
		if(strCheckFloat.Find(chrVal) == -1)  //Not in the list
			return FALSE;

		if(chrVal == '-' && intCtr != 0) //- sign should only be present at the start
			return FALSE; 
		
		if(chrVal == '.') {
			
			if(intCtr == intParamLength - 1) //Last position
				return FALSE; //decimal point should be followed by at least one digit

			intPointCtr++;
		}
		
		if(intPointCtr > 1) //Point should be present only once
			return FALSE;
		
	}

	outFltVal = (float)atof(strParameter);

	if(inBlnNonZero) {
		if(outFltVal == 0.0)
			return FALSE;
	}
	
	if(inBlnValidateLimits) {
		if(outFltVal < inFltMinLimit || outFltVal > inFltMaxLimit)
			return FALSE;
	}

	return TRUE;
}


unsigned char parameterList::getParameterCount() {
	return c_ucharParameterCount; 
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ scriptReader Class functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

scriptReader::scriptReader() {
	
	//initialize 
	c_uintLineNumber = 0;


}

BOOL scriptReader::openFile(CString inStrFilePath, CString &outStrErrMsg) {
	
	if(!c_ptrFile.Open(inStrFilePath, CFile::modeRead)) {
		outStrErrMsg = "Error: Could not open file: \n'" + inStrFilePath;
		outStrErrMsg+= "'\n for reading.";
		return FALSE;
	}
	
	return TRUE;
}

BOOL scriptReader::readNextValue(SCR_INSTRUCTION &outInstruction, CString &outStrMessage) {

	try {
		
		CString strLine;
		
		if(!c_ptrFile.ReadString(strLine)) { //End of file reached

			outStrMessage = "End of file reached";
			return FALSE;
		}

		c_uintLineNumber++; 		

		strLine.TrimLeft();
		strLine.TrimRight();
		
		if(strLine == "") { //Blank line

			outStrMessage = "Blank line";
			return FALSE;
		}

		if(strLine.Left(2) == "//") { //Commented line (equivalent to blank line)
		
			outStrMessage = "Blank line";
			return FALSE;
		}

		//Get the instruction type ~~~~~~~~~~~~~~~~~~~~~~~~~~	
		int intCommandEndIndex;
		
		intCommandEndIndex = strLine.Find(" ") ;
		
		CString strCommand;
		CString strParameters;

		
		if(intCommandEndIndex == -1) {
			strCommand = strLine;
			strParameters = "";
		} else {
			strCommand = strLine.Left(intCommandEndIndex);
			strParameters = strLine.Mid(intCommandEndIndex + 1);			
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		
		//The character "|" will be used by the parameterList class to temporarily replace commas
		//from in between double quotes, so raise an error if this character is already present
		if(strParameters.Find("|") != -1) {

			outStrMessage.Format("Invalid character '|' found at line number %d", c_uintLineNumber);
			return FALSE;
		}		

		
		//Parse the parameter by using the proper class for it
		parameterList pList(strParameters);


		//Extract other params based on the instruction type
		if(strCommand == "LOADIMAGE") {
			
			if(pList.getParameterCount() != 1) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction LOADIMAGE at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_IMAGEPATH_PARAMS *ptrParams = new SCR_IMAGEPATH_PARAMS();
			
			if(!pList.getFilePathParam(1,ptrParams->imagepath)) {
				outStrMessage.Format("Invalid filepath for instruction LOADIMAGE at line number %d", c_uintLineNumber);
				return FALSE;
			}
			CString strExtension = ptrParams->imagepath.Right(4);
			strExtension.MakeUpper();
			if(strExtension != ".BMP" && strExtension != ".DIB") {
				outStrMessage.Format("Invalid file extension in the first parameter for instruction LOADIMAGE at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_LOADIMAGE;
		
		} else if (strCommand == "SETBACKGROUNDSCREEN") {
			
			if(pList.getParameterCount() != 3) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction SETBACKGROUNDSCREEN at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_SETBACKGROUNDSCREEN_PARAMS *ptrParams = new SCR_SETBACKGROUNDSCREEN_PARAMS();

			//First parameter
			if(pList.getParameter(1) == "RED") 					
				ptrParams->screenType = SCR_SCREENTYPE_RED;	
			else if(pList.getParameter(1) == "GREEN")
				ptrParams->screenType = SCR_SCREENTYPE_GREEN;
			else if(pList.getParameter(1) == "BLUE")
				ptrParams->screenType = SCR_SCREENTYPE_BLUE;
			else {
				outStrMessage.Format("Invalid first parameter for instruction SETBACKGROUNDSCREEN at line number %d", c_uintLineNumber);
				return FALSE;
			}

			//Second parameter
			if(!pList.getParameter(2, ptrParams->ucharMinColorOffset, true, 10, 255)) {
				outStrMessage.Format("Invalid second parameter for instruction SETBACKGROUNDSCREEN at line number %d", c_uintLineNumber);
				return FALSE;
			}
			
			
			//Third parameter
			if(!pList.getParameter(3, ptrParams->ucharMaxDiffPercent, true, 1, 99)) {
				outStrMessage.Format("Invalid third parameter for instruction SETBACKGROUNDSCREEN at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_SETBACKGROUNDSCREEN;
			
		
		} else if (strCommand == "SETBACKGROUNDCOLOR") {
			
			if(!(pList.getParameterCount() == 4 || pList.getParameterCount() == 5)) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_SETBACKGROUNDCOLOR_PARAMS *ptrParams = new SCR_SETBACKGROUNDCOLOR_PARAMS();


			//First parameter
			if(!pList.getParameter(1, ptrParams->ucharBgColorR)) {
				outStrMessage.Format("Invalid first parameter for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Second parameter
			if(!pList.getParameter(2, ptrParams->ucharBgColorG)) {
				outStrMessage.Format("Invalid second parameter for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Third parameter
			if(!pList.getParameter(3, ptrParams->ucharBgColorB)) {
				outStrMessage.Format("Invalid third parameter for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Fourth parameter
			if(pList.getParameter(4) == "SAT") 					
				ptrParams->colorSelType = SAT;	
			else if(pList.getParameter(4) == "LUM")
					ptrParams->colorSelType = LUM;	
			else if(pList.getParameter(4) == "CUR_AND_BRIGHTER")
					ptrParams->colorSelType = CUR_AND_BRIGHTER;	
			else if(pList.getParameter(4) == "CUR_AND_DARKER")
					ptrParams->colorSelType = CUR_AND_DARKER;	
			else if(pList.getParameter(4) == "SINGLE_COLOR")
					ptrParams->colorSelType = NONE;	
			else {
				outStrMessage.Format("Invalid fourth parameter for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
				return FALSE;
			}

			//Fifth parameter
			if(ptrParams->colorSelType == SAT || ptrParams->colorSelType == LUM) {
							
				if(pList.getParameterCount() != 5) {//Missing or incorrect number of parameters
					outStrMessage.Format("Missing or incorrect number of parameters for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
					return FALSE;
				}	
				
				if(!pList.getParameter(5, ptrParams->ucharVarianceVal, true, 0, 240)) {
					outStrMessage.Format("Invalid fifth parameter for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
					return FALSE;
				}
			} else {//Parameters should not be 5 for operations other than sat or lum 

				if(pList.getParameterCount() == 5 ) {//Missing or incorrect number of parameters
					outStrMessage.Format("Missing or incorrect number of parameters for instruction SETBACKGROUNDCOLOR at line number %d", c_uintLineNumber);
					return FALSE;
				}
				ptrParams->ucharVarianceVal = 0;
			}			
			
			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_SETBACKGROUNDCOLOR;			
		
		} else if (strCommand == "SETCAMERAPARAMS") {
			
			if(pList.getParameterCount() != 8 ) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_SETCAMERAPARAMS_PARAMS *ptrParams = new SCR_SETCAMERAPARAMS_PARAMS();

			//First parameter
			if(!pList.getParameter(1, ptrParams->fltH1, TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid first parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Second parameter
			if(!pList.getParameter(2, ptrParams->fltH1Distance, TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid second parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Third parameter
			if(!pList.getParameter(3, ptrParams->fltH2, TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid third parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Fourth parameter
			if(!pList.getParameter(4, ptrParams->fltH2Distance, TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid fourth parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Fifth parameter
			if(!pList.getParameter(5, ptrParams->fltW1,  TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid fifth parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Sixth parameter
			if(!pList.getParameter(6, ptrParams->fltW1Distance,  TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid sixth parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Seventh parameter
			if(!pList.getParameter(7, ptrParams->fltW2,TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid seventh parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Eighth parameter
			if(!pList.getParameter(8, ptrParams->fltW2Distance,  TRUE, TRUE, 0.0001f, 50000)) {
				outStrMessage.Format("Invalid eighth parameter for instruction SETCAMERAPARAMS at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_SETCAMERAPARAMS;
			
		
		} else if (strCommand == "SETCAMERAPARAMS_INFINITY") {
			
			if(pList.getParameterCount() != 2 ) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction SETCAMERAPARAMS_INFINITY at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_SETCAMERAPARAMS_INFINITY_PARAMS *ptrParams = new SCR_SETCAMERAPARAMS_INFINITY_PARAMS();


			//First parameter
			if(!pList.getParameter(1, ptrParams->fltHeight, TRUE, TRUE, 1, 50000)) {
				outStrMessage.Format("Invalid first parameter for instruction SETCAMERAPARAMS_INFINITY at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Second parameter
			if(!pList.getParameter(2, ptrParams->fltWidth, TRUE, TRUE, 1, 50000)) {
				outStrMessage.Format("Invalid second parameter for instruction SETCAMERAPARAMS_INFINITY at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_SETCAMERAPARAMS_INFINITY;
			
		
		} else if (strCommand == "APPLYIMAGE_SNAPS") {
			
			if(pList.getParameterCount() != 6 ) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction APPLYIMAGE_SNAPS at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_APPLYIMAGE_SNAPS_PARAMS *ptrParams = new SCR_APPLYIMAGE_SNAPS_PARAMS();


			//First parameter
			if(!pList.getParameter(1, ptrParams->fltCameraLocationX, FALSE, TRUE, -50000, 50000)) {
				outStrMessage.Format("Invalid first parameter for instruction APPLYIMAGE_SNAPS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Second parameter
			if(!pList.getParameter(2, ptrParams->fltCameraLocationY, FALSE, TRUE, -50000, 50000)) {
				outStrMessage.Format("Invalid second parameter for instruction APPLYIMAGE_SNAPS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Third parameter
			if(!pList.getParameter(3, ptrParams->fltCameraLocationZ, FALSE, TRUE, -50000, 50000)) {
				outStrMessage.Format("Invalid third parameter for instruction APPLYIMAGE_SNAPS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Fourth parameter
			if(!pList.getParameter(4, ptrParams->fltPointingToX, FALSE, TRUE, -50000, 50000	)) {
				outStrMessage.Format("Invalid fourth parameter for instruction APPLYIMAGE_SNAPS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Fifth parameter
			if(!pList.getParameter(5, ptrParams->fltPointingToY, FALSE, TRUE, -50000, 50000)) {
				outStrMessage.Format("Invalid fifth parameter for instruction APPLYIMAGE_SNAPS at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Sixth parameter
			if(!pList.getParameter(6, ptrParams->fltPointingToZ, FALSE, TRUE, -50000, 50000)) {
				outStrMessage.Format("Invalid sixth parameter for instruction APPLYIMAGE_SNAPS at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_APPLYIMAGE_SNAPS;
			
		
		} else if (strCommand == "APPLYIMAGE_SLIDES") {
			
			if(!(pList.getParameterCount() == 0 || pList.getParameterCount() == 1)  ) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction APPLYIMAGE_SLIDES at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_APPLYIMAGE_SLIDES_PARAMS *ptrParams = new SCR_APPLYIMAGE_SLIDES_PARAMS();


			//First parameter
			if(pList.getParameterCount() == 1) {
				if(!pList.getParameter(1, ptrParams->intModifyPtsVal, TRUE, 1, 9)) {
					outStrMessage.Format("Invalid first parameter for instruction APPLYIMAGE_SLIDES at line number %d", c_uintLineNumber);
					return FALSE;
				}
			} else {
				ptrParams->intModifyPtsVal = 0; //Pass zero for no parameters
			}

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_APPLYIMAGE_SLIDES;			
		
		} else if (strCommand == "SAVEIMAGE") {
			
			if(pList.getParameterCount() != 0 ) {//Missing or incorrect number of parameters
				outStrMessage.Format("SAVEIMAGE does not take any parameters. Error occured at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.instructionType = SCR_SAVEIMAGE;			
		
		} else if(strCommand == "SAVEIMAGEAS") {
			
			if(pList.getParameterCount() != 1) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction SAVEIMAGEAS at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_IMAGEPATH_PARAMS *ptrParams = new SCR_IMAGEPATH_PARAMS();

			if(!pList.getFilePathParam(1,ptrParams->imagepath)) {
				outStrMessage.Format("Invalid filepath for instruction SAVEIMAGEAS at line number %d", c_uintLineNumber);
				return FALSE;
			}			
			CString strExtension = ptrParams->imagepath.Right(4);
			strExtension.MakeUpper();
			if(strExtension != ".BMP") {
				outStrMessage.Format("Invalid file extension in the first parameter for instruction SAVEIMAGEAS at line number %d", c_uintLineNumber);
				return FALSE;
			}			
			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_SAVEIMAGEAS;
		
		}else if (strCommand == "CLOSEIMAGE") {
			
			if(pList.getParameterCount() != 0 ) {//Missing or incorrect number of parameters
				outStrMessage.Format("CLOSEIMAGE does not take any parameters. Error occured at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.instructionType = SCR_CLOSEIMAGE;
		
		} else if (strCommand == "CREATEMODEL") {
			
			if(pList.getParameterCount() != 4) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction CREATEMODEL at line number %d", c_uintLineNumber);
				return FALSE;
			}	

			SCR_CREATEMODELPART_PARAMS *ptrParams = new SCR_CREATEMODELPART_PARAMS();

			//First parameter
			if(!pList.getParameter(1, ptrParams->ucharModelClrR)) {
				outStrMessage.Format("Invalid first parameter for instruction CREATEMODEL at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Second parameter
			if(!pList.getParameter(2, ptrParams->ucharModelClrG)) {
				outStrMessage.Format("Invalid second parameter for instruction CREATEMODEL at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Third parameter
			if(!pList.getParameter(3, ptrParams->ucharModelClrB)) {
				outStrMessage.Format("Invalid third parameter for instruction CREATEMODEL at line number %d", c_uintLineNumber);
				return FALSE;
			}

			//Fourth parameter
			if(!pList.getFilePathParam(4,ptrParams->strFilePath)) {
				outStrMessage.Format("Invalid filepath for instruction CREATEMODEL at line number %d", c_uintLineNumber);
				return FALSE;
			}			
			CString strExtension = ptrParams->strFilePath.Right(4);
			strExtension.MakeUpper();
			if(strExtension != ".MDL") {
				outStrMessage.Format("Invalid file extension in the fourth parameter for instruction CREATEMODEL at line number %d", c_uintLineNumber);
				return FALSE;
			}

			//Specify the rest of the points as zero
			ptrParams->intMaxXIndex = 0;
			ptrParams->intMaxYIndex = 0;
			ptrParams->intMaxZIndex = 0;
			ptrParams->intMinXIndex = 0;
			ptrParams->intMinYIndex = 0;
			ptrParams->intMinZIndex = 0;

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_CREATEMODEL;

		
		} else if (strCommand == "CREATEMODELPART") {
			
			if(pList.getParameterCount() != 10 ) {//Missing or incorrect number of parameters
				outStrMessage.Format("Missing or incorrect number of parameters for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}

			SCR_CREATEMODELPART_PARAMS *ptrParams = new SCR_CREATEMODELPART_PARAMS();


			//First parameter
			if(!pList.getParameter(1, ptrParams->ucharModelClrR)) {
				outStrMessage.Format("Invalid first parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Second parameter
			if(!pList.getParameter(2, ptrParams->ucharModelClrG)) {
				outStrMessage.Format("Invalid second parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Third parameter
			if(!pList.getParameter(3, ptrParams->ucharModelClrB)) {
				outStrMessage.Format("Invalid third parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Fourth parameter
			if(!pList.getParameter(4, ptrParams->intMinXIndex,true, 1,1000)) {
				outStrMessage.Format("Invalid fourth parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Fifth parameter
			if(!pList.getParameter(5, ptrParams->intMaxXIndex,true, 1,1000)) {
				outStrMessage.Format("Invalid fifth parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}

			//Upper index should be greater than lower index by at least 2
			if(ptrParams->intMaxXIndex < ptrParams->intMinXIndex + 2) {
				outStrMessage.Format("Wrong index values specified for the X axis for instruction CREATEMODELPART.\n maxxindex should be greater than minxindex by at least 2.\nThe error occured at line number %d", c_uintLineNumber);
				return FALSE;
			}
			
			//Sixth parameter
			if(!pList.getParameter(6, ptrParams->intMinYIndex,true, 1,1000)) {
				outStrMessage.Format("Invalid sixth parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Seventh parameter
			if(!pList.getParameter(7, ptrParams->intMaxYIndex,true, 1,1000)) {
				outStrMessage.Format("Invalid seventh parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}

			
			//Upper index should be greater than lower index by at least 2
			if(ptrParams->intMaxYIndex < ptrParams->intMinYIndex + 2) {
				outStrMessage.Format("Wrong index values specified for the Y axis for instruction CREATEMODELPART.\n maxyindex should be greater than minyindex by at least 2.\nThe error occured at line number %d", c_uintLineNumber);
				return FALSE;
			}
			
			//Eighth parameter
			if(!pList.getParameter(8, ptrParams->intMinZIndex,true, 1,1000)) {
				outStrMessage.Format("Invalid eighth parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}
			//Nineth parameter
			if(!pList.getParameter(9, ptrParams->intMaxZIndex,true, 1,1000)) {
				outStrMessage.Format("Invalid nineth parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}

			//Upper index should be greater than lower index by at least 2
			if(ptrParams->intMaxZIndex < ptrParams->intMinZIndex + 2) {
				outStrMessage.Format("Wrong index values specified for the Z axis for instruction CREATEMODELPART.\n maxzindex should be greater than minzindex by at least 2.\nThe error occured at line number %d", c_uintLineNumber);
				return FALSE;
			}		
			
			//Tenth parameter
			if(!pList.getFilePathParam(10,ptrParams->strFilePath)) {
				outStrMessage.Format("Invalid filepath for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}			
			CString strExtension = ptrParams->strFilePath.Right(4);
			strExtension.MakeUpper();
			if(strExtension != ".MDL") {
				outStrMessage.Format("Invalid file extension in the tenth parameter for instruction CREATEMODELPART at line number %d", c_uintLineNumber);
				return FALSE;
			}

			outInstruction.assignAddress(ptrParams);
			outInstruction.instructionType = SCR_CREATEMODELPART;
			
		
		} else {
			outStrMessage.Format("Could not identify instruction at line number %d", c_uintLineNumber);
			return FALSE;
		}

		
	} catch(CException *e) {
		
		e->Delete();				
		
		outStrMessage = "Error occured while reading from the script file";
		return FALSE;

	}

	return TRUE;
}

UINT scriptReader::getLineNumber() {
	return c_uintLineNumber;
}

void scriptReader::closeFile() {
	//initialize 
	c_uintLineNumber = 0;

	//If the file is open, close it
	if(c_ptrFile.m_hFile != (UINT)(-1) && c_ptrFile.m_hFile != NULL) //File is open
		c_ptrFile.Close();
}

scriptReader::~scriptReader() {
	closeFile();
}
	