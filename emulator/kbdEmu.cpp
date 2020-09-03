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

#include "kbdEmu.h"
#include "PosixHelper.h"
#include <iostream>

const std::map<uint8_t, uint8_t> kbdReport::kmodmap = {
  {224,  KBD_LCTRL},
  {225,  KBD_LSHFT},
  {226,  KBD_LALT},
  {227,  KBD_LGUI},
  {228,  KBD_RCTRL},
  {229,  KBD_RSHFT},
  {230,  KBD_RALT},
  {231,  KBD_RGUI},
};

const char* kbdReport::codeToString[256] =
{
"err","err","err",
"A","B","C","D","E","F","G","H","I","J","K","L","M",
"N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
"1","2","3","4","5","6","7","8","9","0",
"Enter","Esc","BSp","Tab","Space",
"-/_","=/+","[/{","]/}","\\/|","...",";/:","\'/\"","'/<","./>","//?","Caps Lock",
"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
"PrtScr","Scroll Lock","Pause","Insert","Home","PgUp","Delete","End","PgDn",
"Right","Left","Down","Up",
"Num Lock","KP /","KP *","KP -","KP +","KP Enter","KP 1/End","KP 1/Down",
"KP 2/Down","KP 3/PgDn","KP 4/Left","KP 5","KP 6/Right","KP 7/Home","KP 8/Up",
"KP 9/PgUp","KP 0/Ins","KP ./Del",
"...","Applic","Power","KP=",
"F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24",
"Execute","Help","Menu","Select","Stop","Again","Undo","Cut","Copy","Paste",
"Find","Mute","Volume Up","Volume Down","Locking Caps Lock","Locking Num Lock",
"Locking Scroll Lock","KP ,","KP =",
"Internet","Internet","Internet","Internet","Internet","Internet","Internet","Internet","Internet",
"LANG","LANG","LANG","LANG","LANG","LANG","LANG","LANG","LANG",
"Alt Erase","SysRq","Cancel","Clear","Prior","Return","Separ","Out","Oper","Clear/Again",
"CrSel/Props","ExSel","","","",
"LCtrl","LShift","LAlt","LGUI","RCtrl","RShift","RAlt","RGUI",
};

kbdEmu::kbdEmu(const char* _gadget, const char* _kbd, size_t size)
{
	gadget.open(_gadget, std::fstream::out | std::fstream::in | std::fstream::binary);
	if(gadget.fail())
	{
		PERROR("hidEmu gadget.open");
		exit(EXIT_FAILURE);
	}
	kbd.open(_kbd, std::fstream::out | std::fstream::binary);
	if(kbd.fail())
	{
		PERROR("hidEmu kbd.open");
		exit(EXIT_FAILURE);
	}

	queue = new circleBuf<kbdReport>(size);

	this->worker[0] = std::thread(&kbdEmu::sender, this);
	//this->worker[1] = std::thread(&kbdEmu::receiver, this);
}

kbdEmu::~kbdEmu()
{
	delete queue;
	gadget.close();
	kbd.close();
}

int kbdEmu::addKbdReport(kbdReport report)
{
	return queue->push_back(report);
}

void kbdEmu::sender(void)
{
	kbdReport r;
	while(1)
	{
		r = queue->pop_front();

		DEBUG("%s send report: %s",
				PosixHelper::getTimeStamp().c_str(),
				r.getString().c_str());

		if(gadget.fail())
		{
			PERROR("KbdEmu write");
			exit(EXIT_FAILURE);
		}

		gadget.write((const char*)&r.report, sizeof(r.report));
		gadget.flush();
	}
}

void kbdEmu::receiver(void)
{
	ledReport r;
	while(1)
	{
		gadget.read(reinterpret_cast<char*>(&r.byte), sizeof(r));
		if(gadget)
		{
			DEBUG("Received led report: %d", r.byte);
			//for(int i = 0; i < 8; i++)
			//	DEBUG("%d,", r[i]);

			if(-1 == sendToKbd((const char*)&r.byte, sizeof(r.byte)))
			{
				PERROR("kbdEmu:sendToKbd");
				exit(EXIT_FAILURE);
			}
		} else {
			PERROR("kbdEmu::read");
			exit(EXIT_FAILURE);
		}

	}
}

int kbdEmu::sendToKbd(const char* data, size_t n)
{
	if(kbd.fail())
	{
		return -1;
	}
	kbd.write(data, n);
	kbd.flush();
	return 0;
}
