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

std::string getDllPath()
{
#if defined(_WINDOWS) || defined(_WIN32) || defined(_WIN64)
	HMODULE hModule = NULL;
	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR)&getDllPath,
		&hModule))
		return "";

	char winTempPath[2048];
	::GetModuleFileNameA(hModule, winTempPath, 2048);

	std::string strPath = winTempPath;
	size_t stPos = strPath.rfind('\\');
	return strPath.substr(0, stPos);
#else
	Dl_info info;
	dladdr((void*)getDllPath, &info);

	std::string strPath = info.dli_fname;
	int stPos = (int)strPath.rfind('/');
	if (stPos != -1)
		return strPath.substr(0, stPos);
	else
		return "";;
#endif
}