#pragma once

#include<string>
#include<functional>
#include<Event/WindowEvent.h>


namespace MV
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

}