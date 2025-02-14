#ifndef _STDIO_H
#define _STDIO_H_
#include <stdio.h>

#include "include/types.h"
#include "include/bdos.h"
#include "include/errno.h"

typedef struct struct_FILE
{
    FCB     FCB;
    long    SeekPos;
    long    FileSize;
    uchar   Flags;
    uchar   Type;
    uchar   BufMode;
    size_t  DMAPtr;
    errno_t ErrNo;
    char*   DMABuf;
} FILE;

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#define BUFSIZ       128
#define FILENAME_MAX 13
#define L_tmpnam     13
#define FOPEN_MAX    32
#define TMP_MAX      25 

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

typedef long fpos_t;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int putchar(int c);
int getchar(void);
int puts(const char *s);

FILE * fopen ( const char * filename, const char * mode);
FILE * freopen ( const char * filename, const char * mode, FILE * stream );
int fclose ( FILE * stream );
int feof ( FILE * stream );
int fflush ( FILE * stream );

int remove ( const char * filename );
int rename ( const char * oldname, const char * newname );

void setbuf ( FILE * stream, char * buffer );
int setvbuf ( FILE * stream, char * buffer, int mode, size_t size );

size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );

void rewind ( FILE * stream );
int fseek ( FILE * stream, long offset, int origin );
long int ftell ( FILE * stream );
int fsetpos ( FILE * stream, const fpos_t * pos );
int fgetpos ( FILE * stream, fpos_t * pos );

int fputs ( const char * str, FILE * stream );
int fputc ( int character, FILE * stream );
int fgetc ( FILE * stream );
char * gets ( char * str );
char * fgets ( char * str, int num, FILE * stream );
int ungetc ( int character, FILE * stream );
#define getc(stream) fgetc(stream)
#define putc(c, stream) fputc(c, stream)

char * tmpnam ( char * str );
FILE * tmpfile ( void );

int ferror ( FILE * stream );
void perror ( const char * str );
void clearerr ( FILE * stream );

int vprintf ( const char * format, va_list arg );
int printf (const char *format, ...);
int fprintf ( FILE * stream, const char * format, ... );
int vfprintf ( FILE * stream, const char * format, va_list arg );

int snprintf ( char * s, size_t n, const char * format, ... );
int vsnprintf (char * s, size_t n, const char * format, va_list arg );
int vsprintf (char * s, const char * format, va_list arg );
int sprintf ( char * str, const char * format, ... );

int scanf( const char *format, ... );
int sscanf(const char *ibuf, const char *fmt, ...);
int vsscanf(const char *inp, char const *fmt0, va_list ap);
int vfscanf ( FILE * stream, const char * format, va_list arg );
int fscanf ( FILE * stream, const char * format, ... );

void exit(int);
#define eprintf(...) fprintf (stderr, __VA_ARGS__);

#endif