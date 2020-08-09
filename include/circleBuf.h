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

#ifndef CIRCLEBUF_H_
#define CIRCLEBUF_H_

#include <vector>
#include <mutex>
#include <condition_variable>
#include <csignal>

template <class T>
class circleBuf
{
	public:
		circleBuf(size_t size)
		{
			buf.resize(size);
			outpos = 0;
			inpos = 0;
			packets = 0;
		}

		virtual ~circleBuf(void)
		{

		}

		bool ifempty(void)
		{
			std::lock_guard<std::mutex> lock(qlock);
			return (packets == 0);
		}

		bool push_back(T& element)
		{
			std::unique_lock<std::mutex> lock(qlock);
			if(packets >= buf.size())
			{
				return false;
			}
			buf[inpos].element = element;
			increment();
			qdata.notify_all();
			return true;
		}

		T pop_front(void)
		{
			std::unique_lock<std::mutex> lock(qlock);
			qdata.wait(lock, [this]{return packets > 0;});
			size_t pos = outpos;
			decrement();
			return buf[pos].element;
		}

	private:
		class cell
		{
			public:
			T element;
		};

		size_t outpos;
		size_t inpos;
		size_t packets;
		std::vector<cell> buf;
		std::mutex qlock;
		std::condition_variable qdata;

		void increment(void)
		{
			inpos++;
			packets++;
			if(inpos >= buf.size())
			{
				inpos = 0; 
			}
		}

		void decrement(void)
		{
			outpos++;
			packets--;
			if(outpos >= buf.size())
			{
				outpos = 0; 
			}

		}
};

#endif
