#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>

#define true 1
#define false 0

#define MAXINT  32767

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char bool;
typedef unsigned long ulong;
typedef long fpos_t;
typedef unsigned short uintptr_t;

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)

#include <stdio.h>

#ifdef USE_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#define DBGBRK { char* d = 0xfffd; *d = (char) 0; }
#else
#define DEBUG(...)
#define DBGBRK
#endif

#endif