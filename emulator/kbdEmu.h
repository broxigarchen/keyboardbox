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

#ifndef KBDEMU_H_
#define KBDEMU_H_

#include "hidEmu.h"

#define  KBD_LCTRL   (0x1 << 0)
#define  KBD_LSHFT   (0x1 << 1)
#define  KBD_LALT    (0x1 << 2)
#define  KBD_LGUI    (0x1 << 3)
#define  KBD_RCTRL   (0x1 << 4)
#define  KBD_RSHFT   (0x1 << 5)
#define  KBD_RALT    (0x1 << 6)
#define  KBD_RGUI    (0x1 << 7)
typedef struct _kbdReport
{
	union
	{
		uint8_t byte;
		struct
		{
			uint8_t lCtrl:1;
			uint8_t lShft:1;
			uint8_t lAlt:1;
			uint8_t lGUI:1;
			uint8_t rCtrl:1;
			uint8_t rShft:1;
			uint8_t rAlt:1;
			uint8_t rGUI:1;
		}bit;
	}status;
	uint8_t reserve;
    uint8_t keypress[6];	
}kbdReport_t;

#define  LED_NUML    (0x1 << 0)
#define  KBD_CAPL    (0x1 << 1)
#define  KBD_SCROLLL (0x1 << 2)
#define  KBD_COMPOSE (0x1 << 3)
#define  KBD_KANA    (0x1 << 4)
typedef struct _ledReport
{
	uint8_t numLk:1;
	uint8_t capLk:1;
	uint8_t scrollLk:1;
	uint8_t compose:1;
	uint8_t kana:1;
	uint8_t reserve:3;
}ledReport_t;

class kbdEmu: public hidEmu
{
	public:
		int sendReport(uint8_t modifier, uint8_t *keycombo, size_t number);
	private:
};

#endif

