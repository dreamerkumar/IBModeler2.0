#include "model.h"
#include "dialogs.h"

model::model() { 
	arrPoints = arrNormals = NULL;
	blnModelLoaded = false;
}

UINT model::getFileSize() { return intFileSize; }


CString model::loadModel(CString strFilePath){
	CFile fp;
	if(!fp.Open(strFilePath, CFile::modeRead))
		return "The file could not be opened for reading";

	try { 
			
		intFileSize = fp.GetLength();
		//If it is a proper model file, then at least one polygon
		//should be present.
		if(intFileSize < 63) {
			if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
				fp.Close();
			return "Invalid file size.";
		}
		fp.SeekToBegin();
		if(fp.Read(&modelRange.fltMinx, 4) != 4)
			throw new CException();
		if(fp.Read(&modelRange.fltMaxx, 4) != 4)
			throw new CException();
		if(fp.Read(&modelRange.fltMiny, 4) != 4)
			throw new CException();
		if(fp.Read(&modelRange.fltMaxy, 4) != 4)
			throw new CException();
		if(fp.Read(&modelRange.fltMinz, 4) != 4)
			throw new CException();
		if(fp.Read(&modelRange.fltMaxz, 4) != 4)
			throw new CException();
		if(fp.Read(&modelColor.red, 1) != 1)
			throw new CException();
		if(fp.Read(&modelColor.green, 1) != 1)
			throw new CException();
		if(fp.Read(&modelColor.blue, 1) != 1)
			throw new CException();
		
		UINT size = fp.GetLength() - 27;

		intVertices = size/12; //Each point will have an x, y and z value of size 4(float type) 

		//Start allcating the memory

		arrPoints  = (float*)malloc(size);
		if(arrPoints == NULL) {
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();
			return "Could not allocate memory to load the model.";
		}

		arrNormals = (float*)malloc(size); 
		if(arrNormals == NULL) {
			if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
				fp.Close();
			clearMemory();
			return "Could not allocate memory to load the model.";
		}

		 //Read all the points at one go
		if(fp.Read(arrPoints, size)!= size)
			throw new CException();
			
		

	} catch(CException *exception) {
		exception->Delete();
		if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
			fp.Close();
		clearMemory();		
		return "Error occured while reading from the file.";
	}

	if(fp.m_hFile != (UINT)(-1) && fp.m_hFile != NULL)
		fp.Close();
	calcNormals(); 
	blnModelLoaded = true;
	return "SUCCESS";
}

BOOL model::isModelLoaded() {  return blnModelLoaded; }

RGBCOLOR model::getModelColor() { return modelColor; }

float* model::getPoints(){ 	return arrPoints; }

float* model::getNormals() { return arrNormals;	}

UINT model::getVertexCount() { return intVertices; }

MODEL_POINT_RANGE model::getPointRange() { return modelRange; }


void model::calcNormals() {	
	UINT intCtr;
	UINT intTriangles = intVertices/3;
	float *ptPoints = arrPoints;
	float *ptNormals = arrNormals;
	FLTPOINT3D p1, p2, p3;
	VECTOR normalVector;

	for(intCtr = 1; intCtr <= intTriangles; intCtr++) {
		
		p1.x = *ptPoints; p1.y = *(ptPoints+1); p1.z = *(ptPoints +2);
		ptPoints+= 3;

		p2.x = *ptPoints; p2.y = *(ptPoints+1); p2.z = *(ptPoints +2);
		ptPoints+= 3;

		p3.x = *ptPoints; p3.y = *(ptPoints+1); p3.z = *(ptPoints +2);
		ptPoints+= 3;
		
		normalVector = calcNormal(p1, p2, p3);
		*ptNormals = *(ptNormals +3) = *(ptNormals +6) = normalVector.i;
		*(ptNormals+1) = *(ptNormals +4) = *(ptNormals +7) = normalVector.j;
		*(ptNormals+2) = *(ptNormals +5) = *(ptNormals +8) = normalVector.k;
		ptNormals+=9;		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
VECTOR model::calcNormal(FLTPOINT3D p1, FLTPOINT3D p2, FLTPOINT3D p3 ) {

	VECTOR outV;
	float mod;
	outV = VECTOR(p1-p2) * VECTOR(p2-p3);
	mod  = outV.modulus();
	if(mod == 0 ) 
		return VECTOR(0.0f, 0.0f, 0.0f); //Return a null vector to avoid division by zero
	else		
		return outV/mod;

}
	
void model::clearMemory() {
	if(arrPoints != NULL) {
		delete arrPoints;
	}
	if(arrNormals != NULL) {
		delete arrNormals;
	}
	blnModelLoaded = false;
}

model::~model(){
	clearMemory();
}
	
	
	
	
