#include "stdafx.h"
#include "FusionToTK.h"


FusionToTK::FusionToTK()
{
	m_model = NULL;
	m_currModel = NULL;
}

FusionToTK::~FusionToTK()
{
}

Model_tk* FusionToTK::GetModel()
{
	return m_model;
}

bool FusionToTK::Traverse(Ptr<Document> doc, Ptr<ProgressDialog> proDlg)
{
	m_proDlg = proDlg;
	ProgressCallback(10);
	m_modelName = doc->name();
	Ptr<FusionDocument> fdoc = doc;
	Ptr<Design> design = fdoc->design();
	Ptr<Component> rootComp = design->rootComponent();
	if (rootComp)
	{
		ModelStart(m_modelName);
		Ptr<BRepBodies> bbodies = rootComp->bRepBodies();
		if (bbodies)
		{
			if (bbodies->count())
			{
				std::string occName = rootComp->name();
				PartStart(occName);
				for (Ptr<BRepBodies>::iterator bodyitr = bbodies.begin(); bodyitr != bbodies.end(); bodyitr++)
				{
					ExportBody(*bodyitr);
				}
				PartEnd();
			}
		}
		Ptr<Matrix3D> transform;
		if (rootComp->occurrences())
			Traverse(rootComp->occurrences()->asList(), transform);
		ModelEnd();

		ProgressCallback(10);
		return true;
	}
	return false;
}

// Performs a recursive traversal of an entire assembly structure.
void FusionToTK::Traverse(Ptr<OccurrenceList> occurrences, Ptr<Matrix3D> transform)
{
	if (occurrences) {

		for (size_t i = 0; i < occurrences->count(); ++i) {
			ProgressCallback(10);
			Ptr<Occurrence> occ = occurrences->item(i);
			if (occ) {
				std::string occName = occ->name();
				ModelStart(occName);
				Ptr<Component> comp = occ->component();
				Ptr<BRepBodies> bbodies = occ->bRepBodies();
				Ptr<Matrix3D> currTransform = occ->transform();
				//if (currTransform)
				//	ExportTransform(currTransform);
				if (bbodies)
				{
					if (bbodies->count())
					{
						PartStart(occName);
						for (Ptr<BRepBodies>::iterator bodyitr = bbodies.begin(); bodyitr != bbodies.end(); bodyitr++)
						{
							ExportBody(*bodyitr);
						}
						PartEnd();
					}
				}
				
				if (occ->childOccurrences())
				{
					Traverse(occ->childOccurrences());
				}
				ModelEnd();
				//Traverse(comp->occurrences()->asList());
			}
		}
	}

}


void FusionToTK::ExportTransform(Ptr<Matrix3D> transform)
{
	if (m_currModel)
	{
		if (transform)
		{
			std::vector<double> mat = transform->asArray();
			std::vector<float> fmat;
			fmat.insert(fmat.begin(), mat.begin(), mat.end());
			m_currModel->SetMatrix(&fmat[0]);
		}
	}
}


void FusionToTK::ModelStart(std::string modelName)
{
	ProgressCallback(0);
	Model_tk* model = Model_tk::CreateModel(modelName.c_str());
	if (m_model == NULL)
		m_model = model;
	else
		m_currModel->AddModel(model);
	m_currModel = model;
}


void FusionToTK::ModelEnd()
{
	m_currModel = m_currModel->GetParent();
}

void FusionToTK::PartStart(std::string partName)
{
	m_part = Part_tk::CreatePart(partName.c_str());
	if (m_model == NULL)
	{
		m_model = Model_tk::CreateModel(m_modelName.c_str());
		m_currModel = m_model;
	}
}

void FusionToTK::PartEnd()
{
	m_currModel->AddPart(m_part);
}


void FusionToTK::BodyStart(std::vector<double> pointList, std::vector<int> faceList, std::string partName, std::vector<double> normalList, std::vector<double> rgbColorList)
{
	//Part_tk* part = Part_tk::CreatePart(partName.c_str());
	Body_tk* body = Body_tk::CreateBody(partName);
	if (rgbColorList.size() > 0)
	{
		body->SetRGB(rgbColorList[0], rgbColorList[1], rgbColorList[2]);
		body->SetTransparency(rgbColorList[3]);
		body->SetShineness(rgbColorList[4]);
		body->SetAmbientRGB(0.0, 0.0, 0.0);
		body->SetSpecularRGB(1.0, 1.0, 1.0);
	}
	else
		body->SetRGB(0.6, 0.6, 0.6);

	Face_tk* face = NULL;
	for (int i = 0; i < faceList.size() / 3; i++)
	{
		double vert[9], norm[9];
		for (size_t j = 0; j < 3; j++)
		{
			int pointIndex = faceList[i * 3 + j];
			for (int k = 0; k < 3; k++)
			{
				vert[j * 3 + k] = pointList[pointIndex * 3 + k];
			}
		}

		for (size_t j = 0; j < 3; j++)
		{
			int pointIndex = faceList[i * 3 + j];
			for (int k = 0; k < 3; k++)
			{
				norm[j * 3 + k] = normalList[pointIndex * 3 + k];
			}
		}

		face = Face_tk::CreateFace(3, vert, norm);
		body->AddFace(face);
	}

	m_part->AddBody(body);
	//m_currModel->AddPart(part);
}

