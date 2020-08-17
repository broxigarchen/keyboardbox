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
#define LOG_CPP
#include "log.h"

bool usesyslog = 0;
PosixHelper::sysLog logger;


void logToConsole(int loglevel, const char* str, ...)
{
	va_list va, vb;
	va_start(va, str);
	va_copy(vb, va);

	int rc = std::vsnprintf(NULL, 0, str, va); 
	if(rc <= 0)
	{
		return;
	}
	std::string out(rc + 1, '\0');
	va_end(va);

	std::vsnprintf(const_cast<char*>(out.c_str()), out.size(), str, vb);
	va_end(vb);

	if(loglevel == LOG_STDOUT)
	{
		std::cout << out << std::endl;
	} else {
		std::cerr << out << std::endl;
	}
}
