#ifndef __COMDEF_H__
#define __COMDEF_H__
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define CO_LOG(LVL, M, FMT, ...)                     \
	if (LOG_LEVEL <= LVL)                            \
	{                                                \
		printf("[" M " %ld %s:%d|%s]" FMT "\n",      \
			   time(0), strrchr(__FILE__, '/') + 1,  \
			   __LINE__, __FUNCTION__, __VA_ARGS__); \
	}

#define LOG_LEVEL 0

#define CO_DBG(FMT, ...) CO_LOG(0, "DBG", FMT, __VA_ARGS__)
#define CO_ERR(FMT, ...) CO_LOG(1, "ERR", FMT, __VA_ARGS__)
#define CO_INF(FMT, ...) CO_LOG(2, "INF", FMT, __VA_ARGS__)
#define CO_FTL(FMT, ...) CO_LOG(3, "FTL", FMT, __VA_ARGS__)

#define EXPAND_PARAM(ARGS)              \
	va_list __va;                       \
	va_start(__va, ARGS);               \
	void *__arg = va_arg(__va, void *); \
	va_end(__va);

#endif