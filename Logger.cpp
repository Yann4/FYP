#include "Logger.h"

Logger::Logger()
{
	file.open(logFileName.c_str(), std::fstream::out | std::fstream::trunc);
}

Logger::~Logger()
{
	file.close();
}

void Logger::write(const std::string& app)
{
	file << app;
}

void Logger::endl(Logger& log)
{
	log.file << "\n";
	log.file.flush();
}

Logger& operator<<(Logger& log, const std::string& str)
{
	log.file << str;
	return log;
}

Logger& operator<<(Logger& log, const int val)
{
	log.file << std::to_string(val);
	return log;
}

Logger& operator<<(Logger& log, void(*func)(Logger& log))
{
	func(log);
	return log;
}