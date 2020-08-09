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

kbdEmu::kbdEmu(const char* dev, size_t size): hidEmu(dev)
{
	queue = new circleBuf<kbdReport>(size); 
	this->worker = std::thread(&kbdEmu::sender, this);
}

kbdEmu::~kbdEmu()
{
	delete queue;
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

		std::cout<< PosixHelper::getTimeStamp()
				 << " send report: "
				 << r.getString()
				 << std::endl;

		send((const char*)&(r.report), sizeof(r.report));
	}
}

