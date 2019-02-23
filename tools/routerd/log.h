#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

void log_erro(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_debg(const char *fmt, ...);

#endif /* _LOG_H_ */
