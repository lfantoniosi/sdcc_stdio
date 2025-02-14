#include <string.h>
#include <stdlib.h>

#include "include/bdos.h"
#include "include/stdio.h"
#include "include/stdlib.h"

#define STREAM_STDOUT 0
#define STREAM_STDERR 1
#define STREAM_STDIN  2
#define STREAM_FILE   3
#define STREAM_TEMP	  4

#define FILE_OPEN   1
#define FILE_APPEND 2
#define FILE_READ   4
#define FILE_WRITE  8
#define FILE_UPDATE 16
#define FILE_BINARY 32
#define FILE_DIRTY	64
#define FILE_OWNBUF 128

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

int errno = EOK;

void __stdio_init(void)
{
	stdin = malloc(sizeof(FILE));
	stdin->Flags = 0;
	stdin->Type = STREAM_STDIN;
	stdin->DMAPtr = BUFSIZ;
	stdin->DMABuf = NULL;
	stdin->SeekPos = 0;
	stdin->FileSize = 0;
	stdin->ErrNo = EOK;

	stdout = malloc(sizeof(FILE));
	stdout->Flags = 0;
	stdout->Type = STREAM_STDOUT;
	stdout->DMAPtr = BUFSIZ;
	stdout->DMABuf = NULL;
	stdout->SeekPos = 0;
	stdout->FileSize = 0;
	stdout->ErrNo = EOK;

	stderr = malloc(sizeof(FILE));
	stderr->Flags = 0;
	stderr->Type = STREAM_STDERR;
	stderr->DMAPtr = BUFSIZ;
	stderr->DMABuf = NULL;
	stderr->SeekPos = 0;
	stderr->FileSize = 0;
	stderr->ErrNo = EOK;
}

void __stdio_deinit(void)
{
	fclose(stderr);
	fclose(stdout);
	fclose(stdin);
}

int putchar(int c) 
{
	if (c >= 0)
	{
		fputc(c, stdout);
	}
	return c;
}

int getchar(void)
{
	return fgetc(stdin);
}

int puts(const char *s)
{
	while(*s != NULL)
		putchar(*s++);
	putchar('\n');
	return 0;
}

char * gets ( char * str )
{
	char *s = str;
	int c;
	for(;;)
	{
		c = fgetc(stdin);
		if (c == EOF) return NULL;
		if ((char)c == '\n' || (char)c == '\r')
			break;
		if (c > 31)
		{
			*s++ = (char)c;
			*s = (char)NULL;
		} 
	}

	return str;
}

