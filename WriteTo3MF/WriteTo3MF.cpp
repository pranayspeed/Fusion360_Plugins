#include "WriteTo3MF.h"

#include "FusionToTK.h"

Ptr<Application> app;
Ptr<UserInterface> ui;

const std::string commandId = "Write_3MF_File_3DFORMAT";
const std::string commandName = "3MF File Writer";
const std::string commandDescription = "Save Fusion model to 3MF file(*.3mf).";


// Create the command definition.
static Ptr<CommandDefinition> createCommandDefinition()
{
	Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
	if (!commandDefinitions)
		return nullptr;

	// Be fault tolerant in case the command is already added.
	Ptr<CommandDefinition> cmDef = commandDefinitions->itemById(commandId);
	if (!cmDef)
	{
		std::string resourcePath = "./resources";
		cmDef = commandDefinitions->addButtonDefinition(commandId,
			commandName,
			commandDescription,
			resourcePath);// absolute resource file path is specified
	}
	return cmDef;
}


struct TKStruct
{
	TesseKetor::Model_tk* model;
	std::string fileName;
};

static DWORD WINAPI MyThreadFunction(void* pContext)
{
	TKStruct *foo = static_cast<TKStruct*>(pContext);
	Exporter3MF* export3mf = new Exporter3MF(foo->model, foo->fileName);
	bool res =export3mf->ExportModel();
	delete export3mf;
	return res;
}


// CommandDestroyed event handler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{
		adsk::terminate();
	}
}_TesseketorCommandDestroy;


class GearCommandExecuteEventHandler : public adsk::core::CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{
		Ptr<UserInterface> ui = app->userInterface();
		
		//// file dialog
		Ptr<FileDialog> fileDlg = ui->createFileDialog();
		fileDlg->filter("3MF model (*.3mf)\0(.3mf)\0");
		fileDlg->title("Save 3MF File");
		DialogResults dlgResult = fileDlg->showSave();
		std::string fileNanme = fileDlg->filename();
		if (dlgResult == DialogOK && fileNanme.size()>0)
		{
			Ptr<ProgressDialog> prog = ui->createProgressDialog();
			prog->isBackgroundTranslucent(true);
			prog->isCancelButtonShown(true);
			
			prog->show("Writing 3MF", "Processing Please wait...", 0, 100);

			prog->progressValue(20);
			FusionToTK exporter;
			exporter.Traverse(app->activeDocument(),prog);
			prog->progressValue(50);

			TKStruct modelData;
			modelData.model = exporter.GetModel();
			modelData.fileName = fileNanme;
			HANDLE  hThread = CreateThread(NULL, 0, MyThreadFunction, (void*)&modelData, 0L, NULL);
			int t = 0;
			prog->message("Saving File...");
			do
			{
				prog->progressValue(t);
				Sleep(50);
				t=(t+1)%100;
			}
			while (WaitForSingleObject(hThread, 0));
			prog->progressValue(95);
			Sleep(100);
			prog->hide();
		}
	}
}_gearCommandExecute;


// CommandCreated event handler.
class CommandCreatedEventHandler : public adsk::core::CommandCreatedEventHandler
{
public:
	void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override
	{
		if (eventArgs)
		{
			Ptr<Command> command = eventArgs->command();
			if (command)
			{
				Ptr<CommandEvent> executeEvent = command->execute();
				if (!executeEvent)
					return;
				if (!executeEvent->add(&_gearCommandExecute))
					return;
				Ptr<CommandEvent> destroyEvent = command->destroy();
				if (!executeEvent)
					return;
				if (!destroyEvent->add(&_TesseketorCommandDestroy))
					return;
			}
		}
	}

} cmdCreated_;



extern "C" XI_EXPORT bool run(const char* context)
{
	app = Application::get();
	if (!app)
		return false;

	ui = app->userInterface();
	if (!ui)
		return false;

	Ptr<CommandDefinition> command = createCommandDefinition();
	if (!command)
		return false;
	
	Ptr<ToolbarPanel> createPanel = ui->allToolbarPanels()->itemById("SolidScriptsAddinsPanel");//(//->itemById("SolidCreatePanel");

	if (!createPanel)
		return false;
	
	Ptr<CommandControl> colladabutton = createPanel->controls()->addCommand(command);
	if (!colladabutton)
		return false;
	colladabutton->isPromoted(true);
	Ptr<CommandCreatedEvent> commandCreatedEvent = command->commandCreated();
	if (!commandCreatedEvent)
		return false;
	commandCreatedEvent->add(&cmdCreated_);

	return true;
}

