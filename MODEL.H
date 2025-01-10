#ifndef _MODEL_H_
#define _MODEL_H_

#include <afxwin.h>
#include "definitions.h"

class model {
public:
	model();
	~model();
	CString loadModel(CString strFilePath);
	UINT getFileSize();
	BOOL isModelLoaded();
	RGBCOLOR getModelColor();
	float* getPoints();
	float* getNormals();
	UINT getVertexCount();
	MODEL_POINT_RANGE getPointRange();
private:
	void clearMemory();
	void calcNormals();	
	VECTOR calcNormal(FLTPOINT3D p1, FLTPOINT3D p2, FLTPOINT3D p3 );

	BOOL blnModelLoaded;

	//Model data stored in the file-----------------------------------------------
	MODEL_POINT_RANGE modelRange; //Range of points	
	
	RGBCOLOR modelColor; //Default color
	
	float *arrPoints;	//x, y, z values of all the vertices in sequence 	
	//----------------------------------------------------------------------------
	
	UINT intFileSize; //Size of the file
	
	UINT intVertices;
	
	float *arrNormals;

};

#endif