#ifndef LOG_H
#define LOG_H
#ifdef LOG
#undef LOG
#endif

#ifdef ERROR
#undef ERROR
#endif

#include <syslog.h>

#define LOG_INIT(m) do { \
        setlogmask (LOG_UPTO (LOG_NOTICE)); \
	    openlog ((m), LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER); \
    } while(0)

#define LOG(a,...) syslog ((LOG_NOTICE),"antfx_log@[%s: %d]: " a "\n", __FILE__, \
	__LINE__, ##__VA_ARGS__)
	
#define ERROR(a,...) syslog ((LOG_ERR), "antfx_error@[%s: %d]: " a "\n", __FILE__, \
		__LINE__, ##__VA_ARGS__)
#endif