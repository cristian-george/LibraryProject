#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>

#ifdef LOGGER_EXPORTS
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif

class LOGGER_API Logger
{
public:
	enum class Level : char
	{
		Info = 1,
		Warning,
		Error
	};

public:
	Logger() = default;
	Logger(std::ostream&, Level minimumLevel = Level::Info);

	friend std::ostream& operator<<(std::ostream& out, Level typeLevel)
	{
		switch (typeLevel)
		{
		case Logger::Level::Info: out << "Info";
			break;
		case Logger::Level::Warning: out << "Warning";
			break;
		case Logger::Level::Error: out << "Error";
			break;
		default:
			break;
		}

		return out;
	}
	void SetMinimumLogLevel(Level);

	template <class ... Args>
	void Log(Level typeLevel, Args&& ... params)
	{
		if (typeLevel >= minimumLevel)
		{
			out << "[" << typeLevel << "]";

			std::time_t currentTime = std::time(nullptr);
			out << "[" << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S") << "]";
			((out << " " << std::forward<Args>(params)), ...);
			out << std::endl;
		}
	}

private:
	std::ostream& out;
	Level minimumLevel;
};