FILE* fopen_internal ( const char * filename, const char * mode, FILE* stream)
{
	bool ownbuf = false;
	bool ownstream = true;

	if (stream == NULL)
	{
		stream = (FILE*) malloc(sizeof(FILE));
		memset(stream, 0, sizeof(FILE));
		stream->Type = STREAM_FILE;
		ownstream = false;
	}

	if (stream && stream->DMABuf == 0) {
		stream->DMABuf = (char*) malloc(BUFSIZ);
		ownbuf = true;
		stream->BufMode = _IOFBF; // fully buffered
	}

	if (stream  && stream->DMABuf)
	{
		stream->Flags = ownbuf ? FILE_OWNBUF : 0;
		stream->DMAPtr = 0;
		stream->SeekPos = 0;
		stream->FileSize = 0;
		stream->ErrNo = EOK;

		for(int i = 0; i < strlen(mode); i++)
		{
			switch(mode[i])
			{
				case 'w' : stream->Flags |= FILE_WRITE;
				break;
				case 'r' : stream->Flags |= FILE_READ;
				break;
				case 'a' : stream->Flags |= FILE_APPEND;
				break;
				case '+' : stream->Flags |= FILE_UPDATE;
				break;
				case 'b' : stream->Flags |= FILE_BINARY;
				break;
				default: 
					goto fopen_fail;
			}
		}
		
		if ( (stream->Flags & FILE_APPEND) && (stream->Flags & FILE_READ) ||
			 (stream->Flags & FILE_APPEND) && (stream->Flags & FILE_WRITE) ||
			 (stream->Flags & FILE_READ) && (stream->Flags & FILE_WRITE) )
			// there can be only one
			{
				errno = EPERM ;
				goto fopen_fail;
			}

		if (filename == NULL || bdos_init_fcb(&stream->FCB, filename))
		{
			switch(stream->Flags & (FILE_APPEND | FILE_READ | FILE_WRITE))
			{
				case FILE_READ:
				case FILE_APPEND: {
					if (bdos_f_open(&stream->FCB) == 0)
					{
						stream->Flags |= FILE_OPEN;
						memset(stream->DMABuf, TEXT_EOF, BUFSIZ);

						if (bdos_f_size(&stream->FCB) == 0)
						{
							stream->FileSize = bdos_get_randrec(&stream->FCB);
							#ifdef MSXDOS1
							stream->FCB.RSIZE = 1;
							// bdos_f_size is a CP/M function working with 128-byte blocks
							if (stream->FileSize > 0)
							{
								stream->FileSize--;
							}
							stream->FileSize <<= 7;
							#endif
							if (stream->FileSize > 0)
							{
								bdos_f_dmaoff(stream->DMABuf);
								#ifdef MSXDOS1
									stream->FileSize &= ~(BUFSIZ-1);
									// seek to last 128-byte block
									bdos_set_randrec(&stream->FCB, stream->FileSize);
									// check how many bytes are in the last block
									stream->DMAPtr = bdos_f_rnd_blk_rd(&stream->FCB, BUFSIZ);
									stream->FileSize += stream->DMAPtr;
								#else
									if ((stream->Flags & FILE_BINARY) == 0) // text stream ?
									{
										bdos_set_randrec(&stream->FCB, stream->FileSize - 1);
										// read last block and look for EOL
										if (bdos_f_readrand(&stream->FCB) == 0)
										{
											for(size_t i = 0; i < BUFSIZ; i++)
											{
												stream->DMAPtr = i;

												if (stream->DMABuf[i] == TEXT_EOF) // text EOL
												{
													stream->FileSize -= (BUFSIZ - 1 - i);
													stream->FileSize = MAX(stream->FileSize - 1, 0); // ignore the TEXT_EOF
													break;
												}
											}
										}
									}
									else 
									{
										// binary stream, assume end of the block
										stream->DMAPtr = BUFSIZ - 1;
									}
								#endif
							}

							if ((stream->Flags & FILE_APPEND) && ((stream->Flags & FILE_UPDATE) == 0))
							{
								stream->SeekPos = stream->FileSize;	// non-update append always at the end of stream
							} 
							else 
							{
								// FILE_READ / FILE_UPDATE
								stream->DMAPtr = 0;
								stream->SeekPos = 0;
								if (stream->FileSize > 0)
								{
									bdos_f_dmaoff(stream->DMABuf);
									bdos_set_randrec(&stream->FCB, 0);
									#ifdef MSXDOS1
										// test if can read first block
										if (bdos_f_rnd_blk_rd(&stream->FCB, BUFSIZ) == 0)
										{
											errno = EIO;					
											goto fopen_fail;
										}
									#else
										// test if can read first block
										if (bdos_f_readrand(&stream->FCB)) {
											errno = EIO;					
											goto fopen_fail;
										} 
									#endif
								}
							}
						}
						else { 
							errno = EIO;
							goto fopen_fail;
						} 
					}
					else
					{
						if (stream->Flags & FILE_APPEND)
						{
							// stream cannot be open, try creating it
							if (bdos_f_make(&stream->FCB) == 0)
							{
								stream->Flags |= FILE_OPEN;
								memset(stream->DMABuf, TEXT_EOF, BUFSIZ);
								#ifdef MSXDOS1
								stream->FCB.RSIZE = 1;
								#endif
							} 
							else {
								errno = EIO;
								goto fopen_fail;
							}
						} 
						else {
							errno = ENOENT; // file read fail
							goto fopen_fail;
						}
					}
				}
				break;
				case FILE_WRITE:
				{
					bdos_f_delete(&stream->FCB);
					if (bdos_f_make(&stream->FCB) == 0)
					{
						#ifdef MSXDOS1
						stream->FCB.RSIZE = 1;
						#endif						
						stream->Flags |= FILE_OPEN;
						memset(stream->DMABuf, TEXT_EOF, BUFSIZ);
					} 
					else { 
						errno = EIO;
						goto fopen_fail;
					}					
				}
				break;
				default: 
				{
					errno = EBADF;
					goto fopen_fail;
				}
				break;
			}

			return stream;
		}
		else errno = EBADF;
	}

fopen_fail:

	if (stream != NULL)
	{
		if ((stream->Flags & FILE_OPEN) != 0)
		{
			bdos_f_close(&stream->FCB);
		}

		if (stream->DMABuf && (stream->Flags & FILE_OWNBUF))
			free(stream->DMABuf);

		if (ownstream == false)
			free(stream);
	}
	return NULL;
}

