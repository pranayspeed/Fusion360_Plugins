// ColloadaIO.h : main header file for the ColloadaIO DLL
//

#pragma once




#include "TesseKetor.h"

#include "COLLADASWStreamWriter.h"
#include "COLLADASWLibrary.h"
//Geomtery
#include "COLLADASWLibraryGeometries.h"
#include "COLLADASWSource.h"
#include "COLLADASWPrimitves.h"

#include "COLLADASWBaseInputElement.h"

#include "COLLADASWLibraryEffects.h"

#include "COLLADASWLibraryNodes.h"
#include "COLLADASWLibraryVisualScenes.h"

#include "COLLADASWNode.h"
#include "COLLADASWInstanceGeometry.h"
#include "COLLADASWInstanceNode.h"
#include "COLLADASWScene.h"
#include "COLLADASWAsset.h"

#include "COLLADASWLibraryEffects.h"
#include "COLLADASWLibraryMaterials.h"

using namespace TesseKetor;

struct CompareColor : public std::binary_function<COLLADASW::Color, COLLADASW::Color, bool> 
{
	bool operator() (COLLADASW::Color a, COLLADASW::Color b) 
	{
		if(a.getRed() < b.getRed())
			return true;
		else if (a.getRed() == b.getRed())
		{
			if (a.getGreen() < b.getGreen())
				return true;
			else if (a.getGreen() == b.getGreen())
			{
				if (a.getBlue() < b.getBlue())
					return true;
			}
		}
		return false;		
	}
};


//global map
std::map<std::string, std::string> m_nodeIdToeffectId;

class MaterialEporter : public COLLADASW::LibraryMaterials, public TesseKetorTraverser
{
private:
	TesseKetor::Model_tk* m_model;
	
public:
	MaterialEporter(COLLADASW::StreamWriter* s, TesseKetor::Model_tk* model) :COLLADASW::LibraryMaterials(s), TesseKetorTraverser(model)
	{
		openLibrary();
	}
	~MaterialEporter()
	{
		closeLibrary();
	}


protected:

	virtual void BodyStart(TesseKetor::Body_tk* body)
	{
		std::string stdName = body->GetName();
		if (stdName.size() == 0)
			stdName.append("Body");		
		COLLADASW::String nodeId = GetUniqueEntityId(body);
		if (m_nodeIdToeffectId[nodeId] == nodeId)
		{
			openMaterial(nodeId + "material", stdName);
			addInstanceEffect("#" + m_nodeIdToeffectId[nodeId] + "effect");
			closeMaterial();
		}
				 
	}
	virtual std::string GetUniqueEntityId(Entity_tk* ent)
	{
		std::string nodeName("Entity");
		nodeName.append(std::to_string(ent->GetID()));
		return nodeName;
	}
};


class EffectEporter : public COLLADASW::LibraryEffects , public TesseKetorTraverser
{
private:
	TesseKetor::Model_tk* m_model;
	std::map<COLLADASW::Color, std::string, CompareColor> m_colorToEffectIdMap;
	
public:
	EffectEporter(COLLADASW::StreamWriter* s, TesseKetor::Model_tk* model) :COLLADASW::LibraryEffects(s), TesseKetorTraverser(model)
	{
		m_colorToEffectIdMap.clear();
		m_nodeIdToeffectId.clear();
		openLibrary();
	}
	~EffectEporter()
	{
		closeLibrary();
	}

protected:

