#ifndef __ErrorHandling_h__
#define __ErrorHandling_h__
#include <stdarg.h>

void err_vfatalf(const char* format, va_list args);
void err_fatalf(const char* format, ...);
void err_checkGL(const char* msg);
bool err_clearGL();
void err_checkSDL(const char* msg);
bool err_clearSDL();

#endif
