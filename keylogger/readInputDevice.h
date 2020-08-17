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
#include <fstream>

#include <thread>
#include "keyLogger.h"
#include "emulator/kbdEmu.h"

#include <string.h>
#include <linux/input.h>

#include "circleBuf.h"
#include "PosixHelper.h"
#include "log.h"


#define KEYCODE_MAX 256

class ReadInputDevice: public keyLogger
{
	public:
		ReadInputDevice(std::string device, size_t size = 100);
		~ReadInputDevice() 
		{
			fs.close();
			delete queue;
		}
		kbdReport getKeyReport(void);
		std::string getDeviceName(void)
		{
			return device;
		}

	private:
		std::string device;
		std::fstream fs;
		std::thread worker;
		circleBuf<kbdReport> *queue;         //hid report queue
		circleBuf<keyEvent_t> *evtqueue;     //linux key event queue
		
		struct
		{
			kbdReport  normal;
			kbdReport  phantom;
			int    state[KEYCODE_MAX];
			size_t pressedNum; //pressed key counter, does not include modifier
			bool   inPhantom;

			void updatePhantom(void)
			{
				if(pressedNum <= sizeof(normal.report.keypress))
					inPhantom = false;
				else
					inPhantom = true;
			}
			void setKeyPress(uint8_t scancode, bool isModifier)
			{
				if(scancode < KEYCODE_MAX &&
					state[scancode] == VAL_KEY_RELEASE)
				{
					state[scancode] = VAL_KEY_PRESS;
					if(!isModifier)
						pressedNum++;
				}
				updatePhantom();
			}
			void setKeyRelease(uint8_t scancode, bool isModifier)
			{
				if(scancode < KEYCODE_MAX &&
					state[scancode] == VAL_KEY_PRESS)
				{
					state[scancode] = VAL_KEY_RELEASE;
					if(!isModifier)
						pressedNum--;
				}
				updatePhantom();
			}
		}kbdStat;

		void kbdListener(void); 
		int InputSystemKbdGetDevice(std::string& dev);
		std::string getEventString(input_event event);
		bool filterKeyEvent(input_event event, keyEvent_t& keystat);
		void storeKeyEvent(input_event event);
		void parseEventQueue(void);
};