extern "C" XI_EXPORT bool stop(const char* context)
{
	if (ui)
	{
		Ptr<ToolbarPanel> createPanel = ui->allToolbarPanels()->itemById("SolidScriptsAddinsPanel");//(//->itemById("SolidCreatePanel");
		if (!createPanel)
			return false;

		Ptr<CommandControl> gearButton1 = createPanel->controls()->itemById(commandId);
		if (gearButton1)
			gearButton1->deleteMe();

		Ptr<CommandDefinition> cmdDef = ui->commandDefinitions()->itemById(commandId);
		if (cmdDef)
			cmdDef->deleteMe();

		ui = nullptr;
	}
	return true;
}


#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif // XI_WIN


TKMatrix Exporter3MF::CreateIdentityTransform()
{
	TKMatrix m;
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			m.a[j][i] = (i == j) ? 1.0f : 0.0f;
		}
	}
	return m;
}

// Utility functions to create vertices and triangles
MODELMESHVERTEX Exporter3MF::fnCreateVertex(float x, float y, float z)
{
	MODELMESHVERTEX result;
	result.m_fPosition[0] = x;
	result.m_fPosition[1] = y;
	result.m_fPosition[2] = z;
	return result;
}

MODELMESHTRIANGLE Exporter3MF::fnCreateTriangle(int v0, int v1, int v2)
{
	MODELMESHTRIANGLE result;
	result.m_nIndices[0] = v0;
	result.m_nIndices[1] = v1;
	result.m_nIndices[2] = v2;
	return result;
}

MODELMESHCOLOR_SRGB Exporter3MF::fnCreateColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char aplha)
{
	MODELMESHCOLOR_SRGB result;
	result.m_Red = red;
	result.m_Green = green;
	result.m_Blue = blue;
	result.m_Alpha = aplha;
	return result;
}

const wchar_t* Exporter3MF::GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}


bool Exporter3MF::Init()
{
	// Check 3MF Library Version
	hResult = lib3mf_getinterfaceversion(&nInterfaceVersion);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not get 3MF Library version: " << std::hex << hResult << std::endl;
		return false;
	}

	if ((nInterfaceVersion != NMR_APIVERSION_INTERFACE)) {
		std::cout << "invalid 3MF Library version: " << NMR_APIVERSION_INTERFACE << std::endl;
		return false;
	}
	return true;
}

bool Exporter3MF::CreateModel(PLib3MFModel*& pModel1)
{
	// Create Model Instance
	hResult = lib3mf_createmodel(&pModel1, true);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not create model: " << std::hex << hResult << std::endl;
		return false;
	}
	return true;
}

bool Exporter3MF::WriteModel(PLib3MFModel*& pModel1, std::string fileName)
{
	// Create Model Writer
	hResult = lib3mf_model_querywriter(pModel, "3mf", &p3MFWriter);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not create model reader: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pModel);
		return false;
	}

	// Export Model into File
	std::cout << "writing colorcube.3mf..." << std::endl;

	hResult = lib3mf_writer_writetofile(p3MFWriter, GetWC(fileName.c_str()));
	if (hResult != LIB3MF_OK) {
		std::cout << "could not write file: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(p3MFWriter, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pModel);
		lib3mf_release(p3MFWriter);
		return false;
	}

	// Release Model Writer
	lib3mf_release(p3MFWriter);

	// Release Model
	lib3mf_release(pModel);
}

bool Exporter3MF::ExportModel()
{
	// initialize the 3MF sdk
	bool res = Init();
	if (!res)
		return false;
	res = CreateModel(pModel);
	if (!res)
		return false;
	// convert Model_TK to 3MF model
	Export();
	WriteModel(pModel, m_fileName);
	return true;
}

Exporter3MF::Exporter3MF(TesseKetor::Model_tk* model, std::string fileName) :TesseKetorTraverser(model), m_fileName(fileName)
{

}
Exporter3MF::~Exporter3MF()
{

}

MODELTRANSFORM Exporter3MF::ExportMatrix(std::vector<float> matrix)
{
	MODELTRANSFORM mMatrix;
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 4; j++) {
			mMatrix.m_fFields[i][j] = matrix[(i * 4) + j];
		}
	}
	return mMatrix;
}

MODELTRANSFORM Exporter3MF::Get3MFTransform(TKMatrix m1)
{
	MODELTRANSFORM mMatrix;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mMatrix.m_fFields[i][j] = m1.a[i][j];
		}
	}
	return mMatrix;
}

void Exporter3MF::ModelStart(TesseKetor::Model_tk* model)
{
	std::string nodeName(model->GetName()), nodeId(model->GetName());
	nodeId.append(std::to_string(model->GetID()));

	// Create model transform for second cube
	TKMatrix matrix;
	matrix = model->GetTransform();
	if (matList.size())
	{
		matrix = matList[0] * matrix;
	}
	matList.insert(matList.begin(), matrix);

}