	void BodyStart(TesseKetor::Body_tk* body)
	{
		double* rgbColor = body->GetRGB();
		double* ambtColor = body->GetAmbientRGB();
		double* specColor = body->GetSpecularRGB();
		double transperancy = body->GetTransparency();
		double shineness = body->GetShineness();
		COLLADASW::Color colladaColor(rgbColor[0], rgbColor[1], rgbColor[2], 1.0);
		COLLADASW::Color colladaambtColor(ambtColor[0], ambtColor[1], ambtColor[2], 1.0);
		COLLADASW::Color colladaspecColor(specColor[0], specColor[1], specColor[2], 1.0);

		std::string stdName = body->GetName();
		if (stdName.size() == 0)
			stdName.append("Body");
		COLLADASW::String nodeId = GetUniqueEntityId(body);

		if (m_colorToEffectIdMap.find(colladaColor) == m_colorToEffectIdMap.end())
		{
			openEffect(nodeId + "effect");
			COLLADASW::EffectProfile effect(mSW);
			effect.setShaderType(COLLADASW::EffectProfile::ShaderType::PHONG);
			effect.setDiffuse(COLLADASW::ColorOrTexture(colladaColor));
			effect.setAmbient(COLLADASW::ColorOrTexture(colladaambtColor));
			effect.setSpecular(COLLADASW::ColorOrTexture(colladaspecColor));
			effect.setTransparency(transperancy);
			effect.setShininess(shineness);
			addEffectProfile(effect);
			closeEffect();
			m_colorToEffectIdMap[colladaColor] = nodeId;
			m_nodeIdToeffectId[nodeId] = nodeId;
		}
		else
		{
			m_nodeIdToeffectId[nodeId] = m_colorToEffectIdMap[colladaColor];
		}
	}

	virtual std::string GetUniqueEntityId(Entity_tk* ent)
	{
		std::string nodeName("Entity");
		nodeName.append(std::to_string(ent->GetID()));
		return nodeName;
	}
};


class VisualNodeExporter : public COLLADASW::LibraryVisualScenes, public TesseKetorTraverser
{
private:
	TesseKetor::Model_tk* m_model;
	COLLADASW::Node* m_curNode;
	std::vector<COLLADASW::Node*> nodeList;
public:
	VisualNodeExporter(COLLADASW::StreamWriter* s, TesseKetor::Model_tk* model) :COLLADASW::LibraryVisualScenes(s), TesseKetorTraverser(model)
	{
		openLibrary();
		openVisualScene("Scene");
	}
	~VisualNodeExporter()
	{
		closeVisualScene();
		closeLibrary();
	}
protected:
	void ExportMatrix(COLLADASW::Node*& node,std::vector<float> matrix)
	{
		double mat[4][4];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				mat[j][i] = matrix[(j * 4) + i];
			}
		}
		node->addMatrix(mat);
	}
	
	virtual void ModelStart(TesseKetor::Model_tk* model)
	{
		COLLADASW::Node* node = new COLLADASW::Node(mSW);
		std::string nodeName(model->GetName());
		COLLADASW::String nodeId = GetUniqueEntityId(model);
				
		node->setNodeId(nodeId + "node");
		node->setNodeName(nodeName);
		node->setType(COLLADASW::Node::NODE);
		node->start();
		if (!model->IsIdentityMatrix())
			ExportMatrix(node, model->GetTransform());

		nodeList.insert(nodeList.begin(), node);
	}
	virtual void ModelEnd(TesseKetor::Model_tk* model)
	{
		nodeList[0]->end();
		nodeList.erase(nodeList.begin());
	}
	virtual void PartStart(TesseKetor::Part_tk* part)
	{
		COLLADASW::Node* node = new COLLADASW::Node(mSW);
		std::string nodeName(part->GetName());
		COLLADASW::String nodeId = GetUniqueEntityId(part);
		node->setNodeId(nodeId + "node");
		node->setNodeName(nodeName);
		node->setType(COLLADASW::Node::NODE);
		node->start();
		if (!part->IsIdentityMatrix())
			ExportMatrix(node, part->GetTransform());

		nodeList.insert(nodeList.begin(), node);
	}
	virtual void PartEnd(TesseKetor::Part_tk* part)
	{
		nodeList[0]->end();
		nodeList.erase(nodeList.begin());
	}
	virtual void BodyStart(TesseKetor::Body_tk* body)
	{
		COLLADASW::Node* node = new COLLADASW::Node(mSW);

		std::string stdName = body->GetName();
		if (stdName.size() == 0)
			stdName.append("Body");
		COLLADASW::String nodeName(stdName);

		COLLADASW::String nodeId = GetUniqueEntityId(body);
		node->setNodeId(nodeId + "node");
		node->setNodeName(nodeName);
		node->setType(COLLADASW::Node::NODE);
		node->start();
		if (!body->IsIdentityMatrix())
			ExportMatrix(node, body->GetTransform());

		COLLADASW::InstanceGeometry geomNode(mSW);
		geomNode.setUrl(COLLADASW::URI(EMPTY_STRING, nodeId + "geometry"));
		geomNode.getBindMaterial().getInstanceMaterialList().push_back(COLLADASW::InstanceMaterial(m_nodeIdToeffectId[nodeId] + "material", "#" + m_nodeIdToeffectId[nodeId] + "material"));
		geomNode.add();
		node->end();
	}

	virtual std::string GetUniqueEntityId(Entity_tk* ent)
	{
		std::string nodeName("Entity");
		nodeName.append(std::to_string(ent->GetID()));
		return nodeName;
	}
};