int fclose_internal ( FILE * stream )
{
	if (fflush(stream) == 0)
	{
		if ((stream->Flags & FILE_OPEN) == 0) return 0;

		if (bdos_f_close(&stream->FCB) == 0)
		{
			stream->Flags &= ~FILE_OPEN;
			return 0;
		}
	}

	errno = EIO;
	stream->ErrNo = EIO;
	return EOF;
}

FILE* fopen ( const char * filename, const char * mode)
{
	if (filename != NULL)
		return(fopen_internal(filename, mode, NULL));
	errno = EBADF;
	return NULL;
}

int fclose ( FILE * stream )
{
	if (fclose_internal(stream) == 0)
	{	

		if (stream->Type == STREAM_TEMP)
		{
			char tmpNam[13];
			memcpy(tmpNam, stream->FCB.F, 8);
			tmpNam[8] = '.';
			memcpy(&tmpNam[9], stream->FCB.T, 3);
			tmpNam[12] = (char)NULL;
			remove(tmpNam);
			return 0;
		}

		if (stream->DMABuf && (stream->Flags & FILE_OWNBUF))
			free(stream->DMABuf);
		free(stream);
		return 0;
	}
	return EOF;
}

FILE * freopen ( const char * filename, const char * mode, FILE * stream )
{
	if (filename == NULL && (stream->Type == STREAM_FILE || stream->Type == STREAM_TEMP))
	{
		errno = EPERM;
		return NULL;
	}

	if (fclose_internal(stream) == 0)
	{
		if (stream->Type == STREAM_TEMP)
		{
			free(stream);
			return NULL;
		}
		if (filename) return fopen_internal(filename, mode, stream);

		if (stream->DMABuf && (stream->Flags & FILE_OWNBUF))
		{
			free(stream->DMABuf);
		}
		stream->Flags = 0;
		stream->DMAPtr = BUFSIZ;
		stream->DMABuf = NULL;
		stream->SeekPos = 0;
	}
	return NULL;
}

