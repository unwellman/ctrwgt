#ifndef CTRW_LOG_H
#define CTRW_LOG_H

/* Wrapper for SDL_Log.
 * Frees my headers from including SDL_Log if I don't want them to.
 * Why this is necessary I do not know.
 * */

#include <stdarg.h>
#include <SDL3/SDL_log.h>

void log_trace (const char *fmt, ...);
void log_debug (const char *fmt, ...);
void log_info (const char *fmt, ...);
void log_warn (const char *fmt, ...);
void log_error (const char *fmt, ...);
void log_critical (const char *fmt, ...);


#endif

