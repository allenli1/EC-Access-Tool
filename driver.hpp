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

 //-----------------------------------------------------------------------------
 //     Modified by : Soberia
 //        Web		: https://github.com/Soberia
 //    License	    : The modified BSD license
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

#ifndef DRIVER_H
#define DRIVER_H

// IOCTL Function Code
#define OLS_TYPE 40000 // The Device type code
#define IOCTL_OLS_GET_REFCOUNT CTL_CODE(OLS_TYPE, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OLS_READ_IO_PORT_BYTE CTL_CODE(OLS_TYPE, 0x833, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_OLS_WRITE_IO_PORT_BYTE CTL_CODE(OLS_TYPE, 0x836, METHOD_BUFFERED, FILE_WRITE_ACCESS)

// DLL Status Code
constexpr BYTE OLS_DLL_NO_ERROR = 0;
constexpr BYTE OLS_DLL_DRIVER_NOT_LOADED = 1;
constexpr BYTE OLS_DLL_DRIVER_NOT_FOUND = 2;
constexpr BYTE OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK = 3;
constexpr BYTE OLS_DLL_UNKNOWN_ERROR = 4;

// Driver Type
constexpr BYTE OLS_DRIVER_TYPE_UNKNOWN = 0;
constexpr BYTE OLS_DRIVER_TYPE_WIN_NT = 1;
constexpr BYTE OLS_DRIVER_TYPE_WIN_NT_X64 = 2;

// Driver Install Status Code
constexpr BYTE OLS_DRIVER_INSTALL = 1;
constexpr BYTE OLS_DRIVER_REMOVE = 2;
constexpr BYTE OLS_DRIVER_SYSTEM_INSTALL = 3;
constexpr BYTE OLS_DRIVER_SYSTEM_UNINSTALL = 4;

#pragma pack(push, 4)

typedef struct _OLS_WRITE_IO_PORT_INPUT {
	ULONG PortNumber;
	UCHAR CharData;
} OLS_WRITE_IO_PORT_INPUT;

#pragma pack(pop)

class DriverManager {
 public:
	 DriverManager(LPCTSTR DriverId);
	 BOOL manage(LPCTSTR DriverPath, USHORT Function);

 protected:
	 HANDLE gHandle;
	 LPCTSTR DriverId;

	 BOOL installDriver(SC_HANDLE hSCManager, LPCTSTR DriverPath);
	 BOOL removeDriver(SC_HANDLE hSCManager);
	 BOOL startDriver(SC_HANDLE hSCManager);
	 BOOL stopDriver(SC_HANDLE hSCManager);
	 BOOL isSystemInstallDriver(SC_HANDLE hSCManager, LPCTSTR DriverPath);
	 BOOL openDriver();
};

class Driver : public DriverManager {
 public:
	 enum class DriverType { eWinRing0, eRwDrv, eUnInit };
	 BOOL driverFileExist;
	 BOOL WINAPI initialize();
	 VOID WINAPI deinitialize();
	 VOID virtual WINAPI readIoPortByte(BYTE& port, BYTE& value) = 0;
	 VOID virtual WINAPI writeIoPortByte(BYTE& port, BYTE& value) = 0;

	 Driver(LPCTSTR DriverId, LPCTSTR DriverFileName);

 protected:
	 BYTE driverFileExistence();

 private:
	 TCHAR gDriverFileName[MAX_PATH];
	 TCHAR gDriverPath[MAX_PATH];
	 BOOL gInitDll = FALSE;
	 BYTE gDllStatus = OLS_DLL_UNKNOWN_ERROR;
	 BYTE gDriverType = OLS_DRIVER_TYPE_UNKNOWN;
};

class WinRing0 : public Driver {
 public:
	 BOOL bResult;
	 DWORD bytesReturned;

	 WinRing0(LPCTSTR DriverId, LPCTSTR DriverFileName);

	 VOID virtual WINAPI readIoPortByte(BYTE& port, BYTE& value);
	 VOID virtual WINAPI writeIoPortByte(BYTE& port, BYTE& value);

};

class RwDrv : public Driver {
 public:
	 BOOL bResult;
	 DWORD bytesReturned;

	 RwDrv(LPCTSTR DriverId, LPCTSTR DriverFileName);

	 VOID virtual WINAPI readIoPortByte(BYTE& port, BYTE& value);
	 VOID virtual WINAPI writeIoPortByte(BYTE& port, BYTE& value);

};
#endif
