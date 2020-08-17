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
#include "log.h"

class kbdHandler: public PosixHelper::sysSignalHandler
{
	static void dummy(int sig)
	{
		//do nothing
	}

	virtual void init(void)
	{
		addsig(SIGINT, &kbdHandler::dummy);
		addsig(SIGQUIT, &kbdHandler::dummy);
		addsig(SIGTSTP, &kbdHandler::dummy);
	}
};

int main(int argc, char** argv)
{
	int opt;
	int rc;

	while((opt = getopt(argc, argv, "hd")) != -1) {
		switch(opt) {
			case 'h':
				fprintf(stdout, 
						"Usage: keyboardbox [-hd]\n"
						"-d     debug mode, use stdout\n"
						"-h		show this help\n"
						);
				break;
			case 'd':
				usesyslog = 0;
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
		PERROR("switchToRoot");
		ERROR("Require root privilege to run!\n");
		return EXIT_FAILURE;
	}

	//setup emulator, logger, keymap
	//ReadInputDevice logger("default");
	ReadInputDevice logger("/dev/input/event0");
	kbdEmu          emulator("/dev/hidg0",logger.getDeviceName().c_str());
	keyMaps         keymap;

	//setup actor
	//actorInit();	

	rc = launch_task(emulator, logger, keymap);

	kbdHandler handler;
	handler.run();
	return EXIT_SUCCESS;
}
