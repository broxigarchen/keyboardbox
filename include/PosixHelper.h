
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
#include <map>
#include <iostream>
#include <vector>

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <string>
#include <stdarg.h>
#include <syslog.h>

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

class sysSignalHandler
{
	public:
	sysSignalHandler(void)
	{
		sigemptyset(&set);
		sigmap.clear();
	}

	void addsig(int sig, void(*cb)(int))
	{
		if(cb == NULL)
			return;
		sigaddset(&set, sig);
		sigmap[sig] = cb;

		act.sa_handler = cb;
		act.sa_flags = 0;
		sigaction(sig, &act, NULL);
	}

	virtual void init(void)
	{
	}

	void run(void)
	{
		int      sig;

		init();
		while(1)
		{
			int rc = sigwait(&set, &sig);
			if(rc == 0)
			{
				continue;
			}
		}
	}

	private:
		sigset_t set;
		std::map<int, void (*)(int)> sigmap;
		struct sigaction act;
};

static inline std::string parseTimeval(struct timeval& time)
{
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64];
	std::string output(64, '\0');

	//gettimeofday(&time, NULL);
	nowtime = time.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
	std::snprintf(const_cast<char*>(output.c_str()), output.size(),
				"%s.%06ld", tmbuf, time.tv_usec);
	return output;
}

static inline std::string getTimeStamp(void)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return parseTimeval(time);
}

class sysLog
{
#define SYSLOG(priority, str)\
			va_list va;\
			va_start(va, str);\
			vsyslog(LOG_ERR, str, va);\
			va_end(va);
	public:
		sysLog()
		{
			openlog(NULL, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
		}

		~sysLog()
		{
			closelog();
		}

		void alert(const char* str, ...) { SYSLOG(LOG_ALERT, str); }
		void alert(const char* str, va_list va)	{ vsyslog(LOG_ERR, str, va); }

		void critical(const char* str, ...) { SYSLOG(LOG_CRIT, str); }
		void critical(const char* str, va_list va) { vsyslog(LOG_CRIT, str, va); }

		void info(const char* str, ...) { SYSLOG(LOG_INFO, str); }
		void info(const char* str, va_list va) { vsyslog(LOG_INFO, str, va); }

		void error(const char* str, ...) { SYSLOG(LOG_ERR, str); }
		void error(const char* str, va_list va) { vsyslog(LOG_ERR, str, va); }

		void warning(const char* str, ...) { SYSLOG(LOG_WARNING, str); }
		void warning(const char* str, va_list va) { vsyslog(LOG_WARNING, str, va); }

		void debug(const char* str, ...) { SYSLOG(LOG_DEBUG, str); }
		void debug(const char* str, va_list va) { vsyslog(LOG_DEBUG, str, va); }
};

}
#endif