int fputc_internal ( int character, FILE * stream ) _REENTRANT
{
	if ( (stream->Type == STREAM_STDOUT || stream->Type == STREAM_STDERR) && ((stream->Flags & FILE_OPEN) == 0) )
	{
		bdos_c_write((char)character);

		if (character == '\n')
			bdos_c_write((char)'\r'); // automatically adds CR after NL on screen

		return 0;
	}
	else if (stream->Type == STREAM_STDIN)
	{
		stream->ErrNo = EIO;
		errno = EIO;
		return EOF;
	} 
	else if ( (stream->Flags & FILE_OPEN) && (stream->Flags & (FILE_APPEND | FILE_WRITE | FILE_UPDATE)) )
	{
		if (stream->DMAPtr == BUFSIZ) 
			if (fflush(stream))
				return EOF;

		if (stream->DMAPtr < BUFSIZ)
		{
			if (stream->Flags & FILE_APPEND)
			{
				stream->DMAPtr = stream->FileSize & (BUFSIZ - 1);
				stream->SeekPos = stream->FileSize;
			}

			stream->DMABuf[stream->DMAPtr++] = (char) character;
			stream->Flags |= FILE_DIRTY;

			if ((stream->DMAPtr == BUFSIZ || stream->BufMode == _IONBF) || (stream->BufMode == _IOLBF && character == '\n'))
			{
				if (fflush(stream))
					return EOF;
			}

			if (stream->FileSize == stream->SeekPos)
				stream->FileSize++;		// end of the file, increment file size

			stream->SeekPos++;

			return character;
		}
	}

	errno = EIO;
	stream->ErrNo = EIO;

	return EOF;
}

int fputc( int character, FILE * stream ) _REENTRANT
{
	int ret;
	
	ret = fputc_internal(character, stream);

	return ret;
}


int fgetc ( FILE * stream ) _REENTRANT
{
	if ( (stream->Type == STREAM_STDOUT || stream->Type == STREAM_STDERR) )
	{
		stream->ErrNo = EIO;
		errno = EIO;
		return EOF;
	}
	else if ( stream->Type == STREAM_STDIN && ((stream->Flags & FILE_OPEN) == 0) )
	{
		return (int)bdos_c_read();
	} 
	else if (stream->SeekPos >= stream->FileSize)
		return EOF;

	if (stream->DMAPtr < BUFSIZ)
	{
		stream->SeekPos++;
		return (stream->DMABuf[stream->DMAPtr++]);
	}
	else
	{
		bdos_f_dmaoff(stream->DMABuf);
		#ifdef MSXDOS1
			bdos_set_randrec(&stream->FCB, stream->SeekPos & ~(BUFSIZ - 1));
			if (bdos_f_rnd_blk_rd(&stream->FCB, BUFSIZ) > 0)
			{
				stream->DMAPtr = (stream->SeekPos) & (BUFSIZ - 1);
				stream->SeekPos++;
				return (stream->DMABuf[stream->DMAPtr++]);
			}
		#else
			bdos_set_randrec(&stream->FCB, stream->SeekPos);
			if (bdos_f_readrand(&stream->FCB) == 0)
			{
				stream->DMAPtr = (stream->SeekPos) & (BUFSIZ - 1);
				stream->SeekPos++;
				return (stream->DMABuf[stream->DMAPtr++]);
			}
		#endif
	}

	return EOF;
}

int fflush( FILE * stream )
{
	stream->ErrNo = EOK;
	errno = EOK;
	if ( (stream->Flags & FILE_OPEN) && (stream->Flags & (FILE_APPEND | FILE_WRITE | FILE_UPDATE)) )
	{
		if (stream->Flags & FILE_DIRTY)
		{
			bdos_f_dmaoff(stream->DMABuf);		
			#ifdef MSXDOS1
				for(int i = 0; i < stream->DMAPtr; i++)
				{
					printf("%c", stream->DMABuf[i]);
				}
				bdos_set_randrec(&stream->FCB, stream->SeekPos & ~(BUFSIZ - 1));
				if (bdos_f_rnd_blk_wr(&stream->FCB, stream->DMAPtr) == 0)
				{
					stream->Flags &= ~FILE_DIRTY;
					stream->DMAPtr = (stream->DMAPtr) & (BUFSIZ - 1);
					return 0;
				} else goto fflush_fail;
			#else
				bdos_set_randrec(&stream->FCB, stream->SeekPos);
				if (bdos_f_writerand(&stream->FCB) == 0)
				{
					stream->Flags &= ~FILE_DIRTY;
					stream->DMAPtr = (stream->DMAPtr) & (BUFSIZ - 1);
					if (stream->SeekPos == stream->FileSize) 
					{
						memset(stream->DMABuf + stream->DMAPtr, TEXT_EOF, BUFSIZ - stream->DMAPtr);
					}
					return 0;
				} else goto fflush_fail;
			#endif			
		}
		else
		{
			if (stream->Flags & FILE_APPEND)
				stream->SeekPos = stream->FileSize;
			stream->DMAPtr = (stream->SeekPos) & (BUFSIZ - 1);
			bdos_f_dmaoff(stream->DMABuf);		
			#ifdef MSXDOS1
				bdos_set_randrec(&stream->FCB, stream->SeekPos & ~(BUFSIZ - 1));
				bdos_f_rnd_blk_rd(&stream->FCB, stream->DMAPtr);
			#else
				bdos_set_randrec(&stream->FCB, stream->SeekPos);
				bdos_f_readrand(&stream->FCB);
			#endif
			return 0;
		}
	} 
	else return 0;

fflush_fail:
	errno = EIO;
	stream->ErrNo = EIO;

	return EOF;
}

