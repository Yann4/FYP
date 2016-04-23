#pragma once
#include <string>
#include <fstream>

class Logger
{
public:
	static Logger& Instance()
	{
		static Logger *instance = new Logger();
		return *instance;
	}

	void write(const std::string& app);

	friend Logger& operator<<(Logger& log, const std::string& str);
	friend Logger& operator<<(Logger& log, const int val);
	friend Logger& operator<<(Logger& log, void(*func)(Logger& log));
	
	static void endl(Logger& log);
private:
	std::string logFileName = "LogFile.txt";
	std::fstream file;

	Logger();
	~Logger();
};
