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

#include <cstdint>
#include "emulator/kbdEmu.h"
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

class keyLogger
{
	public:
		keyLogger() {
		}

		virtual ~keyLogger() {
		}
		uint8_t convertScanToKeyLinux(uint8_t code);
		uint8_t convertKeyToScanLinux(uint8_t code);
		static const uint8_t scanToKeyLinux[256];
		static const uint8_t keyToScanLinux[256];

		uint8_t convertScanToKeyWin(uint8_t code);
		uint8_t convertKeyToScanWin(uint8_t code);
		static const uint8_t scanToKeyWin[256];
		static const uint8_t keyToScanWin[256];

		uint8_t convertScanToKeyMac(uint8_t code);
		uint8_t convertKeyToScanMac(uint8_t code);
		static const uint8_t scanToKeyMac[256];
		static const uint8_t keyToScanMac[256];

		virtual kbdReport getKeyReport(void) = 0;
	private:
		virtual void kbdListener(void) = 0;
};
