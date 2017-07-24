#include <windows.h>

#pragma once
LPTSTR wstring_to_LPTSTR(std::wstring const & wstr);

int WriteToFile(std::string fileToWrite, std::string content);

DWORD RunSilent(const char* strFunct, const char* strstrParams);

bool CheckIfWordInFile(char *word, char *fileName);

bool StrReplace(std::string& str, const std::string& from, const std::string& to);
