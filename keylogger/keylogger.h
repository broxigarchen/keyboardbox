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

#include <mutex>
#include <condition_variable>

#include <string.h>
#include <linux/input.h>
#include "PosixHelper.h"
class keylogger
{
	public:
		keylogger() {}
		virtual ~keylogger() {}
		
		virtual void kbdListener(void) = 0;
		virtual bool ifkeyStrokeAvailable(void) = 0;
		virtual int  getKeyStroke(void) = 0;

		enum VAL_EV_KEY
		{
			VAL_KEY_RELEASE = 0x0,
			VAL_KEY_PRESS   = 0x1,
			VAL_KEY_REPEAT  = 0x2,
		};

		typedef struct _keyStat
		{
			int code;  //key code
			int act;   //key act
		}keyStat_t;

		std::string parseEvent(input_event event)
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

		void storeKeystroke(input_event event)
		{
			/* filter */
		}
	private:
		keyStat_t filterKeystroke(input_event event)
		{
			keyStat_t t;
			memset(&t, 0 , sizeof(keyStat_t));
			switch(event.type)
			{
				case EV_KEY:
					t.code = event.code;
					t.act = event.value;
					break;
				case EV_SYN:
					break;
				default:
					break;
			}
			return t;

		}
		std::condition_variable keymutex;
	
};
