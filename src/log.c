#include <SDL3/SDL_log.h>
#include "log.h"

#define LOG_FUNCTION(name, prio) \
	void name (const char *fmt, ...) {\
		va_list args;\
		va_start(args, fmt);\
		SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,\
				prio, fmt, args);\
		va_end(args);\
	}

LOG_FUNCTION(log_trace, SDL_LOG_PRIORITY_TRACE)
LOG_FUNCTION(log_debug, SDL_LOG_PRIORITY_DEBUG)
LOG_FUNCTION(log_info, SDL_LOG_PRIORITY_INFO)
LOG_FUNCTION(log_warn, SDL_LOG_PRIORITY_WARN)
LOG_FUNCTION(log_error, SDL_LOG_PRIORITY_ERROR)
LOG_FUNCTION(log_critical, SDL_LOG_PRIORITY_CRITICAL)


