#include <string.h>
#include <ctype.h>

#include "include/bdos.h"


void bdos(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
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

void bdos_reset(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	__asm
	ld		c,#BDOS_RESET
	jp		_bdos
	__endasm;
}

char bdos_c_read(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	__asm
	ld		c,#BDOS_C_READ
	jp		_bdos
	__endasm;
}

void bdos_c_write(char c) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	c;
	__asm
	ld 		e,a
	ld		c,#BDOS_C_WRITE
	jp		_bdos
	__endasm;
}

char bdos_c_rawio(char c) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	c;
	__asm
	ld		e,a
	ld		c,#BDOS_C_RAWIO
	jp		_bdos
	__endasm;
}

char bdos_c_stat(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	__asm
	ld		c,#BDOS_C_STAT
	jp		_bdos
	__endasm;
}

char bdos_drv_allreset(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	__asm
	ld		c,#BDOS_DRV_ALLRESET
	jp		_bdos
	__endasm;
}

char bdos_drv_set(char drive) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	drive;
	__asm
	ld		e,a
	ld		c,#BDOS_DRV_SET
	jp		_bdos
	__endasm;
}

char bdos_drv_get(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	__asm
	ld		c,#BDOS_DRV_GET
	jp		_bdos
	__endasm;
}

char bdos_f_open(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex		de,hl
	ld		c,#BDOS_F_OPEN
	jp		_bdos
	__endasm;
}

char bdos_f_close(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_CLOSE
	jp		_bdos
	__endasm;
}

char bdos_f_make(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_MAKE
	jp		_bdos
	__endasm;
}

void bdos_f_dmaoff(void* offset) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	offset;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_DMAOFF
	jp		_bdos
	__endasm;
}

char bdos_f_read(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_READ
	jp		_bdos
	__endasm;
}

char bdos_f_write(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_WRITE
	jp		_bdos
	__endasm;
}

char bdos_f_delete(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_DELETE
	jp		_bdos
	__endasm;
}

char bdos_f_rename(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_RENAME
	jp		_bdos
	__endasm;
}

char bdos_f_size(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_SIZE
	jp		_bdos
	__endasm;
}

char bdos_f_readrand(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_READRAND
	jp		_bdos
	__endasm;
}

char bdos_f_writerand(FCB* fcb) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb;
	__asm
	ex 		de,hl
	ld		c,#BDOS_F_WRITERAND
	jp		_bdos
	__endasm;
}

char bdos_f_rnd_blk_wr(FCB* fcb, uint count) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb; count;
	__asm
	ex		de,hl
	ld		c,#BDOS_F_RNDBLKWR
	jp		_bdos
	__endasm;
}

uint bdos_f_rnd_blk_rd(FCB* fcb, uint count) __naked __sdcccall(1) __preserves_regs(iyl, iyh)
{
	fcb; count;
	__asm
	ex		de,hl
	ld		c,#BDOS_F_RNDBLKRD
	call	_bdos
	ex 		de,hl // return records read in DE
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
#ifdef MSXDOS1
	return *((long*)&fcb->R[0]);
#else
	return (*((long*)&fcb->R[0])) << 7;
#endif
}

void bdos_set_randrec(FCB* fcb, long extent)
{
#ifndef MSXDOS1
	extent >>= 7;
#endif
	*((long*)&fcb->R[0]) = extent;
}

int kbhit(void)
{
	if (bdos_c_stat() == 0) return 0;
	return 1;
}
