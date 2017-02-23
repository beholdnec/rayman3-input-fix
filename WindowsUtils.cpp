// WindowsUtils.cpp
// Nolan Check
// Created 2/27/2012

#include "WindowsUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <cstdio>
#include <cstdarg>

void Debug(const char* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);

	int len = _vscprintf(msg, args) + 1;

	char* buf = new char[len];
	vsprintf_s(buf, len, msg, args);

	va_end(args);
  
#if 1
  FILE* flog = NULL;
  fopen_s(&flog, "r3if-log.txt", "a");
  if (flog)
  {
    fprintf(flog, "%s", buf);
    fclose(flog);
  }
#endif
	OutputDebugStringA(buf);
	delete[] buf;
}
