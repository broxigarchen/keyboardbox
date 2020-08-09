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

#include <iostream>
#include <unistd.h>

#include "actor.h"
#include "PosixHelper.h"
#include "actors/actorInit.h"
#include "keylogger/readInputDevice.h"
#include "emulator/kbdEmu.h"
#include "keymaps/keyMaps.h"
#include "task.h"

int main(int argc, char** argv)
{
	int opt;
	int rc;
	bool set_daemon = false;

	while((opt = getopt(argc, argv, "dh")) != -1) {
		switch(opt) {
			case 'd':
				set_daemon = 1;
				break;
			case 'h':
				fprintf(stdout, 
						"Usage: keyboardbox [-d]\n"
						"-d		run as daemon\n"
						"-h		show this help\n"
						);
				break;
			default:
				fprintf(stderr, "unkown argument %c", opt);
				exit(EXIT_FAILURE);
				break;
		}
	}

	rc = PosixHelper::switchToRoot();
	if(rc)
	{
		perror("switchToRoot");
		fprintf(stderr, "Require root privilege to run!\n");
		return EXIT_FAILURE;
	}

	//setup emulator, logger, keymap
	kbdEmu          emulator("/dev/hidg0");
	//ReadInputDevice logger("default");
	ReadInputDevice logger("/dev/input/event0");
	keyMaps         keymap;

	//setup actor
	//actorInit();	

	rc = launch_task(emulator, logger, keymap);

	if(set_daemon)
	{
		PosixHelper::runAsDaemon();
	}

	PosixHelper::SysSignalHandler monitor(NULL);
	return EXIT_SUCCESS;
}
