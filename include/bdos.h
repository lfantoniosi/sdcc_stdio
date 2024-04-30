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

int kbhit();

void bdos();
uchar bdos_c_read();
void bdos_c_write(uchar c);
uchar bdos_c_rawio();
uchar bdos_c_stat();
uchar bdos_drv_allreset();
uchar bdos_drv_get();

bool bdos_init_fcb(FCB* fcb, const char* filename);
void bdos_f_dmaoff(void* dmaoff);
uchar bdos_f_open(FCB* fcb);
uchar bdos_f_close(FCB* fcb);
uchar bdos_f_make(FCB* fcb);
uchar bdos_drv_get();
uchar bdos_drv_set(uchar drive);
uchar bdos_f_read(FCB* fcb);
uchar bdos_f_write(FCB* fcb);
uchar bdos_f_delete(FCB* fcb);
uchar bdos_f_rename(FCB* fcb);
uchar bdos_f_size(FCB* fcb);
uchar bdos_f_readrand(FCB* fcb);
uchar bdos_f_writerand(FCB* fcb);

uchar bdos_f_rnd_readout(FCB* fcb, uint count);

long bdos_get_randrec(FCB* fcb);
void bdos_set_randrec(FCB* fcb, long extent);

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
