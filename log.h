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

#ifndef LOG_H_
#define LOG_H_

#include <string.h>
#include <errno.h>
#include "PosixHelper.h"

#ifndef LOG_CPP
extern PosixHelper::sysLog logger;
extern bool usesyslog;
#endif

enum logLevel
{
	LOG_STDOUT,
	LOG_STDERR,
};

void logToConsole(int loglevel, const char* str, ...);

#define INFO(str, ...)\
{\
	if(usesyslog)\
		logger.info(str, ##__VA_ARGS__);\
	else\
		logToConsole(LOG_STDOUT, str, ##__VA_ARGS__);\
}

#define ERROR(str, ...)\
{\
	if(usesyslog)\
		logger.error(str, ##__VA_ARGS__);\
	else\
		logToConsole(LOG_STDERR, str, ##__VA_ARGS__);\
}

#define PERROR(str)\
{\
	if(usesyslog)\
		logger.error("%s: %s", strerror(errno));\
	else\
		logToConsole(LOG_STDERR, "%s: %s", str, strerror(errno));\
}

#define ALERT(str, ...)\
{\
	if(usesyslog)\
		logger.alert(str, ##__VA_ARGS__);\
	else\
		logToConsole(LOG_STDERR, str, ##__VA_ARGS__);\
}

#define WARNING(str, ...)\
{\
	if(usesyslog)\
		logger.warning(str, ##__VA_ARGS__);\
	else\
		logToConsole(LOG_STDOUT, str, ##__VA_ARGS__);\
}

#define CRITICAL(str, ...)\
{\
	if(usesyslog)\
		logger.critical(str, ##__VA_ARGS__);\
	else\
		logToConsole(LOG_STDERR, str, ##__VA_ARGS__);\
}

#define DEBUG(str, ...)\
{\
	if(usesyslog)\
		logger.debug(str, ##__VA_ARGS__);\
	else\
		logToConsole(LOG_STDOUT, str, ##__VA_ARGS__);\
}

#endif
