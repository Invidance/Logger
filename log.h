#pragma once
#include <string>
#include <sstream>
#include <mutex>

#define INFO "INFO"
#define DEBUG "DEBUG"
#define ERROR "ERROR"
#define WARNING "WARNING"

class Logger
{
public:
	Logger(const char* prefix);
	class Stream
	{
	public:
		friend class Logger;

		~Stream() noexcept;

		template<typename T>
		Stream& operator <<(const T& message)
		{
			buffer_ << message;
			return *this;
		}

		template<typename T>
		Stream& operator <<(const T* message)
		{
			if(message != nullptr)
				buffer_ << message;
			return *this;
		}

		Stream& operator<<(std::ostream& (*manip)(std::ostream&));

	protected:
		std::string level_;
		std::string prefix_;
		std::ostringstream buffer_;
		Logger* logger_;

		Stream(Logger* logger);
		Stream(Logger* logger, const char* level);
		Stream(Logger* logger, const char* level, const char* prefix);

		Stream(const Stream&) = delete;
		Stream& operator=(const Stream&) = delete;
		Stream(Stream&&) noexcept = default;

		void log();
		void setPrefix(const char* prefix);

	private:
	};

	template<typename T>
	Stream operator <<(const T& message)
	{
		Stream stream(this, INFO, prefix_.c_str());
		stream << message;

		return stream;
	}

	template<typename T>
	Stream operator <<(const T* message)
	{
		Stream stream(this, INFO, prefix_.c_str());
		if (message != nullptr)
			stream << message;

		return stream;
	}

	Stream operator()(const char* level);
	

	std::string getTime();
	void setPrefix(const char* prefix);
	const char* getPrefix() const;
	std::mutex& getMutex();

private:
	std::string prefix_;
	std::mutex mutex_;

	void validateString(std::string& buffer);
};

Logger getLogger(const char* prefix = "");

