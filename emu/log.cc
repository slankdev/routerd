
#include "log.h"
#include <syslog.h>
#include <stdarg.h>

void log_erro(const char *format, ...)
{
  va_list va;
  va_start(va, format);
  vsyslog(LOG_EMERG, format, va);
  va_end(va);
  va_start(va, format);
  vprintf(format, va);
  va_end(va);
}

void log_warn(const char *format, ...)
{
  va_list va;
  va_start(va, format);
  vsyslog(LOG_WARNING, format, va);
  va_end(va);
  va_start(va, format);
  vprintf(format, va);
  va_end(va);
}

void log_info(const char *format, ...)
{
  va_list va;
  va_start(va, format);
  vsyslog(LOG_INFO, format, va);
  va_end(va);
  va_start(va, format);
  vprintf(format, va);
  va_end(va);
}

void log_debg(const char *format, ...)
{
  va_list va;
  va_start(va, format);
  vsyslog(LOG_DEBUG, format, va);
  va_end(va);
  va_start(va, format);
  vprintf(format, va);
  va_end(va);
}