int fputs ( const char * str, FILE * stream )
{
	while(*str != NULL)
		if (fputc(*str++, stream) == EOF)
			return EOF;
	return 0;
}

void rewind ( FILE * stream )
{
	fflush(stream);
	if (stream->Flags & (FILE_READ | FILE_WRITE | FILE_UPDATE))
	{
		bdos_set_randrec(&stream->FCB, 0);
		stream->DMAPtr = BUFSIZ;
		stream->SeekPos = 0;		
	}
}

int fseek ( FILE * stream, long offset, int origin )
{
	if (stream->Flags & FILE_DIRTY)
		if (fflush(stream))
			return EOF;

	if (origin == SEEK_SET)
	{
		if (offset <= stream->FileSize)
		{
			bdos_set_randrec(&stream->FCB, offset);
			stream->SeekPos = offset;
			stream->DMAPtr = BUFSIZ;
			return 0;
		}
	}
	else if (origin == SEEK_CUR)
	{
		if (stream->SeekPos + offset <= stream->FileSize)
		{
			bdos_set_randrec(&stream->FCB, stream->SeekPos + offset);
			stream->SeekPos += offset;
			stream->DMAPtr = BUFSIZ;
			return 0;
		}
	}
	else if (origin == SEEK_END)
	{
		if (stream->FileSize + offset <= stream->FileSize)
		{
			bdos_set_randrec(&stream->FCB, stream->FileSize + offset);
			stream->SeekPos = stream->FileSize + offset;
			stream->DMAPtr = BUFSIZ;
			return 0;
		}
	}

	return EOF;
}

long int ftell ( FILE * stream )
{
	return stream->SeekPos;
}

int feof ( FILE * stream )
{
	if (stream->Type == STREAM_STDIN) return 0; // stdin never EOF

	return (stream->SeekPos == stream->FileSize) ? EOF : 0;
}

int ferror ( FILE * stream )
{
	return stream->ErrNo;
}

int fsetpos ( FILE * stream, const fpos_t * pos )
{
	if (fseek(stream, *pos, SEEK_SET) != EOF)
		return 0;

	return EIO;
}

int fgetpos ( FILE * stream, fpos_t * pos )
{
	*pos = ftell(stream);
	return 0;
}

size_t fread ( void * ptr, size_t size, size_t count, FILE * stream )
{
	char *p = (char *)ptr;
	int b;			
	size_t c;
	for(size_t c = 0; c < count; c++)
		for(size_t s = 0; s < size; s++)
		{
			if ((b = fgetc(stream)) != EOF)
			{
				*p++ = (char)b;
			} 
			else return c;
		}

	return c;
}

size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream )
{
	char *p = (char *)ptr;
	size_t c;
	for(size_t c = 0; c < count; c++)
		for(size_t s = 0; s < size; s++)
		{
			if(fputc(*p++, stream) == EOF)	
				return c;
		}

	return c;
}

