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


#ifndef ACTORS_H_
#define ACTORS_H_

#include <map>

class actor
{
public:
	actor(void) {}
	actor(std::string name): actorName(name) {} 
	virtual ~actor(void) {}

	int registerCallback(std::string& comboname, void* (*callback)(void*))
	{
		auto search = actor::cbmap.find(comboname);
		if(search == actor::cbmap.end()) 
		{
			return -1;
		}
		search->second = callback;
		return 0;
	}
	int registerCallback(const char* comboname, void* (*callback)(void*))
	{
		std::string name(comboname);
		return this->registerCallback(name, callback);
	}

	void*(*getCb(std::string comboname))(void*)
	{
		auto search = actor::cbmap.find(comboname);
		if(search == actor::cbmap.end()) 
		{
			return nullptr;
		}
		return search->second;
	}
	void*(*getCb(const char* comboname))(void*)
	{
		std::string name(comboname);
		return this->getCb(name);
	}

	virtual int init(void) = 0;

private:
	static std::map<std::string, void*(*)(void*)> cbmap;
	std::string actorName;
};





#endif
