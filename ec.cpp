/*
 *  EC-Access-Tool Copyright(C) 2021, File Modified by: Shubham Paul under 'GPLv3 only'
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
 //     Author : Soberia
 //        Web : https://github.com/Soberia
 //    License : The modified BSD license
 // 
 //    Copyright(C) 2021
 //-----------------------------------------------------------------------------

#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <windows.h>

#include "ec.hpp"

EmbeddedController::EmbeddedController(
    BYTE scPort,
    BYTE dataPort,
    BYTE endianness,
    UINT16 retry,
    UINT16 timeout,
    Driver::DriverType driverType
)
    : scPort(scPort)
    , dataPort(dataPort)
    , endianness(endianness)
    , retry(retry)
    , timeout(timeout)
{
    if (driverType == Driver::DriverType::eWinRing0) {
        driver = std::make_unique<WinRing0>(L"WinRing0_1_2_0", L"WinRing0x64.sys");
    } else {
        driver = std::make_unique<RwDrv>(L"rwdrv", L"rwdrv.sys");
    }
    
    if (driver->initialize()) {
        driverLoaded = TRUE;
    }

    driverFileExist = driver->driverFileExist;
}

VOID EmbeddedController::close() {
    driver->deinitialize();
    driverLoaded = FALSE;
}

BOOL EmbeddedController::readByte(BYTE& bRegister, BYTE& value) {
    return operation(READ, bRegister, value);
}

BOOL EmbeddedController::writeByte(BYTE& bRegister, BYTE& value) {
    return operation(WRITE, bRegister, value);
}

BOOL EmbeddedController::operation(BYTE mode, BYTE& bRegister, BYTE& value) {
    BOOL isRead = mode == READ;
    BYTE operationType = isRead ? RD_EC : WR_EC;

    for (UINT16 i = 0; i < retry; i++) {
        if (status(EC_IBF)) { // Wait until IBF is free
            driver->writeIoPortByte(scPort, operationType); // Write operation type to the Status/Command port
            if (status(EC_IBF)) {                                  // Wait until IBF is free
                driver->writeIoPortByte(dataPort, bRegister); // Write register address to the Data port
                if (status(EC_IBF)) {                               // Wait until IBF is free
                    if (isRead) {
                        if (status(EC_OBF)) { // Wait until OBF is full
                            driver->readIoPortByte(dataPort, value); // Read from the Data port
                            return TRUE;
                        }
                    } else {
                        driver->writeIoPortByte(dataPort, value); // Write to the Data port
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

BOOL EmbeddedController::status(BYTE flag) {
    BOOL done = flag == EC_OBF ? 0x01 : 0x00;
    for (UINT16 i = 0; i < timeout; i++) {
        BYTE result;
        driver->readIoPortByte(scPort, result);
        // First and second bit of returned value represent
        // the status of OBF and IBF flags respectively
        if (((done ? ~result : result) & flag) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}
