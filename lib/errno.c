#include <string.h>
#include <stdlib.h>

#include "include/stdio.h"
#include "include/bdos.h"
#include "include/stdlib.h"


extern int errno;


void clearerr ( FILE * stream )
{
	stream->ErrNo = EOK;
}

void perror ( const char * str )
{
	switch(errno)
	{
		case EOK:
			break;
		case EPERM:
			eprintf("\n\rERRNO(%d) - Operation not permitted: %s\n\r", errno, str);
		break;
		case ENOENT:
			eprintf("\n\rERRNO(%d) - No such file or directory: %s\n\r", errno, str);
		break;
		case EIO:
			eprintf("\n\rERRNO(%d) - Input/output error: %s\n\r", errno, str);
		break;
		case EBADF:
			eprintf("\n\rERRNO(%d) - Bad file descriptor: %s\n\r", errno, str);
		break;
		case EOPNOTSUPP:
			eprintf("\n\rERRNO(%d) - Operation not supported %s\n\r", errno, str);
		break;
		default:
			eprintf("\n\rERRNO(%d) - Unknown error: %s\n\r", errno, str);
		break;

	}
}