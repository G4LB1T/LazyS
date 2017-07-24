#include <windows.h>
#include <string>
#include <cstring>
#include <mbstring.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <iostream>



int WriteToFile(std::string fileToWrite, std::string content) {

	std::fstream fs;
	//std::cout << fileToWrite << " " << content << std::endl;
	fs.open(fileToWrite, std::fstream::in | std::fstream::out | std::fstream::app);

	fs << content;

	fs.close();

	return 0;
}

LPTSTR wstring_to_LPTSTR(std::wstring const & wstr)
{
#ifdef _UNICODE
	size_t const size = wstr.size() + 1;
	TCHAR * const tstr = new TCHAR[size];
	_tcscpy_s(tstr, size, wstr.c_str());
#else
	size_t const size = (wstr.size() + 1) * sizeof(wchar_t);
	TCHAR * const tstr = new TCHAR[size];
#ifdef _MBCS
	size_t retval = 0;
	errno_t const status = wcstombs_s(&retval, tstr, size, wstr.c_str(), size);
	assert(status == 0);
	assert(retval != -1);
#else
	int const status = sprintf_s(tstr, size, "%S", wstr.c_str());
	assert(status != -1);
#endif
#endif
	return tstr;
}

bool StrReplace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

bool CheckIfWordInFile(char *word, char *fileName) {
	char aWord[5000];
	std::ifstream file(fileName);
	while (file.good()) {
		file >> aWord;
		//std::cout << aWord << std::endl;
		//std::cout << word << std::endl;

		if (file.good() && strcmp(aWord, word) == 0) {
			return true;
		}
	}
	return false;
}


DWORD RunSilent(const char* strFunct, const char* strstrParams)
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	char Args[4096];
	char *pEnvCMD = NULL;
	char *pDefaultCMD = "CMD.EXE";
	ULONG rc;

	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	Args[0] = 0;

	pEnvCMD = getenv("COMSPEC");

	if (pEnvCMD) {

		strcpy(Args, pEnvCMD);
	}
	else {
		strcpy(Args, pDefaultCMD);
	}

	// "/c" option - Do the command then terminate the command window
	strcat(Args, " /c ");
	//the application you would like to run from the command window
	strcat(Args, strFunct);
	strcat(Args, " ");
	//the parameters passed to the application being run from the command window.
	strcat(Args, strstrParams);

	printf(Args);

	if (!CreateProcess(NULL, Args, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&StartupInfo,
		&ProcessInfo))
	{
		printf("%d",GetLastError());
		return GetLastError();
	}

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	if (!GetExitCodeProcess(ProcessInfo.hProcess, &rc))
		rc = 0;

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);
	//printf("done");

	return rc;

}
