# sdcc_stdio
Complete stdio lib implementation for SDCC compiler for CP/M-80

This library implements the missing stdio components for SDCC Compiler v4.2 or higher.

The crt0 included will parse the command line, adds them into a list at the end of the TPA and sends to the main program as argc/argv. The argv[0] usually is the executable name but we cannot extract that from CP/M, so for compatibilty a null string is always pointed at argv[0].

The argv[] list is placed at the end of the TPA (right before the BOOT BDOS call)

The SP stack pointer is placed right before the argv[] list and by default is set to 2K (see crt0.s if you need to change this)

The malloc will use all the free TPA from the end of the program until the end of the stack pointer. So the space will vary from CP/M implementation.

All the routines including FILE stream manipulation including redirection of stdout/stdin/stderr (freopen) are supported except by setvbuf as the file buffer cannot be different than 128 bytes (CP/M BDOS requirement).

Scanf does not support float

The main.c is a test program and can be built for Pc for result comparison. Just do make pc_build and run it on PC.
