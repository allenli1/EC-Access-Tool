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
#include <windows.h>
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <stdexcept>
#include "driver.hpp"

class ArgumentParser {
 public:
	 enum class ErrorType {eOutOfRange, eInvalidNumber, eTrailingChars, eShowUsage, eDriverUnInit, eMultiDriver};

	 ArgumentParser(int argc, char** argv);
	 BOOL stringToInt(std::string arg, uint8_t& number);
	 void showError(ErrorType error, std::string arg) const;
     const std::vector<std::tuple<char, uint8_t, uint8_t>>& getParsedCommandsRef() const;
	 Driver::DriverType getDriverType() const;

 private:
	 Driver::DriverType mDriverType;
	 std::vector<std::tuple<char, uint8_t, uint8_t>> mParsedCommands;
};