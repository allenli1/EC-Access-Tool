/*
 *  EC-Access-Tool Copyright(C) 2021, File Modified by: Shubham Paul under GPLv3
 *
 *	This program is free software : you can redistribute it and /or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

/*
 *  The code to interface with RwDrv.sys is part of https://github.com/Cr4sh/fwexpl which is released under GPLv3.
 */

 //-----------------------------------------------------------------------------
 //     Modified by : Soberia
 //        Web	    : https://github.com/Soberia
 //    License      : The modified BSD license
 // 
 //    Copyright(C) 2021
 //-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                          Copyright 2007 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <VersionHelpers.h>

#include "driver.hpp"

DriverManager::DriverManager(LPCTSTR DriverId) 
	: gHandle(INVALID_HANDLE_VALUE)
	, DriverId(DriverId)
{}

BOOL DriverManager::manage(LPCTSTR DriverPath, USHORT Function) {
	BOOL rCode = FALSE;
	DWORD error = NO_ERROR;
	SC_HANDLE hService = NULL;
	SC_HANDLE hSCManager = NULL;

	if (DriverId == NULL || DriverPath == NULL) {
		return FALSE;
	}

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		return FALSE;
	}

	switch (Function) {
	case OLS_DRIVER_INSTALL:
		if (installDriver(hSCManager, DriverPath)) {
			rCode = startDriver(hSCManager);
		}
		break;
	case OLS_DRIVER_REMOVE:
		if (!isSystemInstallDriver(hSCManager, DriverPath))	{
			stopDriver(hSCManager);
			rCode = removeDriver(hSCManager);
		}
		break;
	case OLS_DRIVER_SYSTEM_INSTALL:
		if (isSystemInstallDriver(hSCManager, DriverPath)) {
			rCode = TRUE;
		} else	{
			if (!openDriver()) {
				stopDriver(hSCManager);
				removeDriver(hSCManager);
				if (installDriver(hSCManager, DriverPath)) {
					startDriver(hSCManager);
				}
				openDriver();
			}

			hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
			if (hService != NULL) {
				rCode = ChangeServiceConfig(
					hService,
					SERVICE_KERNEL_DRIVER,
					SERVICE_AUTO_START,
					SERVICE_ERROR_NORMAL,
					DriverPath,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL);
				CloseServiceHandle(hService);
			}
		}
		break;
	case OLS_DRIVER_SYSTEM_UNINSTALL:
		if (!isSystemInstallDriver(hSCManager, DriverPath)) {
			rCode = TRUE;
		} else {
			if (gHandle != INVALID_HANDLE_VALUE) {
				CloseHandle(gHandle);
				gHandle = INVALID_HANDLE_VALUE;
			}

			if (stopDriver(hSCManager)) {
				rCode = removeDriver(hSCManager);
			}
		}
		break;
	default:
		rCode = FALSE;
		break;
	}

	if (hSCManager != NULL) {
		CloseServiceHandle(hSCManager);
	}
	return rCode;
}

