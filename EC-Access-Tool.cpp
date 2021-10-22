/*
 *  EC-Access-Tool Copyright(C) 2021, Shubham Paul under GPLv3
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

#pragma once
#include <iomanip>
#include "ArgumentParser.hpp"
#include "ec.hpp"

int main(int argc, char** argv) {

    ArgumentParser arg_parser = ArgumentParser(argc, argv);
    const auto& parsed_commands = arg_parser.getParsedCommandsRef();
    auto driver_type = arg_parser.getDriverType();

    EmbeddedController ec = EmbeddedController(EC_SC, EC_DATA, LITTLE_ENDIAN, 5, 100, driver_type);
    if (!ec.driverLoaded) {
        std::cerr << std::endl
            << "Error: Couldn't load Driver" << std::endl
            << std::endl;
    }
    else if (!ec.driverFileExist) {
        std::cerr << std::endl
            << "Error: Driver File Not Found" << std::endl
            << "Please make sure that the file 'RwDrv.sys' or 'WinRing0x64.sys' is in the same directorty as EC-Access-Tool.exe" << std::endl
            << std::endl;
    }
    else {
        BYTE read_value = 0x00;
        BYTE last_read_value = 0x00;

        for (const auto& command : parsed_commands) {
            char operation = std::get<0>(command);
            if (operation == 'w') {
                //std::cout << "write: " << (int)std::get<1>(command) << " " << (int)std::get<2>(command) << std::endl; //Debug prints
                uint8_t write_address = std::get<1>(command);
                uint8_t write_value = std::get<2>(command);
                ec.writeByte(write_address, write_value);
            }
            else if (operation == 'r') {
                //std::cout << "read: " << (int)std::get<1>(command) << std::endl; //Debug prints
                uint8_t read_address = std::get<1>(command);
                if (ec.readByte(read_address, read_value) == TRUE) {// Read value of register 
                    std::cout << " 0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (INT)read_value;
                    last_read_value = read_value;
                }
                else {
                    std::cout << " 0xXX";
                }
            }
        }
        std::cout << std::endl;
        ec.close(); // Close the resources
    }

    return 0;
}