void setbuf ( FILE * stream, char * buffer )
{
	if (buffer)
	{
		if (stream->Flags & FILE_OWNBUF) 
		{			
			free(stream->DMABuf);
			stream->Flags &= ~FILE_OWNBUF;
		}
		stream->DMABuf = buffer;
	}
	else
	{
		if ((stream->Flags & FILE_OWNBUF) == 0) 
		{			
			// CPM default buffer
			stream->DMABuf = (char*)malloc(BUFSIZ);
			stream->Flags |= FILE_OWNBUF;
			stream->BufMode = _IONBF; // treat as unbuffered
		}
	}
}

int setvbuf ( FILE * stream, char * buffer, int mode, size_t size )
{
	errno = EOPNOTSUPP;
	if (size == BUFSIZ)
	{
		switch(mode)
		{
			case _IOFBF:
			case _IOLBF:
			case _IONBF:
				setbuf(stream, buffer);
				stream->BufMode = mode;
				errno = EOK;
			break;
			default:
			break;
		}
	}
	return errno;
}

char * fgets ( char * str, int num, FILE * stream )
{
	int c;
	char *p = str;

	if (feof(stream) || num == 0) return NULL;

	for(int i = 0; i < num-1; i++)
	{
		c = fgetc(stream);

		if (c == EOF) break;

		*p++ = c;

		if (c == '\n') break;
	}

	*p = '\0';
	return str;
}

int remove ( const char * filename )
{
	int e = EOK;

	FILE *stream = (FILE*) malloc(sizeof(FILE));
	stream->Flags = 0;
	stream->DMAPtr = 0;
	stream->SeekPos = 0;
	stream->FileSize = 0;
	stream->ErrNo = EOK;

	errno = ENOENT;
	if (bdos_init_fcb(&stream->FCB, filename))
	{
		stream->ErrNo = bdos_f_delete(&stream->FCB);
		errno = stream->ErrNo;
	}
	free(stream);
	return errno;
}

int rename ( const char * oldname, const char * newname )
{
	FCB from;
	FCB to;
	memset(&from, 0, sizeof(FCB));
	memset(&to, 0, sizeof(FCB));
	errno = EBADF;
	if (bdos_init_fcb(&from, oldname))
	{
		if (bdos_init_fcb(&to, newname))
		{
			char* c = (char*)&from;
			memcpy(c + 17, to.F, 11);
			errno = bdos_f_rename(&from);
		}
	}
	return errno;
}

static char tempname[13];

char * tmpnam ( char * str )
{
	FCB tmpFCB;
	int c;
	memset(&tmpFCB, 0, sizeof(FCB));
	for(;;)
	{
		for(c = 0; c < 11; c++) {
			tmpFCB.F[c] = (char)(rand() % 26) + 'A';
		}

		if (bdos_f_open(&tmpFCB) == 0)
		{
			if (bdos_f_close(&tmpFCB) != 0) break;
		} 
		else
		{
			memcpy(tempname, tmpFCB.F, 8);
			tempname[8] = '.';
			memcpy(&tempname[9], tmpFCB.T, 3);
			tempname[12] = (char)NULL;
			if (str) {
				strcpy(str, tempname);
				return str;
			}
			return tempname;
		} 
	}

	return 0;
}

FILE * tmpfile ( void )
{
	FILE *tmpFile = fopen(tmpnam(NULL), "wb+");
	if (tmpFile)
	{
		tmpFile->Type = STREAM_TEMP;
	}
	return tmpFile;
}

int ungetc ( int character, FILE * stream )
{
	if (stream->Type == STREAM_FILE || stream->Type == STREAM_TEMP)
	{
		if (stream->Flags & FILE_READ)
		{
			stream->ErrNo = EOK;
			errno = EOK;
			
			if (stream->SeekPos > 0)
			{
				stream->SeekPos--;
				fflush(stream);
				stream->DMAPtr = stream->SeekPos % BUFSIZ;
				stream->DMABuf[stream->DMAPtr] = (char) character;
				return character;
			}
		}		
	}
	
	return EOF;
}