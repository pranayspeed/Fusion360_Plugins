




#ifndef TESSEKETOR_H
#define TESSEKETOR_H

#if defined(_DLLPROJECT)
#if defined(_WINDOWS) || defined(WIN32)
#ifdef TESSEKETOR_EXPORT_DLL
#define TESSEKETOR_EXPORT __declspec(dllexport)
#else
#define TESSEKETOR_EXPORT __declspec(dllimport)
#endif
#endif
#else
#define TESSEKETOR_EXPORT
#endif


#include <vector>
#include <string>
 namespace TesseKetor
{
#define STRUCTURE_TK

#ifdef STRUCTURE_TK

	enum EntityType{
		MODEL_TK,
		PART_TK,
		BODY_TK
	};
	
	class TESSEKETOR_EXPORT Camera_tk
	{
	private:
		Camera_tk(double* leftVec, double* upVec, double* viewVec, double* pos,double* target,double distance)
		{
			m_upVec[0] = upVec[0]; m_upVec[1] = upVec[1]; m_upVec[2] = upVec[2];
			m_leftVec[0] = leftVec[0]; m_leftVec[1] = leftVec[1]; m_leftVec[2] = leftVec[2];
			m_viewVec[0] = viewVec[0]; m_viewVec[1] = viewVec[1]; m_viewVec[2] = viewVec[2];
			m_camPos[0] = pos[0]; m_camPos[1] = pos[1]; m_camPos[2] = pos[2];
			m_camTarget[0] = target[0]; m_camTarget[1] = target[1]; m_camTarget[2] = target[2];
			m_distance = distance;
			m_camName = "Camera";
			m_camName.append(std::to_string(camId++));
		}
	public:
		static Camera_tk* CreateCamera(double* leftVec, double* upVec, double* viewVec, double* pos, double* target, double distance)
		{
			return new Camera_tk(leftVec, upVec, viewVec, pos, target, distance);
		}

		double* GetUpVec(){ return m_upVec; }
		double* GetLeftVec(){ return m_leftVec; }
		double* GetViewVec(){ return m_viewVec; }
		double* GetCameraPosition(){ return m_camPos; }
		double* GetCameraTarget(){ return m_camTarget; }
		double GetDistance(){ return m_distance; }

		void SetUpVec(double* upVec)
		{
			m_upVec[0] = upVec[0]; m_upVec[1] = upVec[1]; m_upVec[2] = upVec[2];
		}
		void SetLeftVec(double* leftVec)
		{
			m_leftVec[0] = leftVec[0]; m_leftVec[1] = leftVec[1]; m_leftVec[2] = leftVec[2];
		}
		void SetViewVec(double* viewVec)
		{
			m_viewVec[0] = viewVec[0]; m_viewVec[1] = viewVec[1]; m_viewVec[2] = viewVec[2];
		}
		void SetCameraPosition(double* pos)
		{
			m_camPos[0] = pos[0]; m_camPos[1] = pos[1]; m_camPos[2] = pos[2];
		}

		void SetCameraTarget(double* target)
		{
			m_camTarget[0] = target[0]; m_camTarget[1] = target[1]; m_camTarget[2] = target[2];
		}
		void SetDistance(double distance)
		{
			m_distance = distance;
		}


		void SetCameraName(std::string name)
		{
			m_camName = name;
		}

		std::string GetCameraName()
		{
			return m_camName;
		}
	protected:
		double m_upVec[3];
		double m_leftVec[3];
		double m_viewVec[3];
		double m_camPos[3];
		double m_camTarget[3];
		double m_distance;
		std::string m_camName;
		static int camId;
	};


	class TESSEKETOR_EXPORT Entity_tk
	{
	protected:
		std::string m_name;
		int m_id;
		EntityType m_type;
		std::vector<float> m_matrix;
		bool m_isIdentity;
		static int id;
		double m_diffuseRGB[3];
		double m_ambientRGB[3];
		double m_SpecularRGB[3];
		double m_transparency;
		double m_shineness;
	public:
		
		Entity_tk(std::string name, EntityType type)
		{
			m_name = name;
			m_id = id++;
			m_type = type;
			int i, j;
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					m_matrix.push_back((i == j) ? 1.0f : 0.0f);
				}
			}

