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

#include "ArgumentParser.hpp"


ArgumentParser::ArgumentParser(int argc, char** argv) : mDriverType(Driver::DriverType::eUnInit), mParsedCommands() {
    if (argc < 2) {
        showError(ErrorType::eShowUsage, "");
        exit(1);
    }
   
    int remainingArgs = argc - 1;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-w" && (remainingArgs != 2 && remainingArgs != 1)) {
            //std::cout << "write: " << argv[i + 1] << " " << argv[i + 2] << " remainingArgs:" << remainingArgs << std::endl; //Debug prints
            std::string addr_arg = argv[i + 1];
            std::string value_arg = argv[i + 2];
            uint8_t write_address = 0, write_value = 0;
            if (stringToInt(addr_arg, write_address) && stringToInt(value_arg, write_value)) {
                mParsedCommands.push_back(std::tuple<char, uint8_t, uint8_t>('w', write_address, write_value));
            } else {
                showError(ErrorType::eShowUsage, "");
                exit(1);
            }
            i += 2;
            remainingArgs -= 2;
        } else if (arg == "-r" && remainingArgs != 1) {
            //std::cout << "read: " << argv[i + 1] << " remainingArgs:" << remainingArgs << std::endl; //Debug prints
            std::string addr_arg = argv[i + 1];
            uint8_t read_address = 0;
            if (stringToInt(addr_arg, read_address)) {
                mParsedCommands.push_back(std::tuple<char, uint8_t, uint8_t>('r', read_address, 0));
            } else {
                showError(ErrorType::eShowUsage, "");
                exit(1);
            }
            i += 1;
            remainingArgs -= 1;
        } else if (arg == "-sc" && remainingArgs != 1) {
            //std::cout << "read: " << argv[i + 1] << " remainingArgs:" << remainingArgs << std::endl; //Debug prints
            std::string addr_arg = argv[i + 1];
            uint8_t sc_address = 0;
            if (stringToInt(addr_arg, sc_address)) {
                ARG_EC_SC = sc_address;
            }
            else {
                showError(ErrorType::eShowUsage, "");
                exit(1);
            }
            i += 1;
            remainingArgs -= 1;
        } else if (arg == "-data" && remainingArgs != 1) {
            //std::cout << "read: " << argv[i + 1] << " remainingArgs:" << remainingArgs << std::endl; //Debug prints
            std::string addr_arg = argv[i + 1];
            uint8_t data_address = 0;
            if (stringToInt(addr_arg, data_address)) {
                ARG_EC_DATA = data_address;
            }
            else {
                showError(ErrorType::eShowUsage, "");
                exit(1);
            }
            i += 1;
            remainingArgs -= 1;
        } else if (arg == "-rwdrv") {
            if (mDriverType != Driver::DriverType::eWinRing0) {
                mDriverType = Driver::DriverType::eRwDrv;
            } else {
                showError(ErrorType::eMultiDriver, "");
                showError(ErrorType::eShowUsage, "");
                exit(1);
            }
        } else if (arg == "-winring0") {
            if (mDriverType != Driver::DriverType::eRwDrv) {
                mDriverType = Driver::DriverType::eWinRing0;
            } else {
                showError(ErrorType::eMultiDriver, "");
                showError(ErrorType::eShowUsage, "");
                exit(1);
            }
        } else {
            showError(ErrorType::eShowUsage, "");
            exit(1);
        }
        remainingArgs -= 1;
    }

    if (mDriverType == Driver::DriverType::eUnInit) {
        showError(ErrorType::eDriverUnInit, "");
        showError(ErrorType::eShowUsage, "");
        exit(1);
    }
}

void ArgumentParser::showError(ErrorType error, std::string arg) const {

    switch (error) {
    case ErrorType::eOutOfRange:
        std::cerr << std::endl
            << "Error: The number '" << arg << "' is out of range" << std::endl
            << "Address and Value should be less than or equal to 0xFF (255)" << std::endl;
        break;
    case ErrorType::eInvalidNumber:
        std::cerr << std::endl
            << "Error: The string '" << arg << "' is not a valid number" << std::endl
            << "Please use valid numbers between 0x00 and 0xFF for addresses and values " << std::endl;
        break;
    case ErrorType::eTrailingChars:
        std::cerr << std::endl
            << "Error: The string '" << arg << "' has trailing characters and therefore is not a valid number" << std::endl
            << "Please use valid numbers between 0x00 and 0xFF for addresses and values " << std::endl;
        break;
    case ErrorType::eShowUsage:
        std::cerr << std::endl
            << "Usage: EC-Access-Tool [options]" << std::endl
            << std::endl
            << "Options (per register operation):" << std::endl
            << "\t-[rwdrv|winring0]     Use RwDrv.sys or WinRing0 as the driver to access EC" << std::endl
            << "\t-w  [address] [value] Write to a register with 'address' and 'value'" << std::endl
            << "\t-r  [address]         Read from a register with 'address'           " << std::endl
            << "\t-data  [port]         Read from a specified data port (0x62)           " << std::endl
            << "\t-sc  [port]           Read from a specified sc port (0x66)          " << std::endl
            << std::endl
            << "Example Usage (N operations at a time, N can be any natural number):" << std::endl
            << "\tEC-Access-Tool -winring0 -r RegAddr-1 -r RegAddr-2 -w RegAddr-3 WriteVal-3 ... -[w/r] RegAddr-N WriteVal-N " << std::endl
            << "\tOutput: ReadVal-1 ReadVal-2 ... (Only the read outputs from N operations are displayed)" << std::endl
            << std::endl;
        break;
    case ErrorType::eDriverUnInit:
        std::cerr << std::endl
            << "Error: Driver type was not selected" << std::endl
            << "Please use '-rwdrv' or '-winring0' option to select the corresponding driver" << std::endl;
        break;
    case ErrorType::eMultiDriver:
        std::cerr << std::endl
            << "Error: More than one driver selected" << std::endl
            << "Please use either '-rwdrv' or '-winring0' option at a time" << std::endl;
        break;
    }
}

BOOL ArgumentParser:: stringToInt(std::string arg, uint8_t& number) {
    try {
        std::size_t pos;
        unsigned long converted_number = std::stoul(arg, &pos, 16);
        if (pos < arg.size()) {
            showError(ErrorType::eTrailingChars, arg);
            return FALSE;
        }
        else if (converted_number > 255) {
            throw std::out_of_range("outOfRange");
        }
        else {
            number = static_cast<uint8_t>(converted_number);
        }
    }
    catch (std::invalid_argument const& ex) {
        (void)ex;
        showError(ErrorType::eInvalidNumber, arg);
        return FALSE;
    }
    catch (std::out_of_range const& ex) {
        (void)ex;
        showError(ErrorType::eOutOfRange, arg);
        return FALSE;
    }
    return TRUE;
}

Driver::DriverType ArgumentParser::getDriverType() const {
    return mDriverType;
}

const std::vector<std::tuple<char, uint8_t, uint8_t>>& ArgumentParser::getParsedCommandsRef() const {
    return mParsedCommands;
}

BYTE ArgumentParser::getDataPort() {
    return ARG_EC_DATA;
}

BYTE ArgumentParser::getScPort() {
    return ARG_EC_SC;
}