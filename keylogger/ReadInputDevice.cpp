/*
MIT License

Copyright (c) 2020 Brox Chen broxigarchen@gmail.com 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <regex>
#include <iostream>
#include <linux/input.h>
#include <thread>
#include <string>

#include "ReadInputDevice.h"
#include "PosixHelper.h"

/*
 * Base on Linux input subsystem
 * Search through /proc/bus/input/devices to retrieve event number
 */
int ReadInputDevice::InputSystemKbdGetDevice(std::string& dev)
{
	const char* cmd = "cat /proc/bus/input/devices | grep -A 4 -E Keyboard | grep -Eo \"event[0-9]+\"";
	std::string res = PosixHelper::exec(cmd);
		
	std::cmatch match;
	std::regex expected("event[0-9]+", std::regex::extended | std::regex::icase);
	if(std::regex_search(res.c_str(), match, expected))
	{
		dev = "/dev/input/";
		dev += match[0];
		return 0;
	}
	return -1;
}

ReadInputDevice::ReadInputDevice(std::string device)
{
	if(device.compare("default") == 0)
	{
		int res = this->InputSystemKbdGetDevice(device);	
		if(res == -1)
		{
			throw std::ios_base::failure("no keyboard device found!");
		}
	}
	std::cout<<"device is "<<device<<std::endl;
	ifs.open(device.c_str(), std::ifstream::in | std::ifstream::binary);
	if(ifs.fail())
	{
		perror("ReadInputDevice");
		throw std::ios_base::failure("");
	}
	this->worker = std::thread(&ReadInputDevice::kbdListener, this);
}

void ReadInputDevice::kbdListener(void)
{
	std::cout<<"worker start"<<std::endl;
	input_event event;
	std::string output('\0', 100);

	while(1)
	{
		ifs.read(reinterpret_cast<char*>(&event), sizeof(input_event));
		if(ifs)
		{
			std::cout<< this->parseEvent(event) <<std::endl;
		} else {
			std::cout<<"error read"<<std::endl;
		}
	}
	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

bool ReadInputDevice::ifkeyStrokeAvailable(void)
{
	return false;
}

int  ReadInputDevice::getKeyStroke(void)
{
	return -1;
}

