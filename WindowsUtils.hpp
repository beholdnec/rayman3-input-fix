// WindowsUtils.hpp
// Nolan Check
// Created 2/27/2012

#ifndef _WINDOWSUTILS_HPP
#define _WINDOWSUTILS_HPP

void Debug(const char* msg, ...);

static inline float Lerp(float x0, float x1, float y0, float y1, float x) {
	return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

#endif
