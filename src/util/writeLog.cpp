#include "writeLog.hpp"
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <thread>

/*
The default log level is the log level that is in effect
when the driver starts up. Once a DSN is loaded, the
log level defined in the DSN will override this default
level.
*/
#ifdef DEBUG
LogLevel DEFAULT_LOG_LEVEL = LL_TRACE;
#else
LogLevel DEFAULT_LOG_LEVEL = LL_NONE;
#endif

LogLevel CURRENT_LOG_LEVEL = DEFAULT_LOG_LEVEL;

std::ofstream* logStreamPtr = nullptr;

static std::ofstream* getLogStream() {
  if (logStreamPtr == nullptr) {
    logStreamPtr =
        new std::ofstream("C:\\temp\\odbclog.txt", std::ios_base::app);
  }
  return logStreamPtr;
}

std::wofstream* wideLogStreamPtr = nullptr;

static std::wofstream* getWideLogStream() {
  if (wideLogStreamPtr == nullptr) {
    wideLogStreamPtr =
        new std::wofstream("C:\\temp\\odbclog.txt", std::ios_base::app);
  }
  return wideLogStreamPtr;
}

void setLogLevel(LogLevel level) {
  CURRENT_LOG_LEVEL = level;
}

LogLevel getLogLevel() {
  return CURRENT_LOG_LEVEL;
}

void WriteLog(LogLevel level, std::ostringstream& oss) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ofstream* log = getLogStream();
  // Trace-level logging includes a thread id suffix on every log entry.
  if (CURRENT_LOG_LEVEL == LL_TRACE) {
    oss << " [Thread " << std::this_thread::get_id() << "]";
  }
  *log << oss.str() << std::endl;
}

void WriteLog(LogLevel level, std::wostringstream& oss) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::wofstream* log = getWideLogStream();
  // Trace-level logging includes a thread id suffix on every log entry.
  if (CURRENT_LOG_LEVEL == LL_TRACE) {
    oss << " [Thread " << std::this_thread::get_id() << "]";
  }
  *log << oss.str() << std::endl;
}

void WriteLog(LogLevel level, const std::string& s) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ostringstream oss;
  oss << s;
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, const std::wstring& s) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::wostringstream oss;
  oss << s;
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, const char* message) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ostringstream oss;
  oss << message;
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, unsigned char* message, int length) {
  /*
  The unsigned char* may not be null terminated, so we need to pass
  the length along as well to prevent reading past the end of the array.

  If the length value is negative, we interpret this to mean it is
  null terminated.
  */
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }

  std::string logMessage;
  if (length >= 0) {
    logMessage = std::string(reinterpret_cast<char*>(message), length);
  } else {
    logMessage = std::string(reinterpret_cast<char*>(message));
  }
  std::ostringstream oss;
  oss << logMessage;
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, const std::map<std::string, std::string>& m) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ostringstream oss;
  for (const auto& pair : m) {
    oss << pair.first << ": " << pair.second << "; ";
  }
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, const json& jsonData) {
  WriteLog(level, jsonData.dump(2));
}

void WriteLog(LogLevel level, void* p) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ostringstream oss;
  oss << p;
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, unsigned int i) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ostringstream oss;
  oss << i;
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, int64_t i) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ostringstream oss;
  oss << i;
  WriteLog(level, oss);
}

void WriteLog(LogLevel level, uint64_t i) {
  if (level < CURRENT_LOG_LEVEL) {
    return;
  }
  std::ostringstream oss;
  oss << i;
  WriteLog(level, oss);
}