class GeometryExporter : public COLLADASW::LibraryGeometries, public TesseKetorTraverser
{
private:
	TesseKetor::Model_tk* m_model;
public:
	GeometryExporter(COLLADASW::StreamWriter* s, TesseKetor::Model_tk* model) :COLLADASW::LibraryGeometries(s), TesseKetorTraverser(model)
	{
		openLibrary();
	}

	~GeometryExporter()
	{
		closeLibrary();
	}

	virtual std::string GetUniqueEntityId(Entity_tk* ent)
	{
		std::string nodeName("Entity");
		nodeName.append(std::to_string(ent->GetID()));
		return nodeName;
	}
protected:

	virtual void BodyStart(TesseKetor::Body_tk* body)
	{
		std::string stdName = body->GetName();
		if (stdName.size() == 0)
			stdName.append("Body");
		COLLADASW::String bodyname(stdName.c_str());
		COLLADASW::String nodeId = GetUniqueEntityId(body);
		bodyname.append(std::to_string(body->GetID()));
		nodeId.append("geometry");
		ExportShells(body, nodeId, bodyname);
	}

	void ExportNormals(std::vector<double> normals, COLLADASW::String& meshId)
	{
		COLLADASW::FloatSource vertexSource(mSW);
		vertexSource.setId(meshId + NORMALS_SOURCE_ID_SUFFIX);
		vertexSource.setArrayId(meshId + NORMALS_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX);
		vertexSource.setAccessorStride(3);
		vertexSource.setAccessorCount(normals.size()/3);
		vertexSource.getParameterNameList().push_back("X");
		vertexSource.getParameterNameList().push_back("Y");
		vertexSource.getParameterNameList().push_back("Z");
		vertexSource.prepareToAppendValues();
		for (int i = 0; i < normals.size(); ++i)
		{
			vertexSource.appendValues(normals[i]);
		}
		vertexSource.finish();
	}
	
	void ExportPoints(std::vector<double> vertices, COLLADASW::String& meshId)
	{
		COLLADASW::FloatSource vertexSource(mSW);
		vertexSource.setId(meshId + POSITIONS_SOURCE_ID_SUFFIX);
		vertexSource.setArrayId(meshId + POSITIONS_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX);
		vertexSource.setAccessorStride(3);
		vertexSource.setAccessorCount(vertices.size()/3);
		vertexSource.getParameterNameList().push_back("X");
		vertexSource.getParameterNameList().push_back("Y");
		vertexSource.getParameterNameList().push_back("Z");
		vertexSource.prepareToAppendValues();
		for (int i = 0; i < vertices.size(); ++i)
		{
			vertexSource.appendValues(vertices[i]);
		}
		vertexSource.finish();
	}


