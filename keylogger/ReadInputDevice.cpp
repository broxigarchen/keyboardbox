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
#include "ReadInputDevice.h"
#include "common/PosixHelper.h"
#include <linux/input.h>
#include <thread>
#include <string>

/*
 * Base on Linux input subsystem
 * Search through /proc/bus/input/devices to retrieve device number
 */
int ReadInputDevice::InputSystemKbdGetDevice(std::string& dev)
{
	const char* cmd = "cat /proc/bus/input/devices | grep -A 4 -E Keyboard | grep -Eo \"event[0-9]+\"";
	std::string res = PosixHelper::exec(cmd);
		
	std::regex expected("event[0-9]+", std::regex::extended | std::regex::icase);
	if(std::regex_match(res.c_str(), expected)) {
		dev = res;
		return 0;
	}
	return -1;
}

ReadInputDevice::ReadInputDevice(std::string device, std::condition_variable* cond) : keylogger(cond)
{
	if(device.compare("default") == 0)
	{
		int res = this->InputSystemKbdGetDevice(device);	
		if(res == -1)
		{
			throw std::ios_base::failure("no keyboard device found!");
		}
	}
	std::cout<<"device is"<<device<<std::endl;
	ofs.open(device.c_str(), std::ofstream::in);
	if(!ofs.is_open())
	{
		throw std::ios_base::failure("cannot open device!");
	}
	std::thread worker(&ReadInputDevice::main, this);
}

void ReadInputDevice::main(void)
{
	/*
	std::cout<<"worker start"<<std::endl;
	input_event event;
	while(1)
	{
		ofs.read(&event, sizeof(input_event));
		if(res <= 0)
		{
			std::cout<<"error read"<<std::endl;
		}
	}
	*/
}

bool ReadInputDevice::ifkeyStrokeAvailable(void)
{
	return false;
}

int  ReadInputDevice::getKeyStroke(void)
{
	return -1;
}

