#include "definitions.h"
#include "errorConstants.h"
#include "math.h"
/////////////////////////////////////////////////////////////////////////////////////////////////

BOOL sameSigns(float inVal1, float inVal2 ) {
	if ( inVal1 < 0 && inVal2 < 0 ) 
		return true;
	else if ( inVal1 >= 0 && inVal2 >= 0 ) 
		return true;
	else 
		return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

float modulus(float inVal) {
	if (inVal < 0 )
		return -inVal;
	else 
		return inVal;
}

/////////////////////////////RGBCOLOR/////////////////////////////////////////
RGBCOLOR::RGBCOLOR() 
	{red = blue = green = 0 ; }

BOOL RGBCOLOR::operator == ( RGBCOLOR inC ) {
	if(red == inC.red && green == inC.green && blue == inC.blue ) 
		return true ;
	else 
		return false;
}

BOOL RGBCOLOR::operator != ( RGBCOLOR inC ) {
	if(red == inC.red && green == inC.green && blue == inC.blue ) 
		return false;
	else 
		return true;
}

///////////////////FLTPOINT3D///////////////////////////////////////////////
FLTPOINT3D::FLTPOINT3D() {
	x = y = z = 0.0f;
}

FLTPOINT3D::FLTPOINT3D( float inX, float inY, float inZ ) {
	x = inX; y = inY; z = inZ;
}

FLTPOINT3D::operator = ( FLTPOINT3D inP ) {
	x = inP.x ; y = inP.y ; z = inP.z ; 
}

FLTPOINT3D FLTPOINT3D::operator + ( FLTPOINT3D inP ) {
	FLTPOINT3D outP;
	outP.x = x + inP.x ; outP.y = y + inP.y ; outP.z = z + inP.z ; 
	return outP;
}
//Added on 21 Jan 2004
FLTPOINT3D FLTPOINT3D::operator - ( FLTPOINT3D inP ) {
	FLTPOINT3D outP;
	outP.x = x - inP.x ; outP.y = y - inP.y ; outP.z = z - inP.z ; 
	return outP;
} //end of addition


FLTPOINT3D FLTPOINT3D::operator * ( float inK ) {
	FLTPOINT3D outP;
	outP.x = x * inK; outP.y = y * inK; outP.z = z * inK; 
	return outP;
}

FLTPOINT3D FLTPOINT3D::operator / ( float inK ) {
	FLTPOINT3D outP;
	outP.x = x / inK; outP.y = y / inK; outP.z = z / inK; 
	return outP;
}

///////////////////////////VECTOR/////////////////////////////////////////////
VECTOR::VECTOR() 
	{i = j = k = 0.0f;}

VECTOR::VECTOR(float inI, float inJ, float inK) 
	{i = inI; j = inJ; k = inK;}

VECTOR::VECTOR (FLTPOINT3D inP) 
	{i = inP.x ; j = inP.y ; k = inP.z ;}

VECTOR::operator = ( VECTOR inV ) {	
	i = inV.i; j = inV.j; k =inV.k; 
}

VECTOR VECTOR::operator + ( VECTOR inV ) {	
	VECTOR outV;
	outV.i = i + inV.i;	outV.j = j + inV.j;	outV.k = k + inV.k; 
	return outV ;
}

VECTOR VECTOR::operator - ( VECTOR inV ) {	
	VECTOR outV;
	outV.i = i - inV.i;	outV.j = j - inV.j;	outV.k = k - inV.k; 
	return outV ;
}

VECTOR VECTOR::operator * ( VECTOR inV ) {	
	VECTOR outV;
	outV.i = j*inV.k - inV.j*k ; 
	outV.j = inV.i*k - inV.k*i ;	
	outV.k = i*inV.j - j*inV.i ;
	return outV ;
}

VECTOR VECTOR::operator * (float inK ) {	
	VECTOR outV;
	outV.i = i * inK;	outV.j = j * inK;	outV.k = k * inK; 
	return outV ;
}

VECTOR VECTOR::operator / (float inK ) {//check for null value 
	VECTOR outV;                //before calling this function	
	outV.i = i / inK;	outV.j = j / inK;	outV.k = k / inK; 
	return outV ;
}

float VECTOR::modulus() {
	return (float)sqrt(i*i + j*j + k*k);
}
/////////////////////////////////////////////////////////////////////////////////////////////
CString getErrDescription(unsigned long inLngErrCode) {
	
	CString strErrMessage;	
	strErrMessage = "Error:";

	if(inLngErrCode > 99999) {

		strErrMessage += "\nError Source: ";
		
		//Separate the lakh and the rest part
		int intRem = inLngErrCode%100000;
	
		switch(inLngErrCode - intRem) {
			case SLIDESTOOBJECT_ADDSLIDE:
				strErrMessage += "slidesToObject::addSlide";
				break;
		}		
		inLngErrCode = intRem;	
	}		

	
	if(inLngErrCode > 999) {
		
		if(strErrMessage == "Error:")
			strErrMessage += "\nError Source: ";
		else
			strErrMessage += "::";


		//Separate the thousandth and the rest part
		int intRem = inLngErrCode%1000;
		switch(inLngErrCode - intRem) {
		case POINTS_TO_POLYGONS_CONST:
			strErrMessage += "pointsToPolygons::const";
			break;
		case POINTS_TO_POLYGONS_PROCESS:
			strErrMessage += "pointsToPolygons::process";
			break;
		case POINTS_TO_POLYGONS_PROCESS_OBJECT:
			strErrMessage += "pointsToPolygons::processObject";
			break;
		case POINTS_TO_POLYGONS_IS_VALID_CUBE:
			strErrMessage += "pointsToPolygons::isValidCube";
			break;
		case POINTS_TO_POLYGONS_IS_CUBE_POSSIBLE:
			strErrMessage += "pointsToPolygons::isCubePossible";
			break;
		case POINTS_TO_POLYGONS_GET_CUBE:
			strErrMessage += "pointsToPolygons::getCube";
			break;
		case POINTS_TO_POLYGONS_STORE_TRIANGLE_TO_FILE:
			strErrMessage += "pointsToPolygons::storeTriangleToFile";
			break;
		case POINTS_TO_POLYGONS_STORE_TRIANGLE:
			strErrMessage += "pointsToPolygons::storeTriangle";
			break;
		case POINTS_TO_POLYGONS_DRAW_SURFACES:
			strErrMessage += "pointsToPolygons::drawSurfaces";
			break;
		case IMAGES_TO_OBJECT_CONST:
			strErrMessage += "imagesToObject::const";
			break;
		case IMAGES_TO_OBJECT_SET_NEAR_FAR_RECTANGLE:
			strErrMessage += "imagesToObject::setNearFarRectangle";
			break;
		case IMAGES_TO_OBJECT_SET_INVALID:
			strErrMessage += "imagesToObject::setInvalid";
			break;
		case IMAGES_TO_OBJECT_PROCESS_IMAGE:
			strErrMessage += "imagesToObject::processImage";
			break;
		case PLANEDATAPROCESSOR_INITIALIZE:
			strErrMessage += "planeDataProcessor::initialize";
			break;
		case PLANEDATAPROCESSOR_STORESLIDEDATA:
			strErrMessage += "planeDataProcessor::storeSlideData";
			break;
		default:
			strErrMessage += "-";
			break;
		}
		inLngErrCode = intRem;
	}

	if(inLngErrCode > 99) {

		if(strErrMessage == "Error:")
			strErrMessage += "\nError Source: ";
		else
			strErrMessage += "::";

		//Separate the hundredth and the rest part
		int intRem = inLngErrCode%100;
		switch(inLngErrCode - intRem) {
		case FILE_POINTS_CONST:
			strErrMessage += "filePoints::const";
			break;
		case FILE_POINTS_SET_POINT:
			strErrMessage += "filePoints::setPoint";
			break;
		case FILE_CUBES_CONST:
			strErrMessage += "fileCubes::const";
			break;
		case FILE_POLYGONS_CONST:
			strErrMessage += "filePolygons::const";
			break;
		case FILE_READ_POINTS_CONST:
			strErrMessage += "fileReadPoints::const";
			break;
		case FILE_POINTS_RETRIEVE_VALUES:
			strErrMessage += "fileReadPoints::retrieveValues";
			break;
		case FILEPOINTS_INITIALIZE:
			strErrMessage += "filePoints::initialize";
			break;
		case FILEPOINTS_STORESLIDEDATA:
			strErrMessage += "filePoints::storeSlideData";
			break;
		case CUBE_STATUS_INIT_CUBE_STATUS:
			strErrMessage += "cubeStatus::initCubeStatus";
			break;
		default:
			strErrMessage += "-";
			break;
		}
		inLngErrCode = intRem;
	}
	
	strErrMessage += "\n";

	switch(inLngErrCode) {
	case DIVISION_BY_ZERO:
		strErrMessage += "Division by zero";
		break;
	case LOOKING_VECTOR_NULL:
		strErrMessage += "Looking vector null";
		break;
	case FILE_CREATION_ERROR:
		strErrMessage += "File creation error";
		break;
	case NEGATIVE_VALUES:
		strErrMessage += "Negative values";
		break;
	case FILE_NOT_OPEN:
		strErrMessage += "File not open";
		break;
	case BYTE_POS_EXCEEDED:
		strErrMessage += "Byte pos exceeded";
		break;
	case IMAGE_SHOT_OUT_OF_TARGET:
		strErrMessage += "Image shot out of target";
		break;
	case NULL_DIMENSION_FOR_CUBOID:
		strErrMessage += "Null dimension for cuboid";
		break;
	case CAMERA_AT_INFINITY_INSIDE_CUBOID:
		strErrMessage += "Camera at infinity inside cuboid";
		break;
	case INVALID_RANGE_AT_INFINITY_VALUES:
		strErrMessage += "Invalid range at infinity values";
		break;
	case FILE_ACCESS_ERROR:
		strErrMessage += "File access error";
		break;
	case FILE_SIZE_DOES_NOT_MATCH:
		strErrMessage += "File size does not match";
		break;
	case FILE_PARAMS_DO_NOT_MATCH:
		strErrMessage += "File params do not match";
		break;
	case VALUE_EXCEEDED:
		strErrMessage += "Value exceeded";
		break;
	case INSUFFICIENT_POINTS:
		strErrMessage += "Insufficient points";
		break;
	case INDEX_OUT_OF_RANGE:
		strErrMessage += "Index out of range";
		break;
	case VERTICES_NOT_THREE_FOR_TRIANGLE:
		strErrMessage += "Vertices not three for triangle";
		break;
	case MOLD_TYPE_NOT_FOR_SNAPS:
		strErrMessage += "Invalid mold type. Images cannot be used to process this mold.";
		break;
	case MEMORY_ALLOCATION_FAILURE:
		strErrMessage += "Could not allocate memory to temporary buffer.";
		break;
	case MOLD_TYPE_NOT_FOR_SLIDES:
		strErrMessage += "Invalid mold type. Slides cannot be used to process this mold.";
		break;
	case INVALID_IMAGE_DIMENSIONS:
		strErrMessage += "No valid region in the image to process.";
		break;
	case NOT_THE_FIRST_SLIDE:
		strErrMessage += "Tried to change the point density when slides are already added.";
		break;
	case INVALID_NEW_XY_POINTS:
		strErrMessage += "Invalid input for the new X and Y points of cuboid";
		break;
	case NO_POLYGONS_GENERATED:
		strErrMessage += "No polygons could be generated from the mold.";
		break;
	case NO_MORE_SLIDES_POSSIBLE:
		strErrMessage += "The maximum for the number of slides has been reached.\nNo more slides can be processed.";
		break;
	case INVALID_SECURITY_STRING:
		strErrMessage += "Invalid access.";
		break;
	default:
		strErrMessage += "The operation could not be completed due to an internal error.";
		break;

	}

	return strErrMessage;
}


//The following function returns the int part of a float value without rounding off any values,
//Note that this function does take some time and should not be used if time constraint is important
int getIntFromFloat(float inFltVal) {
	CString str;
	str.Format("%f", inFltVal);

	return atoi(str);
}
//----------------------------------------------------------------------------------------------