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
 //     Author : Soberia
 //        Web : https://github.com/Soberia
 //    License : The modified BSD license
 // 
 //    Copyright(C) 2021
 //-----------------------------------------------------------------------------
#pragma once
#ifndef EC_H
#define EC_H

#include <map>
#include "driver.hpp"

auto constexpr VERSION = "0.1";

constexpr BYTE LITTLE_ENDIAN = 0;
constexpr BYTE BIG_ENDIAN = 1;

constexpr BYTE READ = 0;
constexpr BYTE WRITE = 1;

constexpr BYTE EC_OBF = 0x01;  // Output Buffer Full
constexpr BYTE EC_IBF = 0x02;  // Input Buffer Full
constexpr BYTE EC_DATA = 0x62; // Data Port
constexpr BYTE EC_SC = 0x66;   // Status/Command Port
constexpr BYTE RD_EC = 0x80;   // Read Embedded Controller
constexpr BYTE WR_EC = 0x81;   // Write Embedded Controller

typedef std::map<BYTE, BYTE> EC_DUMP;


/**
 * Implementation of ACPI embedded controller specification to access the EC's RAM
 * @see https://uefi.org/specs/ACPI/6.4/12_ACPI_Embedded_Controller_Interface_Specification/ACPI_Embedded_Controller_Interface_Specification.html
*/
class EmbeddedController {
 public:
     BYTE scPort;
     BYTE dataPort;
     BYTE endianness;
     BOOL driverLoaded = FALSE;
     BOOL driverFileExist = FALSE;

     /**
      * @param scPort Embedded Controller Status/Command port.
      * @param dataPort Embedded Controller Data port.
      * @param endianness Byte order of read and write operations, could be `LITTLE_ENDIAN` or `BIG_ENDIAN`.
      * @param retry Number of retires for failed read or write operations.
      * @param timeout Waiting threshold for reading EC's OBF and IBF flags.
     */
     EmbeddedController(
         BYTE scPort = EC_SC,
         BYTE dataPort = EC_DATA,
         BYTE endianness = LITTLE_ENDIAN,
         UINT16 retry = 5,
         UINT16 timeout = 100,
         Driver::DriverType driverType = Driver::DriverType::eWinRing0
         );

     /** Close the driver resources */
     VOID close();

     /**
      * Read EC register as BYTE.
      * @param bRegister Address of register.
      * @param value read Value of register.
      */
     BOOL readByte(BYTE& bRegister, BYTE& value);

     /**
      * Write EC register as BYTE.
      * @param bRegister Address of register.
      * @param value write Value of register.
      * @return Successfulness of operation.
      */
     BOOL writeByte(BYTE& bRegister, BYTE& value);

 protected:
     UINT16 retry;
     UINT16 timeout;
     std::unique_ptr<Driver> driver;

     /**
      * Perform a read or write operation.
      * @param mode Type of operation.
      * @param bRegister Address of register.
      * @param value Value of register.
      * @return Successfulness of operation.
      */
     BOOL operation(BYTE mode, BYTE& bRegister, BYTE& value);

     /**
      * Check EC status for permission to read or write.
      * @param flag Type of flag.
      * @return Whether allowed to perform read or write.
      */
     BOOL status(BYTE flag);
};

#endif
