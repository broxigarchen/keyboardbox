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

#include <thread>

#include "kbdEmu.h"
#include "readInputDevice.h"
#include "keyMaps.h"
#include "actor.h"
#include "task.h"

static std::thread main_thread;

void main_task(kbdEmu& kbd, ReadInputDevice& logger, keyMaps& map)
{
	kbdReport report;
	while(1)
	{
		report = logger.getKeyReport();

		kbd.addKbdReport(report);
	}
}

int launch_task(kbdEmu& kbd, ReadInputDevice& logger, keyMaps& map)
{
	main_thread = std::thread(main_task, std::ref(kbd), std::ref(logger), std::ref(map));
	return 0;
}