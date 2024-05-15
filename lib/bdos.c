#include <string.h>
#include <ctype.h>

#include "include/bdos.h"


void bdos(void) __naked
{
	__asm
	push	ix
	push	iy
	call	BDOS
	pop		iy
	pop		ix
	ret
	__endasm;
}

uchar bdos_c_read(void) __naked
{
	__asm
	ld		c,#BDOS_C_READ
	call	_bdos
	ret
	__endasm;
}

void bdos_c_write(uchar c) __naked
{
	c;
	__asm

	ld 		e,a
	ld		c,#BDOS_C_WRITE
	call	_bdos

	ret
	__endasm;
}

uchar bdos_c_rawio(void) __naked
{
	__asm

	ld		e,#0xFF;
	ld		c,#BDOS_C_RAWIO
	call	_bdos

	ret
	__endasm;
}

uchar bdos_c_stat(void) __naked
{
	__asm

	ld		c,#BDOS_C_STAT
	call	_bdos

	ret
	__endasm;
}

uchar bdos_drv_allreset(void) __naked
{
	__asm

	ld		c,#BDOS_DRV_ALLRESET
	call	_bdos

	ret
	__endasm;
}

uchar bdos_drv_set(uchar drive) __naked
{
	drive;
	__asm

	ld		e,a
	ld		c,#BDOS_DRV_SET
	call	_bdos

	ret
	__endasm;
}

uchar bdos_drv_get(void) __naked
{
	__asm

	ld		c,#BDOS_DRV_GET
	call	_bdos
	ld  	l,a

	ret
	__endasm;
}

uchar bdos_f_open(FCB* fcb) __naked
{
	fcb;
	__asm

	ex		de,hl

	ld		c,#BDOS_F_OPEN
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_close(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_CLOSE
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_make(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_MAKE
	call	_bdos

	ret
	__endasm;
}

void bdos_f_dmaoff(void* dmaoff) __naked
{
	dmaoff;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_DMAOFF
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_read(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_READ
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_rnd_readout(FCB* fcb, uint count) __naked
{
	fcb; count;
	__asm
	push	ix
	ld		ix,#0
	add		ix,sp
	
	ex		de,hl

	ld		l, 4 (ix)
	ld		h, 3 (ix)

	ld		c,#BDOS_F_READOUT
	call	_bdos

	ld		l,a

	ld	sp, ix
	pop	ix
	ret
	__endasm;
}

uchar bdos_f_write(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_WRITE
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_delete(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_DELETE
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_rename(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_RENAME
	call	_bdos

	ret
	__endasm;
}


uchar bdos_f_size(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_SIZE
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_readrand(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_READRAND
	call	_bdos

	ret
	__endasm;
}

uchar bdos_f_writerand(FCB* fcb) __naked
{
	fcb;
	__asm

	ex 		de,hl

	ld		c,#BDOS_F_WRITERAND
	call	_bdos

	ret
	__endasm;
}


bool bdos_init_fcb(FCB* fcb, const char* filename)
{
	const char *dot, *dr;
	size_t i;
	memset(fcb, 0, sizeof(FCB));
	memset(&fcb->F[0], ' ', 11);
	
	dr = strchr(filename, ':');
	if (dr)
	{
		if (dr - filename != 1)
			return false;
		fcb->DR = toupper(dr[0]) - 'A';
		if (fcb->DR > 25) return false;
		filename = dr + 1;
	}
	else fcb->DR = bdos_drv_get();
 
	dot = strchr(filename, '.');
	if (dot)
	{
		memcpy(&fcb->F[0], filename, MIN(dot - filename, 8));
		dot++;
		memcpy(&fcb->T[0], dot, MIN(strlen(dot), 3));
	}
	else
	{
		memcpy(&fcb->F[0], filename, MIN(strlen(filename), 8));
	}
	for (i = 0; i < 11; i++)
	{
		fcb->F[i] = toupper(fcb->F[i]);
		if (fcb->F[i] == '*' || fcb->F[i] == '?' || fcb->F[i] == '\\' || fcb->F[i] == '/')
			return false;
	}
	
	return true;
}

long bdos_get_randrec(FCB* fcb)
{
	return ( ( (long)fcb->R[0]) | ((long)fcb->R[1]) << 8 | ((long)fcb->R[2]) << 16) << 7;
}

void bdos_set_randrec(FCB* fcb, long extent)
{
	extent >>= 7;
	fcb->R[0] = (uchar)(extent & 0xFF);
	fcb->R[1] = (uchar)((extent >> 8) & 0xFF);
	fcb->R[2] = (uchar)((extent >> 16) & 0xFF);
}


int kbhit(void)
{
	if (bdos_c_stat() == 0) return 0;
	return 1;
}
