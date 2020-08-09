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

#ifndef HIDEMU_H_
#define HIDEMU_H_

#include <fstream>
#include <errno.h>

#include "nonCopyable.h"
class hidEmu: public nonCopyable
{
	public:
		hidEmu(const char* dev)
		{
			fs.open(dev, std::fstream::out | std::fstream::in | std::fstream::binary);
			if(fs.fail())
			{
				perror("hidEmu");
				exit(EXIT_FAILURE);
			}
		}

		virtual ~hidEmu()
		{
			fs.close();
		}

	protected:

		void send(const char* buf, size_t n)
		{
			if(fs.fail())
			{
				perror("KbdEmu write");
				exit(EXIT_FAILURE);
			}

			fs.write(buf, n);
			fs.flush();
		}

		std::fstream fs;
};



#endif
