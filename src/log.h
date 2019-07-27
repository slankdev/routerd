#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void log_erro(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_debg(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif /* _LOG_H_ */