	void ExportShells(TesseKetor::Body_tk* body, COLLADASW::String& meshId, const COLLADASW::String& geomteryName = EMPTY_STRING)
	{
		openMesh(meshId, geomteryName);
		TesseKetor::Face_tk* face = body->GetFaces();
		std::vector<double> vertexList,normalList;
		std::vector<double> faceList;
		int faceIndex = 0;
		while (face)
		{
			std::vector<double> tmpVList;
			vertexList.insert(vertexList.end(), face->GetVertices(), face->GetVertices() + (face->GetVertexCount() * 3));
			normalList.insert(normalList.end(), face->GetNormals(), face->GetNormals() + (face->GetNormalCount() * 3));
			faceList.push_back(faceIndex++);
			faceList.push_back(faceIndex++);
			faceList.push_back(faceIndex++);
			face = face->GetNext();
		}

		ExportPoints(vertexList, meshId);
		ExportNormals(normalList, meshId);
		ExportVertices(meshId);

		COLLADASW::Triangles triangle(mSW);
		triangle.setCount(faceList.size() / 3);

		triangle.getInputList().push_back(COLLADASW::Input(COLLADASW::InputSemantic::VERTEX, COLLADASW::URI(COLLADABU::Utils::EMPTY_STRING, meshId + VERTICES_ID_SUFFIX),0));
		triangle.getInputList().push_back(COLLADASW::Input(COLLADASW::InputSemantic::NORMAL, COLLADASW::URI(COLLADABU::Utils::EMPTY_STRING, meshId + NORMALS_SOURCE_ID_SUFFIX),0));
		triangle.prepareToAppendValues();
		
		for (size_t i = 0; i < faceList.size(); i += 3)
		{
			triangle.appendValues(faceList[i], faceList[i + 1], faceList[i + 2]);
		}
		
		closeMesh();
		closeGeometry();
	}

	void GeometryExporter::ExportVertices(const COLLADASW::String& meshId)
	{
		COLLADASW::VerticesElement vertices(mSW);
		vertices.setId(meshId + VERTICES_ID_SUFFIX);
		//vertices.setNodeName("vertex");
		COLLADASW::InputList* inputList = &vertices.getInputList();
		inputList->push_back(COLLADASW::Input(COLLADASW::InputSemantic::POSITION, COLLADASW::URI(COLLADABU::Utils::EMPTY_STRING, meshId + POSITIONS_SOURCE_ID_SUFFIX)));
		vertices.add();
	}


};



class ColladaIO
{

private:
	COLLADASW::StreamWriter* m_writer;
	TesseKetor::Model_tk* m_model;
public :
	ColladaIO(TesseKetor::Model_tk* model,std::string fileName) :m_model(model)
	{
		if (fileName.size() == 0)
			fileName = "D:\\test.dae";

		COLLADASW::NativeString fName(fileName);		
		m_writer = new COLLADASW::StreamWriter(fName,true);				
		m_writer->startDocument();
		COLLADASW::Asset asset(m_writer);
		asset.setSubject("3d Model");
		asset.getContributor().mAuthor = "TesseKetor";
		asset.setUnit("meter", 1.0);
		asset.add();
	}
	virtual ~ColladaIO()
	{
		m_writer->endDocument();
		delete m_writer;
	}

	void Export()
	{
		ExportEffects();
		ExportMaterials();
		ExportGeometryLibrary();
		ExportNodesLibrary();
		ExportVisualSceneNodesLibrary();

		COLLADASW::Scene scene(m_writer, COLLADASW::URI("#Scene"));
		scene.add();
	}

	void ExportEffects()
	{
		EffectEporter* exporter = new EffectEporter(m_writer, m_model);
		exporter->Export();
		delete exporter;
	}

	void ExportMaterials()
	{
		MaterialEporter* exporter = new MaterialEporter(m_writer, m_model);
		exporter->Export();
		delete exporter;
	}

	void ExportGeometryLibrary()
	{
		GeometryExporter* exporter = new GeometryExporter(m_writer, m_model);
		exporter->Export();
		delete exporter;
	}
	void ExportNodesLibrary()
	{
		//TODO: support nodes
	}
	void ExportVisualSceneNodesLibrary()
	{
		VisualNodeExporter* exporter = new VisualNodeExporter(m_writer, m_model);
		exporter->Export();
		delete exporter;
	}
};