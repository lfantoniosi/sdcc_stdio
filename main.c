#if !defined(__GNUC__) && !defined(__clang__)
#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/errno.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#endif
#include <string.h>


FILE *test_fopen(const char *filename, const char *mode) {
    FILE *file;
    printf("Testing fopen %s...", mode);
    file = fopen(filename, mode);
    if (!file) {
        puts(":ERROR!");
        exit(1);
    } else {
        printf("OK\n");
    }
    return file;
}

void test_fclose(FILE *file) {
    printf("Testing fclose...");
    if (fclose(file) != 0) {
        puts(":ERROR!");
        exit(1);
    } 
    printf(":OK\n");
}

void test_fputc(FILE *file, const char *str, int count) {
    printf("Testing fputc...");
    for(int i = 0; i < count; i++) {
        //putchar(str[i]);
        if (fputc(str[i], file) == EOF) {
            puts(":ERROR!");
            exit(1);
        }
    }
    printf(":OK (%d) chars put\n", count);
}

void test_fgetc(FILE *file, const char *str, int count) {
    printf("Testing fgetc...");
    for(int i = 0; i < count; i++) {
        int c = fgetc(file);
        putchar(c);
        if (c == EOF) puts("<EOF>");
        if (c == EOF || (char)c != str[i]) {
            puts(":ERROR!");
            exit(1);
        }
    }
    printf(":OK (%d) chars put\n", count);
}

void test_stdio_functions(void) {
    FILE *file1, *file2;
    const char* text1 = "<Text1>Hello, World! This is a very long text made exclusively for testing the stdio functions. It should be written and read to the file correctly. It was made large enough on purpose to test the 128 bytes extents on CP/M rand make sure it works fine!</Text1>";
    const char* text2 = "<Text2>This string should have been written to the file and read back correctly</Text2>";
    const char* text3 = "<Text3>This new string should replace the old one written here, can you see it?</Text3>";

    file1 = test_fopen("test0.txt", "w");
    fputc('a', file1);
    fclose(file1);


    file1 = test_fopen("test1.txt", "w");
    test_fputc(file1, text1, 1);
    fflush(file1);
    test_fputc(file1, text1+1, 126);
    fflush(file1);
    test_fputc(file1, text1+127, strlen(text1)-127);
    fclose(file1);

    file1 = test_fopen("test1.txt", "r");
    test_fgetc(file1, text1, strlen(text1));
    fclose(file1);    


    file1 = test_fopen("test2.txt", "w");
    test_fputc(file1, text1, strlen(text1));
    fclose(file1);

    file1 = test_fopen("test2.txt", "r");
    test_fgetc(file1, text1, strlen(text1));
    fclose(file1);

    file1 = test_fopen("test2.txt", "a");
    test_fputc(file1, text2, strlen(text2));
    fclose(file1);

    file1 = test_fopen("test2.txt", "r");
    test_fgetc(file1, text1, strlen(text1));
    test_fgetc(file1, text2, strlen(text2));
    fclose(file1);

    file1 = test_fopen("test2.txt", "a+");
    test_fgetc(file1, text1, strlen(text1));
    //test_fgetc(file1, text2, strlen(text2));
    test_fputc(file1, text3, strlen(text3));
    fclose(file1);

    file1 = test_fopen("test2.txt", "r");
    test_fgetc(file1, text1, strlen(text1));
    test_fgetc(file1, text2, strlen(text2));
    test_fgetc(file1, text3, strlen(text3));
    fclose(file1);

    file1 = test_fopen("test2.txt", "r+");
    fseek(file1, 0, SEEK_END);
    printf("File size: %ld\n", ftell(file1));
    fseek(file1, strlen(text1), SEEK_SET);
    test_fputc(file1, text3, strlen(text3));
    fclose(file1);

    file1 = test_fopen("test2.txt", "r");
    test_fgetc(file1, text1, strlen(text1));
    test_fgetc(file1, text3, strlen(text3));
    test_fgetc(file1, text3, strlen(text3));
    fclose(file1);

    file1 = test_fopen("test3.txt", "w+");
    test_fputc(file1, text1, 128);
    fseek(file1, 0, SEEK_SET);
    test_fgetc(file1, text1, 128);
    fclose(file1);


}


int main(void) {
    test_stdio_functions();
    return 0;
}