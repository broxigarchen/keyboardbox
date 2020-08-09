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

enum VAL_EV_KEY
{
	VAL_KEY_RELEASE = 0x0,
	VAL_KEY_PRESS   = 0x1,
	VAL_KEY_REPEAT  = 0x2,
};

typedef struct _keyEvent
{
	int code;  //key code
	int act;   //key act
}keyEvent_t;

#define KEYCODE_MAX 256

class ReadInputDevice: public keyLogger
{
	public:
		ReadInputDevice(std::string device, size_t size = 100);
		~ReadInputDevice() 
		{
			ifs.close();
			delete queue;
		}
		kbdReport getKeyReport(void);
		uint8_t convertScanToKeyLinux(uint8_t code);

	private:
		std::ifstream ifs;
		std::thread worker;
		circleBuf<kbdReport> *queue;         //hid report queue
		std::condition_variable hasdata;

		circleBuf<keyEvent_t> *evtqueue;     //linux key event queue
		
		struct
		{
			kbdReport  normal;
			kbdReport  phantom;
			int    state[KEYCODE_MAX];
			size_t pressedNum; //pressed key counter, does not include modifier
			bool   inPhantom;

			void setKeyPress(int index)
			{
				if(index < KEYCODE_MAX &&
					state[index] == VAL_KEY_RELEASE)
				{
					state[index] = VAL_KEY_PRESS;
					if(!normal.isModifier(index))
						pressedNum++;
				}
				if(pressedNum <= sizeof(normal.report.keypress))
				{
					inPhantom = false;
				}
			}
			void setKeyRelease(int index)
			{
				if(index < KEYCODE_MAX &&
					state[index] == VAL_KEY_PRESS)
				{
					state[index] = VAL_KEY_RELEASE;
					if(!normal.isModifier(index))
						pressedNum--;
				}
				if(pressedNum > sizeof(normal.report.keypress))
				{
					inPhantom = true;
				}
			}
		}kbdStat;

		void kbdListener(void); 
		int InputSystemKbdGetDevice(std::string& dev);
		std::string getEventString(input_event event);
		bool filterKeyEvent(input_event event, keyEvent_t& keystat);
		void storeKeyEvent(input_event event);
		void parseEventQueue(void);
};
