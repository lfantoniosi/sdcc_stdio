#ifndef _BDOS_H_
#define _BDOS_H_

#include "include/types.h"

typedef struct struct_FCB 
{
	uchar DR;		// drive number
	char  F[8];		// file name
	char  T[3];		// extension type
	uchar EX;		// current block
	uchar S1;		// number of blocks from the top
	uchar S2;		// record size
	uchar RC;		// record size
	ulong FSIZE;	// filesize
	uchar DATE[2];  // date last acess
	uchar TIME[2];  // time last acess
	uchar AL[8];	// platform independent
	uchar CR;		// current record
	uchar R[3];		// random record number (16-bit R[0]-R[1] with R[2] overflow)
} FCB;


void bdos(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_c_read(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
void bdos_c_write(char) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_c_rawio(char) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_c_stat(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_drv_allreset(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh);

void bdos_f_dmaoff(void*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_open(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_close(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_make(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_drv_get(void) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_drv_set(char) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_read(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_write(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_delete(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_rename(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_size(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_readrand(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_writerand(FCB*) __naked __sdcccall(1) __preserves_regs(iyl, iyh);
char bdos_f_rnd_readout(FCB*, uint) __naked __sdcccall(1) __preserves_regs(iyl, iyh);

bool bdos_init_fcb(FCB*, const char*);
long bdos_get_randrec(FCB*);
void bdos_set_randrec(FCB*, long);
int kbhit(void);

#define BDOS 0x0005


#define BDOS_C_READ			1
#define BDOS_C_WRITE		2
#define BDOS_C_RAWIO		6
#define BDOS_C_STAT			11
#define BDOS_DRV_ALLRESET	13
#define BDOS_DRV_SET 		14
#define BDOS_F_OPEN 		15
#define BDOS_F_CLOSE		16
#define BDOS_F_SFIRST		17
#define BDOS_F_NEXT			18
#define BDOS_F_DELETE		19
#define BDOS_F_READ     	20
#define BDOS_F_WRITE    	21
#define BDOS_F_MAKE     	22
#define BDOS_F_RENAME   	23
#define BDOS_DRV_GET		25
#define BDOS_F_DMAOFF		26
#define BDOS_F_READRAND		33
#define BDOS_F_WRITERAND	34
#define BDOS_F_SIZE			35
#define BDOS_F_READOUT  	39

#define BDOS_OK						0
#define BDOS_EOF					1
#define BDOS_DIR_FULL				1
#define BDOS_DISC_FULL				2
#define BDOS_INVALID_FCB			9
#define BDOS_MEDIA_CHANGED			10
#define BDOS_HW_ERROR				255

#define HW_FILE_NOT_FOUND			0
#define HW_DRIVE_SELECT				1
#define HW_READ_ONLY_DISK			2
#define HW_READ_ONLY_FILE			3
#define HW_INVALID_DRIVE			4
#define HW_FILE_ALREADY_OPEN		5
#define HW_FILE_ALREADY_EXISTS		8
#define HW_FILENAME_ERROR			9

#define TEXT_EOF					26

__at(0x005c) FCB MAIN_FCB;

#endif
