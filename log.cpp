#include "log.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <syncstream>
#include <format>
#include <utility>
#include <algorithm>

Logger getLogger(const char* prefix) 
{
	return Logger(prefix);
}
// ================== Stream ==================
Logger::Stream::Stream(Logger* logger) : logger_(logger), level_(INFO), prefix_("")
{
	logger_ = logger;
}
Logger::Stream::Stream(Logger* logger, const char* level) : logger_(logger), level_(level), prefix_("")
{
}
Logger::Stream::Stream(Logger* logger, const char* level, const char* prefix) : logger_(logger), level_(level), prefix_(prefix)
{
}
Logger::Stream::~Stream()
{
	log();
}
// OPERATORS
Logger::Stream& Logger::Stream::operator<<(std::ostream& (*manip)(std::ostream&))
{
	manip(buffer_);
	return *this;
}

// FUNCS
void Logger::Stream::setPrefix(const char* prefix)
{
	prefix_ = prefix == nullptr ? "" : prefix;
}
void Logger::Stream::log()
{
	std::string buff = buffer_.str();
	auto thread_id = std::this_thread::get_id();
	if (logger_ != nullptr)
		logger_->validateString(buff);
	
	std::osyncstream(std::cout) << (logger_ != nullptr ? logger_->getTime() : "") << "; " << level_ << "; " << prefix_ << "(" << thread_id << "): " << buff << std::endl;
}

// ================== Logger ==================
Logger::Logger(const char* prefix) : prefix_(prefix == nullptr ? "" : prefix)
{
	validateString(prefix_);
}
// OPERATORS
Logger::Stream Logger::operator()(const char* level)
{
	return Stream(this, level != nullptr ? level : INFO, prefix_.c_str());
}
// FUNCTIONS
void Logger::setPrefix(const char* prefix)
{
	prefix_ = prefix == nullptr ? "" : prefix;
	validateString(prefix_);
}
const char* Logger::getPrefix() const
{
	return prefix_.c_str();
}
std::mutex& Logger::getMutex()
{
	return mutex_;
}

std::string Logger::getTime()
{
	auto now = std::chrono::system_clock::now();
	auto local_zone = std::chrono::current_zone();
	std::chrono::zoned_time local_time(local_zone, now);
	return std::format("{:%d.%m.%Y %H:%M:%S}", local_time);
}
void Logger::validateString(std::string& buffer)
{
	std::erase(buffer, '\n');
}