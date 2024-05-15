CC = sdcc
ASM = sdasz80
ASMFLAGS =
OBJDIR = obj
LIBDIR = lib
INCDIR = include

C_SOURCES = bdos.c stdio.c stdlib.c
ASM_SOURCES = crt0.s
HEADERS =  types.h bdos.h stdio.h stdlib.h errno.h

ifeq ($(EXCLUDE_PRINTF), true)
	CFLAGS += -DEXCLUDE_PRINTF
	EXCLUDE_ERRNO = true
else
	C_SOURCES += printf.c
endif

ifeq ($(EXCLUDE_SCANF), true)
	CFLAGS += -DEXCLUDE_SCANF
	EXCLUDE_ERRNO = true
else
	C_SOURCES += scanf.c
endif

ifeq ($(EXCLUDE_ERRNO), true)
	CFLAGS += -DEXCLUDE_ERRNO
else
	C_SOURCES += errno.c
endif

C_OBJ := $(patsubst %.c, $(OBJDIR)/%.rel, $(C_SOURCES))
ASM_OBJ := $(patsubst %.s, $(OBJDIR)/%.rel, $(ASM_SOURCES))
OBJECTS = $(ASM_OBJ) $(C_OBJ)
INCLUDES := $(patsubst %.h, $(INCDIR)/%.h, $(HEADERS))

CFLAGS = -mz80 --opt-code-size --std-sdcc11 -I./

%.asm : %.c
	$(CC) $(CFLAGS) -S $<

main.com: $(OBJECTS) main.rel
	$(CC) $(CFLAGS) --data-loc 0 --code-loc 0x200 --no-std-crt0 $^ -omain.ihx
	hex2bin -e com main.ihx
	#cp main.com ../stdiodsk/main.com

all: main.com

pc_build:
	gcc main.c -D _PC_BUILD_ -o main

$(OBJDIR)/%.rel: $(LIBDIR)/%.c $(INCLUDES)
	$(CC) $(CFLAGS) -o$@ -c $<

$(OBJDIR)/%.rel: $(LIBDIR)/%.s
	$(ASM) $(ASMFLAGS) -o $@ $<

%.rel : %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o$@ -c $<

%.rel : %.s
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	+$(RM) *.rel *.asm *.lst *.sym *.map *.ihx main.com *.lk *.noi obj/* testsrc.txt testdst.txt