void FusionToTK::ExportBody(Ptr<BRepBody> body)
{
	std::string bodyName = body->name();
	Ptr<MeshManager> meshMrg = body->meshManager();

	Ptr<TriangleMeshList> triMeshlist = meshMrg->displayMeshes();
	if (triMeshlist)
	{
		
		Ptr<TriangleMesh> triMesh = triMeshlist->bestMesh();
		if (triMesh)
		{
			std::vector<double> pointList(triMesh->nodeCoordinatesAsDouble());
			std::vector<double> normalList(triMesh->normalVectorsAsDouble());
			std::vector<int> faceList(triMesh->nodeIndices());

			//std::vector<double> textureCoo(triMesh->textureCoordinatesAsDouble());

			std::vector<double> rgbColorList;
			GetMaterial(rgbColorList, body);
			BodyStart(pointList, faceList, bodyName, normalList, rgbColorList);
		}
	}
}

void FusionToTK::GetOneColor(Ptr<Properties> properties, std::vector<double>& colorList)
{
	if (properties == nullptr)
		return;
	for (int i = 0; i < properties->count(); i++)
	{
		Ptr<Property> prop = properties->item(i);

		if (prop->objectType() == ColorProperty::classType())
		{
			Ptr<ColorProperty> colorprop = nullptr;
			colorprop = prop;
			if (colorprop != nullptr)
			{
				try
				{
					short r, g, b, a;
					if (!colorprop->hasMultipleValues() && colorprop->value())
					{
						colorprop->value()->getColor(r, g, b, a);
						colorList.push_back((double)r*1.0 / 255.0);
						colorList.push_back((double)g*1.0 / 255.0);
						colorList.push_back((double)b *1.0 / 255.0);
						break;
					}
				}
				catch (std::exception e)
				{

				}
			}
		}

	}
}

void FusionToTK::GetMaterial(std::vector<double>& colorList, Ptr<BRepBody> body)
{

	Ptr<Properties> apperProp = body->appearance()->appearanceProperties();

	double transparency = 0.0;
	double shineness = 0.1;
	if (apperProp != nullptr)
	{
		//GetOneColor(apperProp, colorList);
		Ptr<ColorProperty> colorprop = nullptr;
		if (apperProp->itemById("metal_f0"))
		{
			colorprop = apperProp->itemById("metal_f0");
			Ptr<FloatProperty> roughnessProp = apperProp->itemById("surface_roughness");
			if (roughnessProp)
				shineness = 1.0 - roughnessProp->value();
		}
		else if (apperProp->itemById("opaque_albedo"))
		{
			colorprop = apperProp->itemById("opaque_albedo");
			Ptr<FloatProperty> roughnessProp = apperProp->itemById("surface_roughness");
			if (roughnessProp)
				shineness = 1.0-roughnessProp->value();

		}
		else if (apperProp->itemById("layered_diffuse"))
		{
			colorprop = apperProp->itemById("layered_diffuse");
		}
		else if (apperProp->itemById("transparent_color"))
		{
			colorprop = apperProp->itemById("transparent_color");
			Ptr<FloatProperty> roughnessProp = apperProp->itemById("surface_roughness");
			if (roughnessProp)
				transparency = 1.0-roughnessProp->value();
		}
		else if (apperProp->itemById("wood_early_color"))
		{
			colorprop = apperProp->itemById("wood_early_color");
		}
		else if (apperProp->itemById("metal_color"))
		{
			colorprop = apperProp->itemById("metal_color");
			Ptr<FloatProperty> roughnessProp = apperProp->itemById("surface_roughness");
			if (roughnessProp)
				shineness = 1.0-roughnessProp->value();
		}
		else if (apperProp->itemById("generic_diffuse"))
		{
			colorprop = apperProp->itemById("generic_diffuse");
		}

		if (colorprop)
		{
			short r, g, b, a;
			if (colorprop->value())
			{
				colorprop->value()->getColor(r, g, b, a);
				colorList.push_back((double)r / 255.0);
				colorList.push_back((double)g / 255.0);
				colorList.push_back((double)b / 255.0);
				colorList.push_back(transparency);
				colorList.push_back(shineness);
			}
		}
		else
		{
			GetOneColor(apperProp, colorList);
		}


	}
}