
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

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <string>

namespace PosixHelper
{

#define LOG(...)

static inline bool sysCmdIsAvailable(void)
{
	return (0 != std::system(NULL));
}

static inline std::string exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result("");
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

static inline int grantRootPriviliege(const char* name)
{
	char cmd[PATH_MAX];
	snprintf(cmd, PATH_MAX,"sudo chmod -v u+s %s", name);
	return std::system(const_cast<const char*>(cmd));
}

static inline int switchToRoot(void)
{
	return setuid(0); 
}

static inline int runAsDaemon(void)
{
	pid_t pid, sid;

	pid = fork();
	if(pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if(pid > 0) {
		/* quit parent */
		std::cout << "Daemon running..." << std::endl;
		exit(EXIT_SUCCESS);
	}
	/* child pid start */

	/* change file mode mask */
	umask(0);

	/* get a sid */
	sid = setsid();
	if(sid < 0) {
		LOG(errno);
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		LOG(errno);
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	return sid;
}

static inline void sleep(int ms)
{
	::sleep(ms);
}

class SysSignalHandler
{
	public:
	SysSignalHandler(void (*cleanup)(void))
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

static inline std::string parseTimeval(struct timeval time)
{
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64];
	std::string output(64, '\0');

	gettimeofday(&time, NULL);
	nowtime = time.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
	std::snprintf(const_cast<char*>(output.c_str()), output.size(),
				"%s.%06ld", tmbuf, time.tv_usec);
	return output;
}

}
#endif