			m_isIdentity = true;
			m_diffuseRGB[0] = 0.5;
			m_diffuseRGB[1] = 0.5;
			m_diffuseRGB[2] = 0.5;
			m_ambientRGB[0] = 0.0;
			m_ambientRGB[1] = 0.0;
			m_ambientRGB[2] = 0.0;
			m_SpecularRGB[0] = 1.0;
			m_SpecularRGB[1] = 1.0;
			m_SpecularRGB[2] = 1.0;
			m_transparency = 0.0;//defualt no transparency
			m_shineness = 0.5;
		}

		void SetShineness(double shine)
		{
			m_shineness = shine;
		}
		double GetShineness()
		{
			return m_shineness;
		}

		void SetTransparency(double transp)
		{
			m_transparency = transp;
		}
		double GetTransparency()
		{
			return m_transparency;
		}

		bool IsIdentityMatrix(){
			return m_isIdentity;
		}

		void SetRGB(double r, double g, double b)
		{
			m_diffuseRGB[0] = r;
			m_diffuseRGB[1] = g;
			m_diffuseRGB[2] = b;
		}
		double* GetRGB()
		{
			return m_diffuseRGB;
		}

		void SetAmbientRGB(double r, double g, double b)
		{
			m_ambientRGB[0] = r;
			m_ambientRGB[1] = g;
			m_ambientRGB[2] = b;
		}
		double* GetAmbientRGB()
		{
			return m_ambientRGB;
		}

		void SetSpecularRGB(double r, double g, double b)
		{
			m_SpecularRGB[0] = r;
			m_SpecularRGB[1] = g;
			m_SpecularRGB[2] = b;
		}
		double* GetSpecularRGB()
		{
			return m_SpecularRGB;
		}
		
		std::vector<float>& GetTransform()
		{
			return m_matrix;
		}

		void SetMatrix(float* m)
		{
			m_isIdentity = false;
			for (int i = 0; i < 16; i++)
			{
				m_matrix[i] = m[i];
			}
		}

		EntityType GetEntityType()
		{
			return m_type;
		}
		std::string GetName(){ return m_name; }
		int GetID(){ return m_id; }


		virtual~Entity_tk()
		{

		}

	};
	class Model_tk;
	class Part_tk;
	class Body_tk;
	class TESSEKETOR_EXPORT Face_tk
	{
	private:
		int m_vertexCount;
		double* m_vertices;
		int m_normalCount;
		double* m_normals;
		Body_tk* m_parent;
		Face_tk* m_next;
	public:
		static Face_tk* CreateFace(int count,double* vertex,double* normal=NULL)
		{
			return new Face_tk(count,vertex,normal);
		}
		Face_tk* GetNext()
		{
			return m_next;
		}

		void SetNext(Face_tk* next)
		{
			m_next = next;
		}
		void SetParent(Body_tk* parent)
		{
			m_parent = parent;
		}
		
		Body_tk* GetParent(){ return m_parent; }

		int GetVertexCount(){ return m_vertexCount; }
		double* GetVertices(){ return m_vertices; }
		int GetNormalCount(){ return m_normalCount; }
		double* GetNormals(){ return m_normals; }

	private:
		Face_tk(int vertexcount, double* vertex, double* normal=NULL)
		{
			m_vertexCount = vertexcount;

			m_vertices = new double[vertexcount * 3];
			for (int i = 0; i < vertexcount * 3; i++)
			{
				m_vertices[i] = vertex[i];
			}
			m_parent = NULL;
			m_next = NULL;
			m_normalCount = 0;
			m_normals = NULL;
			if (normal)
			{
				m_normalCount = vertexcount;
				m_normals = new double[vertexcount * 3];
				for (int i = 0; i < vertexcount * 3; i++)
				{
					m_normals[i] = normal[i];
				}
			}
		}
		virtual~Face_tk()
		{

		}

	};


	class TESSEKETOR_EXPORT Body_tk : public Entity_tk
	{
	public:
		static Body_tk* CreateBody(std::string name)
		{
			return new Body_tk(name);
		}

		void AddFace(Face_tk* face)
		{
			face->SetParent(this);
			if (m_faces == NULL)
			{
				m_faces = face;
			}
			else
			{
				Face_tk* node = m_faces;
				while (node->GetNext() != NULL)
				{
					node = node->GetNext();
				}
				node->SetNext(face);
			}
			m_faceCount++;
		}

		Face_tk* GetFaces()
		{
			return m_faces;
		}

		Body_tk* GetNext()
		{
			return m_next;
		}

		void SetNext(Body_tk* next)
		{
			m_next = next;
		}

		void SetParent(Part_tk* parent)
		{
			m_parent = parent;
		}

		Part_tk* GetParent(){ return m_parent; }
		int GetFaceCount(){return m_faceCount; }
	private:
		int m_faceCount;
		Face_tk* m_faces;
		Part_tk* m_parent;
		Body_tk* m_next;

		Body_tk() :Entity_tk("body" + std::to_string(id), BODY_TK)
		{
			m_faceCount = 0;
			m_faces = NULL;
			m_parent = NULL;
			m_next = NULL;
		}

		Body_tk(std::string name) :Entity_tk(name, BODY_TK)
		{
			m_faceCount = 0;
			m_faces = NULL;
			m_parent = NULL;
			m_next = NULL;
		}

		virtual~Body_tk(){}
	};

	class TESSEKETOR_EXPORT Part_tk : public Entity_tk
	{
	private:
		Part_tk(std::string name)
			:Entity_tk(name, PART_TK)
		{
			m_bodyCount = 0;
			m_bodies = NULL;
			m_parent = NULL;
			m_next = NULL;
		}
		virtual~Part_tk(){}
		int m_bodyCount;
		Body_tk* m_bodies;
		Model_tk* m_parent;
		Part_tk* m_next;
	public:
		Part_tk* GetNext()
		{
			return m_next;
		}

		void SetNext(Part_tk* next)
		{
			m_next = next;
		}

		void AddBody(Body_tk* body)
		{
			body->SetParent(this);
			if (m_bodies == NULL)
			{
				m_bodies = body;
			}
			else
			{
				Body_tk* node = m_bodies;
				while (node->GetNext() != NULL)
				{
					node = node->GetNext();
				}
				node->SetNext(body);
			}
			m_bodyCount++;
		}


		void SetParent(Model_tk* parent)
		{
			m_parent = parent;
		}

		Model_tk* GetParent(){ return m_parent; }


		Body_tk* GetBodies(){ return m_bodies; }
		static Part_tk* CreatePart(std::string name)
		{
			return new Part_tk(name);
		}


	};

	class TESSEKETOR_EXPORT Model_tk : public Entity_tk
	{
	protected:
		int m_childCount;
		int m_PartCount;
		Model_tk* m_parent;
		Model_tk* m_child;
		Part_tk* m_Parts;
		Model_tk* m_next;
		bool m_hasCamera;
		std::vector<Camera_tk*>* m_cameras;
	public:

		void AddCamera(Camera_tk*& cam)
		{
			m_cameras->push_back(cam);
		}

		std::vector<Camera_tk*> GetCameras()
		{
			return *m_cameras;
		}

		static Model_tk* CreateModel(std::string name)
		{
			return new Model_tk(name);			
		}

		void AddModel(Model_tk* model)
		{
			Model_tk* node = m_child;
			model->SetParent(this);
			if (node == NULL)
			{
				m_child = model;
			}
			else
			{
				while (node->m_next != NULL)
				{
					node = node->m_next;
				}
				node->m_next = model;
			}
			m_childCount++;
		}
		void AddPart(Part_tk* part)
		{
			part->SetParent(this);
			if (m_Parts == NULL)
			{
				m_Parts = part;
			}
			else
			{
				Part_tk* node = m_Parts;
				while (node->GetNext() != NULL)
				{
					node = node->GetNext();
				}
				node->SetNext(part);
			}
			m_PartCount++;
		}

		Part_tk* GetPart()
		{
			return m_Parts;
		}
		Model_tk* GetNext()
		{
			return m_next;
		}

		void SetParent(Model_tk* parent)
		{
			m_parent = parent;
		}

		Model_tk* GetParent(){ return m_parent; }

		Model_tk* GetChildModel(){ return m_child; }
		bool hasCamara(){return m_hasCamera;}

	private:

		Model_tk(std::string name)
			:Entity_tk(name,MODEL_TK)
		{
			m_childCount = 0;
			m_PartCount = 0;
			m_parent = NULL;
			m_child = NULL;
			m_next = NULL;
			m_Parts = NULL;
			m_hasCamera = false;
			m_cameras = new std::vector<Camera_tk*>();
			m_cameras->clear();
		}
		
		virtual~Model_tk()
		{
		}


	};

	class TesseKetorTraverser
	{
	protected:
		TesseKetor::Model_tk* m_model;
		virtual void ModelStart(TesseKetor::Model_tk* model){}
		virtual void ModelEnd(TesseKetor::Model_tk* model){}
		virtual void PartStart(TesseKetor::Part_tk* model){}
		virtual void PartEnd(TesseKetor::Part_tk* model){}
		virtual void BodyStart(TesseKetor::Body_tk* model){}

	public:
		TesseKetorTraverser(TesseKetor::Model_tk* model) :m_model(model){}
		virtual ~TesseKetorTraverser(){}

		void Export(TesseKetor::Model_tk* model = NULL)
		{
			if (!model)
				model = m_model;
			TesseKetor::Model_tk* tmp = model;

			while (tmp != NULL)
			{
				ModelStart(tmp);
				Part_tk* parts = tmp->GetPart();
				while (parts != NULL)
				{
					TraversePart(parts);
					parts = parts->GetNext();
				}
				TesseKetor::Model_tk* tmpChild = tmp->GetChildModel();
				if (tmpChild)
					Export(tmpChild);
				tmp = tmp->GetNext();
				ModelEnd(tmp);
			}
		}

		virtual std::string GetUniqueEntityId(Entity_tk* ent)
		{
			return std::to_string(ent->GetID());
		}
	protected:
		void TraversePart(TesseKetor::Part_tk* part)
		{
			PartStart(part);
			TesseKetor::Body_tk* body = part->GetBodies();
			while (body != NULL)
			{
				BodyStart(body);
				body = body->GetNext();
			}
			PartEnd(part);
		}
	};

	struct TKMatrix
	{
		float a[4][4];

		TKMatrix operator*(const TKMatrix& m1)
		{
			TKMatrix res;
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					res.a[i][j] = 0;
					for (int k = 0; k < 4; k++)
					{
						res.a[i][j] += a[i][k] * m1.a[k][j];
					}
				}
			}
			return res;
		}

		void operator=(std::vector<float> m1)
		{
			int t = 0;
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					a[i][j] = m1[t++];
				}
			}
		}

	};


	//class Part;
	//class Model
	//{
	//public:
	//	Model(const char* ModelName)
	//	{
	//		m_model = (Model_tk*)CreateEntity(MODEL_TK);
	//		m_model->name = new char[strlen(ModelName) + 1];
	//		memcpy(m_model->name,ModelName,strlen(ModelName)+1);
	//		
	//		m_Parts = NULL;
	//	}
	//	virtual ~Model()
	//	{
	//		if (m_model)
	//			delete m_model;
	//		if (m_Parts)
	//			delete m_Parts;
	//	}


	//	void AddModel(Model* model)
	//	{
	//		m_model->child
	//	}
	//	void AddModel(Model_tk* model)
	//	{
	//		Entity_tk* node = m_model->child;
	//		while (node->next != NULL)
	//		{
	//			node = node->next;
	//		}
	//		m_model->child = model;
	//		((Model_tk*)m_model->child)->name = new char[strlen(model->name) + 1];
	//		memcpy(((Model_tk*)m_model->child)->name, model->name, strlen(model->name) + 1);
	//		((Model_tk*)m_model->child)->childCount++;
	//		((Model_tk*)m_model->child)->id = model->id;
	//		((Model_tk*)m_model->child)->next = NULL;
	//		((Model_tk*)m_model->child)->parent = m_model;
	//		
	//	}

	//	void AddPart(Part_tk* part)
	//	{
	//		Entity_tk* node = m_model->child;
	//		while (node->next != NULL)
	//		{
	//			node = node->next;
	//		}
	//		m_model->child = part;
	//		((Model_tk*)m_model->child)->name = new char[strlen(part->name) + 1];
	//		memcpy(((Model_tk*)m_model->child)->name, part->name, strlen(part->name) + 1);
	//		((Model_tk*)m_model->child)->childCount++;
	//		((Model_tk*)m_model->child)->id = part->id;
	//		((Model_tk*)m_model->child)->next = NULL;
	//		((Model_tk*)m_model->child)->parent = m_model;

	//	}


	//	Model_tk* GetModels()
	//	{
	//		m_model;
	//	}

	//	Part* GetParts()
	//	{
	//		m_Parts;
	//	}
	//	static int id;

	//	static Entity_tk* CreateEntity(EntityType type)
	//	{
	//		if (type == MODEL_TK)
	//		{
	//			Model_tk* model = new Model_tk[sizeof(Model_tk)];
	//			model->child = NULL;
	//			model->childCount = 0;
	//			model->id = id++;
	//			model->name = NULL;
	//			model->next = NULL;
	//			model->parent = NULL;
	//			model->type = MODEL_TK;
	//			return model;
	//		}
	//		else if (type == PART_TK)
	//		{
	//			Part_tk* part = new Part_tk[sizeof(Part_tk)];
	//			part->bodies = NULL;
	//			part->bodyCount = 0;
	//			part->id= id++;
	//			part->name = NULL;
	//			part->next= NULL;
	//			part->parent = NULL;
	//			part->type = PART_TK;
	//			return part;
	//		}
	//	}

	//private:
	//	Model_tk* m_model;
	//	Part* m_Parts;
	//	
	//};

	//class Part
	//{
	//public:
	//	Part()
	//	{
	//	}
	//	virtual~Part()
	//	{

	//	}
	//};



