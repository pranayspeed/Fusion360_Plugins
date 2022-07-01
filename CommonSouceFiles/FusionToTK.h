#pragma once
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

typedef void(*FUSION_PROGRESS_CALLBACK)(double, Ptr<ProgressDialog>);

class FusionToTK
{
protected:
	std::string m_modelName;
	Model_tk* m_model;
	Model_tk* m_currModel;
	Part_tk* m_part;
	Ptr<ProgressDialog> m_proDlg;
public:
	FusionToTK();
	virtual ~FusionToTK();

	virtual bool Traverse(Ptr<Document> doc,Ptr<ProgressDialog> proDlg=nullptr);

	virtual void SetModelName(std::string modelName){ m_modelName = modelName; }

	virtual void Traverse(Ptr<OccurrenceList> occurrences, Ptr<Matrix3D> transform = NULL);

	virtual void ModelStart(std::string);
	virtual void ModelEnd();

	virtual void PartStart(std::string);
	virtual void PartEnd();

	virtual void BodyStart(std::vector<double> pointList, std::vector<int> faceList, std::string partName, std::vector<double> normalList, std::vector<double> rgbColorList);

	virtual void GetOneColor(Ptr<Properties> properties, std::vector<double>& colorList);
	virtual void GetMaterial(std::vector<double>& colorList, Ptr<BRepBody> body);
	void ExportTransform(Ptr<Matrix3D> transform);
	virtual void ExportBody(Ptr<BRepBody> body);

	Model_tk* GetModel();

	virtual void ProgressCallback(float perInc)
	{
		if (m_proDlg)
		{
			int progVal = m_proDlg->progressValue();
			m_proDlg->progressValue((progVal + 2) % 100);
		}
	}
};