BOOL DriverManager::installDriver(SC_HANDLE hSCManager, LPCTSTR DriverPath) {
	SC_HANDLE hService = NULL;
	BOOL rCode = FALSE;
	DWORD error = NO_ERROR;

	hService = CreateService(
		hSCManager,
		DriverId,
		DriverId,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		DriverPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (hService == NULL) {
		error = GetLastError();
		if (error == ERROR_SERVICE_EXISTS) {
			rCode = TRUE;
		}
	} else {
		rCode = TRUE;
		CloseServiceHandle(hService);
	}

	return rCode;
}

BOOL DriverManager::removeDriver(SC_HANDLE hSCManager) {
	SC_HANDLE hService = NULL;
	BOOL rCode = FALSE;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
	if (hService == NULL) {
		rCode = TRUE;
	} else {
		rCode = DeleteService(hService);
		CloseServiceHandle(hService);
	}

	return rCode;
}

BOOL DriverManager::startDriver(SC_HANDLE hSCManager) {
	SC_HANDLE hService = NULL;
	BOOL rCode = FALSE;
	DWORD error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
	if (hService != NULL) {
		if (!StartService(hService, 0, NULL)) {
			error = GetLastError();
			if (error == ERROR_SERVICE_ALREADY_RUNNING) {
				rCode = TRUE;
			}
		} else {
			rCode = TRUE;
		}
		CloseServiceHandle(hService);
	}
	return rCode;
}

BOOL DriverManager::stopDriver(SC_HANDLE hSCManager) {
	SC_HANDLE hService = NULL;
	BOOL rCode = FALSE;
	SERVICE_STATUS serviceStatus;
	DWORD error = NO_ERROR;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
	if (hService != NULL) {
		rCode = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		error = GetLastError();
		CloseServiceHandle(hService);
	}

	return rCode;
}

BOOL DriverManager::isSystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverPath) {
	SC_HANDLE hService = NULL;
	BOOL rCode = FALSE;
	DWORD dwSize;
	LPQUERY_SERVICE_CONFIG lpServiceConfig;

	hService = OpenService(hSCManager, DriverId, SERVICE_ALL_ACCESS);
	if (hService != NULL) {
		QueryServiceConfig(hService, NULL, 0, &dwSize);
		lpServiceConfig = (LPQUERY_SERVICE_CONFIG)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
		QueryServiceConfig(hService, lpServiceConfig, dwSize, &dwSize);

		if (lpServiceConfig != nullptr && lpServiceConfig->dwStartType == SERVICE_AUTO_START) {
			rCode = TRUE;
		}

		CloseServiceHandle(hService);
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpServiceConfig);
	}

	return rCode;
}

