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

#include "readInputDevice.h"
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

ReadInputDevice::ReadInputDevice(std::string device, size_t size)
{
	//setup queue
	queue = new circleBuf<kbdReport>(size);	
	evtqueue = new circleBuf<keyEvent_t>(size);	

	//setup event parsing
	memset((void*)&kbdStat, 0, sizeof(kbdStat));

	//setup phantom report
    kbdStat.phantom.setPhantom();

	//setup device monitoring
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
		exit(EXIT_FAILURE);
	}
	this->worker = std::thread(&ReadInputDevice::kbdListener, this);
}

void ReadInputDevice::kbdListener(void)
{
	input_event event;

	while(1)
	{
		ifs.read(reinterpret_cast<char*>(&event), sizeof(input_event));
		if(ifs)
		{
			std::cout<< getEventString(event) <<std::endl;
			storeKeyEvent(event);
		} else {
			perror("ReadInputDevice::read");
			exit(EXIT_FAILURE);
		}

		parseEventQueue();
	}
}

void ReadInputDevice::storeKeyEvent(input_event event)
{
	keyEvent_t keystat;
	/* filter */
	if(filterKeyEvent(event, keystat))
	{
		evtqueue->push_back(keystat);
	}
}

kbdReport ReadInputDevice::getKeyReport(void)
{
	return queue->pop_front();
}

std::string ReadInputDevice::getEventString(input_event event)
{
	std::string output(200, '\0');
	std::snprintf(const_cast<char*>(output.c_str()), output.size(),
				"\n%s, type=%3d, code=0x%03x, val=0x%x", 
				PosixHelper::parseTimeval(event.time).c_str(),
				event.type,
				event.code,
				event.value);
	return output;
}

bool ReadInputDevice::filterKeyEvent(input_event event, keyEvent_t& keystat)
{
	bool ret = false;
	memset(&keystat, 0 , sizeof(keyEvent_t));
	switch(event.type)
	{
		case EV_KEY:
			keystat.code = event.code;
			keystat.act = event.value;
			ret = true;
			break;
		case EV_SYN:
			break;
		default:
			break;
	}
	return ret;
}

void ReadInputDevice::parseEventQueue(void)
{
	keyEvent_t event;
	kbdReport* report = &kbdStat.normal;
	uint8_t scancode;
	uint8_t keycode;
	int rc;

	while(!evtqueue->ifempty())
	{
		event = evtqueue->pop_front();

		scancode = event.code;
		keycode = keyLogger::convertScanToKeyLinux(scancode);

		std::cout << "Received Event - code:" << event.code
			      << " act: " << event.act << std::endl;

		if(report->isModifier(keycode))  //Modifier keys
		{
			switch(event.act)
			{
				case VAL_KEY_RELEASE:
					kbdStat.setKeyRelease(scancode);
					report->removeModifier(keycode);
				break;
				case VAL_KEY_PRESS:
					kbdStat.setKeyPress(scancode);
					report->addModifier(keycode);
				break;
				default:
				break;
			}
		} else {    //other keys
			rc = report->searchKey(keycode);
			if(rc < -1)
			{
				if(!kbdStat.inPhantom)
					/* something weird happend, a non-logged
					   key being released in non-phantom mode */
					continue;
			}
			switch(event.act)
			{
				case VAL_KEY_RELEASE:
					kbdStat.setKeyRelease(scancode);
					report->removeKey(keycode);
				break;
				case VAL_KEY_PRESS:
					kbdStat.setKeyPress(scancode);
					report->addKey(keycode);
				break;
				default:
				break;
			}
		}

		//maintainance done, send packet
		if(kbdStat.inPhantom)
		{
			//copy modifier
			kbdStat.phantom.report.status = kbdStat.normal.report.status;
			queue->push_back(kbdStat.phantom);
		} else {
			queue->push_back(kbdStat.normal);
		}
	}
}

