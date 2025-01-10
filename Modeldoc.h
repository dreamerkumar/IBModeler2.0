#ifndef MODELDOC_H_
#define MODELDOC_H_

#include "definitions.h"
#include "model.h"

class modelDoc : public CDocument {
	DECLARE_DYNCREATE(modelDoc)

private:
	model myModel;
	CString strFilePath;
	CToolBar modelToolBar;

public:
	BOOL OnNewDocument();
	BOOL OnOpenDocument(LPCSTR filePath);
	void DeleteContents();

	BOOL isModelLoaded();

	CString getFilePath();
	UINT getFileSize();
	RGBCOLOR getColor();
	float * getPoints();
	float * getNormals();
	UINT getVertexCount();
	MODEL_POINT_RANGE getPointRange();

	void OnCloseDocument() ;

	//Function to load the model file
	BOOL loadModel(); 

	//DECLARE_MESSAGE_MAP()
};

#endif