#else


	class BaseEntity
	{
	public:
		BaseEntity();
		BaseEntity(std::string& name);
		~BaseEntity();

		int GetID();
	protected:
		int m_id;
		std::string m_name;

		static int id;
	};

	class Part;
	class Transform;
	class Model : public BaseEntity
	{
	public:
		TESSEKETOR_EXPORT Model(std::string& name);
		TESSEKETOR_EXPORT virtual ~Model();

		TESSEKETOR_EXPORT int GetModelsCount();
		TESSEKETOR_EXPORT std::vector<Model*> GetModels();
		TESSEKETOR_EXPORT std::vector<Part*> GetParts(){ return m_Parts; }
		TESSEKETOR_EXPORT Transform* GetTransform();

		TESSEKETOR_EXPORT void AddModel(Model* model){ m_Models.push_back(model); }
		TESSEKETOR_EXPORT void RemoveModel(int index){ m_Models.erase(m_Models.begin() + index); }
		TESSEKETOR_EXPORT void AddPart(Part* part){ m_Parts.push_back(part); }
		TESSEKETOR_EXPORT void RemovePart(int index){ m_Parts.erase(m_Parts.begin() + index); }

	private:
		std::vector<Model*> m_Models;
		int m_numOfModels;
		Transform* m_transform;
		std::vector<Part*> m_Parts;
		
	};

	class Materials;
	class Body;
	class Part : public BaseEntity
	{
	public:
		TESSEKETOR_EXPORT Part(std::string& name);
		TESSEKETOR_EXPORT virtual ~Part();
		TESSEKETOR_EXPORT int GetBodyCount();
		TESSEKETOR_EXPORT std::vector<Body*> GetBodies();
		TESSEKETOR_EXPORT Transform* GetTransform();

		TESSEKETOR_EXPORT void AddBody(Body& body){ m_Bodies.push_back(&body); }
		TESSEKETOR_EXPORT void RemoveBody(int index){ m_Bodies.erase(m_Bodies.begin() + index); }


	private:
		int m_numberOfBodies;
		std::vector<Body*> m_Bodies;
		Transform* m_transform;

	};
	class Face;
	class Body : public BaseEntity
	{
	public:
		TESSEKETOR_EXPORT Body();
		TESSEKETOR_EXPORT virtual ~Body();
		TESSEKETOR_EXPORT int GetFaceCount();
		TESSEKETOR_EXPORT std::vector<Face*> GetFaces();

		TESSEKETOR_EXPORT void AddFace(Face& face){ n_Faces.push_back(&face); m_numberOfFaces++; }
		TESSEKETOR_EXPORT void RemoveFace(int index){ n_Faces.erase(n_Faces.begin() + index); }

	private:
		int m_numberOfFaces;
		std::vector<Face*> n_Faces;
	};

	class Vertex
	{
	public:
		TESSEKETOR_EXPORT Vertex(){
			m_x = 0.0; m_y = 0.0; m_z = 0.0;
		}
		TESSEKETOR_EXPORT Vertex(double x, double y, double z){
			m_x = x; m_y = y; m_z = z;
		}
		TESSEKETOR_EXPORT ~Vertex()
		{

		}
		TESSEKETOR_EXPORT Vertex operator=(Vertex& a)
		{
			m_x = a.m_x; m_y = a.m_y; m_z = a.m_z;
			return a;
		}
		TESSEKETOR_EXPORT double GetX(){ return m_x; }
		TESSEKETOR_EXPORT double GetY(){ return m_y; }
		TESSEKETOR_EXPORT double GetZ(){ return m_z; }
	private:
		double m_x;
		double m_y;
		double m_z;

	};
	class Face
	{
	public:
		TESSEKETOR_EXPORT Face(){};
		TESSEKETOR_EXPORT Face(Vertex* vertices);
		TESSEKETOR_EXPORT Face(Vertex& x, Vertex& y, Vertex& z);
		TESSEKETOR_EXPORT virtual ~Face();
		TESSEKETOR_EXPORT void GetVertices(Vertex& x, Vertex& y, Vertex& z){ x = m_v1; y = m_v2; y = m_v3; }
		TESSEKETOR_EXPORT Vertex GetX(){ return m_v1; }
		TESSEKETOR_EXPORT Vertex GetY(){ return m_v3; }
		TESSEKETOR_EXPORT Vertex GetZ(){ return m_v3; }

		//TESSEKETOR_EXPORT void SetVertices(int i, Vertex* vertices){

		//	m_vertices[i] = vertices;
		//}
		TESSEKETOR_EXPORT void SetX(Vertex x){ m_v1 = x; }
		TESSEKETOR_EXPORT void SetY(Vertex y){ m_v2 = y; }
		TESSEKETOR_EXPORT void SetZ(Vertex z){ m_v3 = z; }
	private:

		Vertex m_v1;
		Vertex m_v2;
		Vertex m_v3;
	};




	class Transform
	{
	public:
		TESSEKETOR_EXPORT Transform();
		TESSEKETOR_EXPORT virtual ~Transform();
		TESSEKETOR_EXPORT void SetTransform(float* matrix){ m_matrix = matrix; }
		TESSEKETOR_EXPORT float* GetTransform(){ return m_matrix; }

	private:
		float* m_matrix;
	};


	class Material
	{
	public:
		TESSEKETOR_EXPORT Material();
		TESSEKETOR_EXPORT Material(float r, float g, float b, float a = 1.0);
		TESSEKETOR_EXPORT virtual~Material();
		TESSEKETOR_EXPORT float* GetRGBA(){ return m_rgba; }
		TESSEKETOR_EXPORT void SetRGBA(float r, float g, float b, float a = 1.0)
		{ 
			m_rgba[0] = r;
			m_rgba[1] = g;
			m_rgba[2] = b;
			m_rgba[3] = a;
		}
		TESSEKETOR_EXPORT void SetRed(float r){ m_rgba[0] = r; }
		TESSEKETOR_EXPORT void SetGreen(float g){ m_rgba[1] = g; }
		TESSEKETOR_EXPORT void SetBlue(float b){ m_rgba[2] = b; }
		TESSEKETOR_EXPORT void SetAlpha(float a){ m_rgba[3] = a; }
	private:
		float* m_rgba;
	};
#endif
}



#endif