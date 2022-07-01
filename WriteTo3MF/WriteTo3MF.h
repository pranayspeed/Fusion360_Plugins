#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>

#include <windows.h>
#include <string>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

#include "TesseKetor.h"

using namespace TesseKetor;



#ifndef __GCC
#include <tchar.h>
#endif // __GCC

#include <iostream>
#include <algorithm>

// COM Includes of 3MF Library
#include "NMR_DLLInterfaces.h"

// Use NMR namespace for the interfaces
using namespace NMR;


class Exporter3MF : public TesseKetorTraverser
{
private:
	TesseKetor::Model_tk* m_model;
	std::string m_fileName;
	// General Variables
	HRESULT hResult;
	DWORD nInterfaceVersion;
	DWORD nErrorMessage;
	LPCSTR pszErrorMessage;

	// Objects
	PLib3MFModel *pModel;
	PLib3MFModelWriter * p3MFWriter;
	PLib3MFModelMeshObject * pMeshObject;
	PLib3MFModelBuildItem * pBuildItem;
	PLib3MFPropertyHandler * pPropertyHandler;

	MODELTRANSFORM mTransform;

	PLib3MFModelComponentsObject * pComponentsObject;
	PLib3MFModelComponent * pComponent1;

	std::vector<TKMatrix> matList;

public:

	TKMatrix CreateIdentityTransform();

	// Utility functions to create vertices and triangles
	MODELMESHVERTEX fnCreateVertex(float x, float y, float z);

	MODELMESHTRIANGLE fnCreateTriangle(int v0, int v1, int v2);

	MODELMESHCOLOR_SRGB fnCreateColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char aplha =255);

	const wchar_t *GetWC(const char *c);

private:
	bool Init();

	bool CreateModel(PLib3MFModel*& pModel1);

	bool WriteModel(PLib3MFModel*& pModel1, std::string fileName);

public:
	bool ExportModel();

	Exporter3MF(TesseKetor::Model_tk* model, std::string fileName);
	~Exporter3MF();
protected:
	MODELTRANSFORM ExportMatrix(std::vector<float> matrix);

	MODELTRANSFORM Get3MFTransform(TKMatrix m1);

	virtual void ModelStart(TesseKetor::Model_tk* model);


	virtual void ModelEnd(TesseKetor::Model_tk* model);
	virtual void PartStart(TesseKetor::Part_tk* part);
	virtual void PartEnd(TesseKetor::Part_tk* part);
	virtual void BodyStart(TesseKetor::Body_tk* body);
};