BOOL DriverManager::openDriver() {
	std::wstring DriverId_(DriverId);
	std::wstring DriverId__ = L"\\\\.\\" + DriverId_;

	gHandle = CreateFile(
		DriverId__.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (gHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	return TRUE;
}

WINAPI Driver::Driver(LPCTSTR DriverId, LPCTSTR DriverFileName) : DriverManager(DriverId), driverFileExist(), gDriverPath() {
	_tcscpy_s(gDriverFileName, MAX_PATH, DriverFileName);
}

BOOL WINAPI Driver::initialize() {
	if (gInitDll == FALSE) {
		if (driverFileExistence() == OLS_DLL_NO_ERROR) {
			for (int i = 0; i < 4; i++)	{
				if (openDriver()) {
					gDllStatus = OLS_DLL_NO_ERROR;
					break;
				}

				manage(gDriverPath, OLS_DRIVER_REMOVE);
				if (!manage(gDriverPath, OLS_DRIVER_INSTALL)) {
					gDllStatus = OLS_DLL_DRIVER_NOT_LOADED;
					continue;
				}

				if (openDriver()) {
					gDllStatus = OLS_DLL_NO_ERROR;
					break;
				}

				Sleep(100 * i);
			}
		}
		gInitDll = TRUE;
	}

	return gDllStatus == OLS_DLL_NO_ERROR;
}

VOID WINAPI Driver::deinitialize() {
	BOOL isHandel = gHandle != INVALID_HANDLE_VALUE;
	if (gInitDll == TRUE && isHandel) {
		CloseHandle(gHandle);
		gHandle = INVALID_HANDLE_VALUE;
		manage(gDriverPath, OLS_DRIVER_REMOVE);

		if (isHandel) {
			CloseHandle(gHandle);
			gHandle = INVALID_HANDLE_VALUE;
		}
		gInitDll = FALSE;
	}
}

BYTE Driver::driverFileExistence() {
	TCHAR* ptr;
	TCHAR root[4];
	TCHAR dir[MAX_PATH];
	HANDLE hFile;
	WIN32_FIND_DATA findData;

	if (gDriverType == OLS_DRIVER_TYPE_UNKNOWN && IsWindowsVersionOrGreater(5, 0, 0)) {
		gDllStatus = OLS_DLL_NO_ERROR;
		gDriverType = OLS_DRIVER_TYPE_WIN_NT_X64;
//#ifndef _WIN64
//		BOOL wow64 = FALSE;
//		IsWow64Process(GetCurrentProcess(), &wow64);
//		if (!wow64)
//		{
//			gDriverType = OLS_DRIVER_TYPE_WIN_NT;
//			_tcscpy_s(gDriverFileName, MAX_PATH, OLS_DRIVER_FILE_NAME_WIN_NT);
//		}
//#endif
	}

	GetModuleFileName(NULL, dir, MAX_PATH);
	if ((ptr = _tcsrchr(dir, '\\')) != NULL) {
		*ptr = '\0';
	}
	wsprintf(gDriverPath, _T("%s\\%s"), dir, gDriverFileName);

	// Check file existence
	hFile = FindFirstFile(gDriverPath, &findData);
	if (hFile != INVALID_HANDLE_VALUE) {
		FindClose(hFile);
	} else {
		return OLS_DLL_DRIVER_NOT_FOUND;
	}

	// Check file is not on network location
	root[0] = gDriverPath[0];
	root[1] = ':';
	root[2] = '\\';
	root[3] = '\0';
	if (root[0] == '\\' || GetDriveType((LPCTSTR)root) == DRIVE_REMOTE) {
		return OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK;
	}
	driverFileExist = TRUE;
	return OLS_DLL_NO_ERROR;
}

WINAPI WinRing0::WinRing0(LPCTSTR DriverId, LPCTSTR DriverFileName) : Driver(DriverId, DriverFileName), bResult(0), bytesReturned(0)
{}

VOID WINAPI WinRing0::readIoPortByte(BYTE& port, BYTE& value) {
	bResult = DeviceIoControl(
		gHandle,
		IOCTL_OLS_READ_IO_PORT_BYTE,
		&port,
		sizeof(port),
		&value,
		sizeof(value),
		&bytesReturned,
		NULL);
}

VOID WINAPI WinRing0::writeIoPortByte(BYTE& port, BYTE& value) {
	OLS_WRITE_IO_PORT_INPUT inBuf = {port, value};
	bResult = DeviceIoControl(
		gHandle,
		IOCTL_OLS_WRITE_IO_PORT_BYTE,
		&inBuf,
		offsetof(OLS_WRITE_IO_PORT_INPUT, CharData) + sizeof(inBuf.CharData),
		NULL,
		0,
		&bytesReturned,
		NULL);
}

WINAPI RwDrv::RwDrv(LPCTSTR DriverId, LPCTSTR DriverFileName) : Driver(DriverId, DriverFileName), bResult(0), bytesReturned(0)
{}

VOID WINAPI RwDrv::readIoPortByte(BYTE& port, BYTE& value) {
	UCHAR Request[0x100];
	ZeroMemory(&Request, sizeof(Request));

	*(PWORD)(Request + 0x00) = port;

	// send request to the driver
	bResult = DeviceIoControl(
		gHandle, 0x222810,
		&Request, sizeof(Request), &Request, sizeof(Request),
		&bytesReturned, NULL
	);

	value = *(PDWORD64)(Request + 0x04);
}

VOID WINAPI RwDrv::writeIoPortByte(BYTE& port, BYTE& value) {
	UCHAR Request[0x100];
	ZeroMemory(&Request, sizeof(Request));

	*(PWORD)(Request + 0x00) = port;
	*(PDWORD64)(Request + 0x04) = value;

	// send request to the driver
	bResult = DeviceIoControl(
		gHandle, 0x222814,
		&Request, sizeof(Request), &Request, sizeof(Request),
		&bytesReturned, NULL
	);
}


