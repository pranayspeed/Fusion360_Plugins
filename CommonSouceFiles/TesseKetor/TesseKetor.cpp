// TessKater.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "TesseKetor.h"

#ifdef STRUCTURE_TK

int TesseKetor::Entity_tk::id = 0;
int TesseKetor::Camera_tk::camId = 0;
#else
int TesseKetor::BaseEntity::id = 0;

TesseKetor::BaseEntity::BaseEntity()
{
	m_name = "";
}
TesseKetor::BaseEntity::BaseEntity(std::string& name)
{
	m_id=id++;
	m_name = name;
}

TesseKetor::BaseEntity::~BaseEntity()
{

}

int TesseKetor::BaseEntity::GetID()
{
	return m_id;
}


TesseKetor::Model::Model(std::string& name) : BaseEntity(name)
{
	m_numOfModels = 0;
	m_transform = new Transform();
}

TesseKetor::Model::~Model()
{
	if (m_transform)
		delete m_transform;
	m_transform = NULL;

}

int TesseKetor::Model::GetModelsCount()
{
	return m_numOfModels;
}

std::vector<TesseKetor::Model*> TesseKetor::Model::GetModels()
{
	return m_Models;
}

TesseKetor::Transform* TesseKetor::Model::GetTransform()
{
	return m_transform;
}


TesseKetor::Part::Part(std::string& name) : BaseEntity(name)
{
	m_numberOfBodies = 0;
	m_transform = new Transform();
}

TesseKetor::Part::~Part()
{
	
}

int TesseKetor::Part::GetBodyCount()
{
	return m_numberOfBodies;
}
std::vector<TesseKetor::Body*> TesseKetor::Part::GetBodies()
{
	return m_Bodies;
}

TesseKetor::Transform* TesseKetor::Part::GetTransform()
{
	return m_transform;
}


TesseKetor::Body::Body()
{
	m_numberOfFaces = 0;
}

TesseKetor::Body::~Body()
{

}
int TesseKetor::Body::GetFaceCount()
{
	return m_numberOfFaces;
}
std::vector<TesseKetor::Face*> TesseKetor::Body::GetFaces()
{
	return n_Faces;
}



TesseKetor::Face::Face(Vertex* vertices)
{
	if (vertices)
	{
		//for (int i = 0; i < 3;i++)
			m_v1 =vertices[0];
			m_v2 = vertices[1];
			m_v3 = vertices[2];
	}
		
}

TesseKetor::Face::Face(Vertex& x, Vertex& y, Vertex& z)
{
	m_v1 = x;

	m_v2=y;
	m_v3=z;
	//m_vertices[0] = &x;
	//m_vertices[1] = &y;
	//m_vertices[2] = &z;
}

TesseKetor::Face::~Face()
{

}



TesseKetor::Transform::Transform()
{
	m_matrix = new float[16];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i==j)
				m_matrix[i + j] = 1.0;
			else
				m_matrix[i + j] = 0.0;
		}
	}
	
}

TesseKetor::Transform::~Transform()
{
	if (m_matrix)
		delete[] m_matrix;
}


TesseKetor::Material::Material()
{
	m_rgba = new float[4];
	SetRGBA(0.0,1.0,0.0, 1.0);
}

TesseKetor::Material::Material(float r, float g, float b, float a)
{
	m_rgba = new float[4];
	SetRGBA(r, g, b, a);
}

TesseKetor::Material::~Material()
{
	if (m_rgba)
		delete[] m_rgba;
}

#endif