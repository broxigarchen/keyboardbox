
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

#ifndef POSIXHELPER_H_
#define POSIXHELPER_H_

#include <string>
#include <signal.h>


namespace PosixHelper
{

	extern  std::string exec(const char* cmd);

	extern bool SysCmdIsAvailable(void);

	class SysSignalHandler
	{
		public:
			SysSignalHandler(void (*cleanup)(void)=NULL) 
			{
				sigset_t set;
				int      sig;
				sigemptyset(&set);
				sigaddset(&set, SIGABRT);
				sigaddset(&set, SIGTERM);
				while(1)
				{
					int rc = sigwait(&set, &sig);	
					if(rc == 0)
					{
						if(cleanup) cleanup();
						break;
					}
				}
			}
	};

}
#endif
