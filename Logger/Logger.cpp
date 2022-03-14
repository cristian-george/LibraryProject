#include "Logger.h"

Logger::Logger(std::ostream& out, Level minimumLevel) :
    out(out), minimumLevel(minimumLevel) {}

void Logger::SetMinimumLogLevel(Level minimumLevel)
{
    this->minimumLevel = minimumLevel;
}