#include <string.h>
#include <stdlib.h>

#include "include/stdio.h"
#include "include/bdos.h"
#include "include/stdlib.h"

static void put_char_to_stream (char c, void* p) _REENTRANT
{
	fputc((int)c, (FILE*)p);
}

int vprintf (const char *format, va_list arg)
{
  return _print_format (put_char_to_stream, stdout, format, arg);
}

int printf (const char *format, ...)
{
  va_list arg;
  int i;

  va_start (arg, format);
  i = _print_format (put_char_to_stream, stdout, format, arg);
  va_end (arg);

  return i;
}

int fprintf ( FILE * stream, const char * format, ... )
{
  va_list arg;
  int i;

  va_start (arg, format);
  i = _print_format (put_char_to_stream, stream, format, arg);
  va_end (arg);

  return i;
}

int vfprintf ( FILE * stream, const char * format, va_list arg )
{
  return _print_format (put_char_to_stream, stream, format, arg);
}

typedef struct nBuf_struct
{
	int Len;
	char *Buf;

} nBuf_t;

static void put_char_to_nbuffer (char c, void* p) _REENTRANT
{
	nBuf_t *nbuf = (nBuf_t*)p;
	if(nbuf->Len > 1)
	{
		nbuf->Len--;
		nbuf->Buf[0] = c;
		nbuf->Buf[1] = (char)NULL;
		nbuf->Buf++;
	}
}

int snprintf ( char * s, size_t n, const char * format, ... )
{
  	va_list arg;
  	int i;

	nBuf_t nbuf;
	nbuf.Len = n;
	nbuf.Buf = s;

  	va_start (arg, format);
  	i = _print_format (put_char_to_nbuffer, &nbuf, format, arg);
  	va_end (arg);

  	return i;

}

int vsnprintf (char * s, size_t n, const char * format, va_list arg )
{
	nBuf_t nbuf;
	nbuf.Len = n;
	nbuf.Buf = s;

	return _print_format (put_char_to_nbuffer, &nbuf, format, arg);
}

int vsprintf (char * s, const char * format, va_list arg )
{

	nBuf_t nbuf;
	nbuf.Len = MAXINT;
	nbuf.Buf = s;
	return _print_format (put_char_to_nbuffer, &nbuf, format, arg);
}

int sprintf ( char * str, const char * format, ... )
{
	char* buf = str;
  	va_list arg;
  	int i;

	nBuf_t nbuf;
	nbuf.Len = MAXINT;
	nbuf.Buf = str;

  	va_start (arg, format);
  	i = _print_format (put_char_to_nbuffer, &nbuf, format, arg);
  	va_end (arg);

  	return i;
}