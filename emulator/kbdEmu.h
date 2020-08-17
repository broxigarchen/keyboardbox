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

#include <thread>
#include <sstream>
#include <map>
#include "hidEmu.h"
#include "circleBuf.h"
#include "string.h"

#define  KBD_LCTRL   (0x1 << 0)
#define  KBD_LSHFT   (0x1 << 1)
#define  KBD_LALT    (0x1 << 2)
#define  KBD_LGUI    (0x1 << 3)
#define  KBD_RCTRL   (0x1 << 4)
#define  KBD_RSHFT   (0x1 << 5)
#define  KBD_RALT    (0x1 << 6)
#define  KBD_RGUI    (0x1 << 7)

/*
 * Keyboard USB spec report format
 *
 * Reference:
 * https://www.win.tue.nl/~aeb/linux/kbd/scancodes-14.html
 * https://wiki.osdev.org/USB_Human_Interface_Devices
 *
 * Notice: "code" is the key code defined in USB spec
 */
class kbdReport
{
	public:
	kbdReport():keycnt(0) {
		clear();
	}

	static const std::map<uint8_t, uint8_t> kmodmap;
	static const char* codeToString[256];

	bool isModifier(uint8_t code)
	{
		if(kbdReport::kmodmap.find(code) == kbdReport::kmodmap.end())
		{
			return false;
		}
		return true;
	}

	void clrModifier(uint8_t modifiers)    { report.status.byte = 0; }

	void addModifier(uint8_t code) { 
		if(!isModifier(code)) return;
		report.status.byte |= kbdReport::kmodmap.at(code); 
	}

	void removeModifier(uint8_t code) {
		if(!isModifier(code)) return;
		report.status.byte &= ~ kbdReport::kmodmap.at(code);
	}

	void clear(void)
	{
		memset(&report, 0, sizeof(report));
		keycnt = 0;
	}

	std::string getString(void)
	{
		std::stringstream str;
		str << std::to_string(report.status.byte) << " ";
		str << std::to_string(report.reserve) << " ";
		for(size_t i = 0; i < sizeof(report.keypress); i++)
		{
			str << std::to_string(report.keypress[i]) << " ";
		}
		str << keycnt;
		str << std::endl;
		return str.str();
	}

	std::string getNameFromCode(int code)
	{
		if(code < sizeof(codeToString))
		{
			std::string name(codeToString[code]);
			return name;
		}
		std::string empty("");
		return empty;
	}

	int getCodeFromName(const char* name)
	{
		for(int i = 0; i< sizeof(codeToString); i++)
		{
			if(codeToString[i].compare(name) == 0)
			{
				return i;
			}
		}
		return -1;
	}

	int searchKey(uint8_t code)
	{
		for(int i = 0; i < keycnt; i++)
		{
			if(code == report.keypress[i])
			{
				return i;
			}
		}
		//0 is err
		return 0;
	}

	void addKey(uint8_t code)
	{
		int index = searchKey(code);
		if(index == -1)
		{
			if(keycnt == sizeof(report.keypress))
			{
				removeKey(report.keypress[0]);
			}
			report.keypress[keycnt++] = code;
		}
	}

	void removeKey(uint8_t code)
	{
		int index = searchKey(code);
		if(index == -1)
		{
			return;
		}
		for(int i = index; i < keycnt; i++)
		{
			if(i < keycnt - 1)
			{
				report.keypress[i] = report.keypress[i+1];
			} else {
				report.keypress[i] = 0;
			}
		}
		keycnt--;
	}

	void setPhantom(void)
	{
		clear();
		report.status.byte = 0x0;
		for(size_t i = 0; i < sizeof(report.keypress); i++)
		{
			report.keypress[i] = 0x1;
		}
		keycnt = 6;
	}

	struct _report
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
	}report;
	int keycnt;
};
/*
#define  LED_NUML    (0x1 << 0)
#define  LED_CAPL    (0x1 << 1)
#define  LED_SCROLLL (0x1 << 2)
#define  LED_COMPOSE (0x1 << 3)
#define  LED_KANA    (0x1 << 4)
*/
#if 1
typedef union _ledReport
{
	uint8_t byte;
	struct
	{
		uint8_t numLk:1;
		uint8_t capLk:1;
		uint8_t scrollLk:1;
		uint8_t compose:1;
		uint8_t kana:1;
		uint8_t reserve:3;
	}bit;
}ledReport;
#else
class ledReport
{
	public:
	void setLedTemplate(void)
	{
		report.field.type = 0x21;
		report.field.request = 0x9;
		report.field.wValueL = 0;
		report.field.wValueH = 0x2;

	}
	union
	{
		uint8_t byte[8];
		struct
		{
			uint8_t type;
			uint8_t request;
			uint8_t wValueL;
			uint8_t wValueH;
			uint8_t wIndexL;
			uint8_t wIndexH;
			uint8_t wLengthL;
			uint8_t wLengthH;
		}field;
	}report;
};
#endif
class kbdEmu: public hidEmu
{
	public:
		kbdEmu(const char* gadget, const char* kbd, size_t size = 100);
		~kbdEmu();

		int addKbdReport(kbdReport report);

	private:
		std::thread worker[2];
		circleBuf<kbdReport> *queue;

		ledReport ledreport;
		std::fstream gadget;  //slave
		std::fstream kbd;     //master

		void sender(void);
		void receiver(void);

		int sendToKbd(const char* data, size_t n);
};

#endif

