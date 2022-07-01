#include "ColladaWriter.h"

#include "ColloadaIO.h"

#include "FusionToTK.h"

Ptr<Application> app;
Ptr<UserInterface> ui;

const std::string commandId = "TesseKetor_Convert_To_Collada";
const std::string commandName = "TesseKetor_Command Collada Export";
const std::string commandDescription = "Write Collada Model(*.dae).";


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


// CommandDestroyed event handler
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{
		adsk::terminate();
	}
}_TesseketorCommandDestroy;

struct TKStruct
{
	TesseKetor::Model_tk* model;
	std::string fileName;
};

static DWORD WINAPI MyThreadFunction(void* pContext)
{
	TKStruct *foo = static_cast<TKStruct*>(pContext);
	ColladaIO* exporter = new ColladaIO(foo->model, foo->fileName);
	exporter->Export();
	delete exporter;
	return true;
}




class GearCommandExecuteEventHandler : public adsk::core::CommandEventHandler
{
public:
	void notify(const Ptr<CommandEventArgs>& eventArgs) override
	{

		Ptr<UserInterface> ui = app->userInterface();

		//// file dialog

		Ptr<FileDialog> fileDlg = ui->createFileDialog();
		fileDlg->filter("Collada Model file (*.dae)\0(.dae)\0");
		fileDlg->title("Save Collada");
		DialogResults dlgResult = fileDlg->showSave();
		std::string fileNanme = fileDlg->filename();
		if (dlgResult == DialogOK && fileNanme.size()>0)
		{
			Ptr<ProgressDialog> prog = ui->createProgressDialog();
			prog->isBackgroundTranslucent(true);
			prog->isCancelButtonShown(true);

			prog->show("Collada Writing....", "Processing....", 0, 100);

			prog->progressValue(20);
			FusionToTK exporter;
			exporter.Traverse(app->activeDocument(),prog);
			prog->progressValue(50);

			TKStruct modelData;
			modelData.model = exporter.GetModel();
			modelData.fileName = fileNanme;
			HANDLE  hThread = CreateThread(NULL, 0, MyThreadFunction, (void*)&modelData, 0L, NULL);
			int t = 0;
			prog->message("Saving...");
			do
			{
				prog->progressValue(t);
				Sleep(100);
				t = (t + 1) % 100;
			} while (WaitForSingleObject(hThread, 0));
			prog->progressValue(95);
			Sleep(100);
			//delete colladaIo;
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
		Ptr<ToolbarPanel> createPanel = ui->allToolbarPanels()->itemById("SolidScriptsAddinsPanel");
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


std::ofstream outFile;

void ExportProperties(Ptr<Properties> properties)
{
	if (properties == nullptr)
		return;
	for (int i = 0; i < properties->count(); i++)
	{
		Ptr<Property> prop = properties->item(i);

		outFile << prop->id() << " :  \tObjectType : " << prop->objectType() << ":";
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
						outFile << "(" << colorprop->value()->red() << "," << colorprop->value()->green() << "," << colorprop->value()->blue() << "," << colorprop->value()->opacity() << ")";
					}
					else
					{
						if (colorprop->hasMultipleValues())
						{
							outFile << "\nMultipleValues:";
							for (size_t j = 0; j < colorprop->values().size(); j++)
							{
								outFile << "(" << colorprop->values()[j]->red() << "," << colorprop->values()[j]->green() << "," << colorprop->values()[j]->blue() << "," << colorprop->values()[j]->opacity() << ")";
							}
						}
						else
						{
							outFile << "NoValues";
						}
					}
				}
				catch (std::exception e)
				{

				}
			}
		}
		else if (prop->objectType() == AppearanceTextureProperty::classType())
		{
			Ptr<AppearanceTextureProperty> txProp = prop;
			Ptr<AppearanceTexture> appTx = txProp->value();
			if (appTx)
			{
				ExportProperties(appTx->properties());
			}

		}
		else if (prop->objectType() == FloatProperty::classType())
		{
			outFile << prop->id() << "=" << ((Ptr<FloatProperty>)prop)->value();
		}
		else if (prop->objectType() == StringProperty::classType())
		{
			outFile << prop->id() << "=" << ((Ptr<StringProperty>)prop)->value();
		}
		else if (prop->objectType() == BooleanProperty::classType())
		{
			outFile << prop->id() << "=" << ((Ptr<BooleanProperty>)prop)->value();
		}
		else if (prop->objectType() == IntegerProperty::classType())
		{
			outFile << prop->id() << "=" << ((Ptr<IntegerProperty>)prop)->value();
		}
		else if (prop->objectType() == FilenameProperty::classType())
		{
			outFile << prop->id() << "=" << ((Ptr<FilenameProperty>)prop)->value();
		}
		outFile << std::endl;

	}
}


void ExportBodyProperties(Ptr<BRepBody> body)
{
	std::string fname("D:\\Testing\\");
	fname.append(body->name());
	fname.append(".txt");

	Ptr<Properties> apperProp = body->appearance()->appearanceProperties();
	if (apperProp != nullptr)
	{
		outFile.open(fname, std::ios::app);
		outFile << body->name() << "======================\n";
		ExportProperties(apperProp);
		outFile.close();
	}
}



void GetOneColor(Ptr<Properties> properties, std::vector<double>& colorList)
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
					
					}
					//else
					//{
					//	if (colorprop->hasMultipleValues())
					//	{
		
					//		//for (size_t j = 0; j < colorprop->values().size(); j++)
					//		//{
					//		//	outFile << "(" << colorprop->values()[j]->red() << "," << colorprop->values()[j]->green() << "," << colorprop->values()[j]->blue() << "," << colorprop->values()[j]->opacity() << ")";
					//		//}
					//	}

					//}
				}
				catch (std::exception e)
				{

				}
			}
		}

	}
}




const wchar_t *GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}