void Exporter3MF::ModelEnd(TesseKetor::Model_tk* model)
{
	matList.erase(matList.begin());
}
void Exporter3MF::PartStart(TesseKetor::Part_tk* part)
{
	std::string nodeName(part->GetName()), nodeId(part->GetName());
	nodeId.append(std::to_string(part->GetID()));

	TKMatrix matrix;
	matrix = part->GetTransform();
	if (matList.size())
	{
		matrix = matList[0] * matrix;
	}
	matList.insert(matList.begin(), matrix);

}
void Exporter3MF::PartEnd(TesseKetor::Part_tk* part)
{
	matList.erase(matList.begin());
}
void Exporter3MF::BodyStart(TesseKetor::Body_tk* body)
{
	std::string stdName = body->GetName();
	if (stdName.size() == 0)
		stdName.append("Body");
	std::string nodeName(stdName), nodeId(stdName);
	nodeId.append(std::to_string(body->GetID()));

	TKMatrix matrix;
	matrix = body->GetTransform();
	if (matList.size())
	{
		matrix = matList[0] * matrix;
	}
	matList.insert(matList.begin(), matrix);

	// Create Mesh Object
	hResult = lib3mf_model_addmeshobject(pModel, &pMeshObject);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not add mesh object: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pModel, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pModel);
		return;
	}

	hResult = lib3mf_object_setnameutf8(pMeshObject, nodeId.c_str());
	if (hResult != LIB3MF_OK) {
		std::cout << "could not set object name: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return;
	}


	TesseKetor::Face_tk* face = body->GetFaces();
	std::vector<double> vertexList, normalList;
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

	std::vector<MODELMESHVERTEX> mVerList;
	std::vector<MODELMESHTRIANGLE> mTriList;

	for (int i = 0; i < vertexList.size(); i += 3)
	{
		mVerList.push_back(fnCreateVertex(vertexList[i], vertexList[i + 1], vertexList[i + 2]));
	}

	for (int i = 0; i < faceList.size(); i += 3)
	{
		mTriList.push_back(fnCreateTriangle(faceList[i], faceList[i + 1], faceList[i + 2]));
	}

	hResult = lib3mf_meshobject_setgeometry(pMeshObject, &mVerList[0], mVerList.size(), &mTriList[0], mTriList.size());
	if (hResult != LIB3MF_OK) {
		std::cout << "could not set mesh geometry: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return;
	}

	// Create color entries for cube
	hResult = lib3mf_meshobject_createpropertyhandler(pMeshObject, &pPropertyHandler);
	if (hResult != LIB3MF_OK) {
		std::cout << "could not create property handler: " << std::hex << hResult << std::endl;
		lib3mf_getlasterror(pMeshObject, &nErrorMessage, &pszErrorMessage);
		std::cout << "error #" << std::hex << nErrorMessage << ": " << pszErrorMessage << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return;
	}

	double* tkcolor = body->GetRGB();
	if (tkcolor)
	{
		MODELMESHCOLOR_SRGB sModelColor = fnCreateColor(tkcolor[0] * 255, tkcolor[1] * 255, tkcolor[2] * 255);
		for (int i = 0; i < mTriList.size(); i++)
		{
			// One-colored Triangles
			lib3mf_propertyhandler_setsinglecolor(pPropertyHandler, i, &sModelColor);
		}
	}

	// release property handler
	lib3mf_release(pPropertyHandler);

	// Create Component Object
	hResult = lib3mf_model_addcomponentsobject(pModel, &pComponentsObject);
	if (hResult != S_OK) {
		std::cout << "could not add mesh object: " << std::hex << hResult << std::endl;
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return;
	}

	// Create model transform for second cube
	MODELTRANSFORM trnsfm = Get3MFTransform(matList[0]);
	hResult = lib3mf_componentsobject_addcomponent(pComponentsObject, pMeshObject, &trnsfm, &pComponent1);
	if (hResult != S_OK) {
		std::cout << "could not add component 1: " << std::hex << hResult << std::endl;
		lib3mf_release(pComponentsObject);
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return;
	}

	// Release Component 1
	lib3mf_release(pComponent1);

	matList.erase(matList.begin());

	trnsfm = Get3MFTransform(matList[0]);
	// Add Build Item for Component
	hResult = lib3mf_model_addbuilditem(pModel, pComponentsObject, NULL, &pBuildItem);
	if (hResult != S_OK) {
		std::cout << "could not create build item: " << std::hex << hResult << std::endl;
		lib3mf_release(pComponentsObject);
		lib3mf_release(pMeshObject);
		lib3mf_release(pModel);
		return;
	}

	// Release Build Item
	lib3mf_release(pBuildItem);

	// Release Objects
	lib3mf_release(pComponentsObject);
	lib3mf_release(pMeshObject);
}
