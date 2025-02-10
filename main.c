#ifdef __SDCC__
#include "include/stdio.h"
#include "include/errno.h"
#else
#include <stdio.h>
#include <errno.h>
#endif
#include <string.h>

void test_stdio_functions(void) {
    FILE *file;
    char buffer[1024]; // Large enough to hold the entire text
    fpos_t pos;
    int c;

    // Test putchar and getchar
    puts("Testing putchar and getchar.");
    puts("Press a key");
    putchar('>');
    c = getchar();
    puts("");
    printf("Character read: %c\n", c);

    // Test puts
    puts("Testing puts");
    puts("Hello, World!");

    // Test fopen, fwrite, fclose
    puts("Testing fopen, fwrite, and fclose");
    puts("Creating testfile.txt");
    file = fopen("testfile.txt", "w");
    if (file == NULL) {
        perror("fopen");
        return;
    } else {
        printf("fopen:OK\n");
    }

    const char *text = "Hello, World!";
    size_t written = fwrite(text, sizeof(char), strlen(text), file);
    if (written != strlen(text)) {
        perror("fwrite");
    } else {
        printf("fwrite:OK\n");
    }
    if (fclose(file) != 0) {
        perror("fclose");
    } else {
        printf("fclose:OK\n");
    }

    // Test longer write
    puts("Testing longer write");
    file = fopen("testfile.txt", "w");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    const char *long_text = "This is a longer text to test fwrite with more data. "
                            "It should be written to the file correctly. "
                            "This text is intentionally made longer than 128 characters "
                            "to test the buffer size and ensure that fwrite handles it correctly.";
    written = fwrite(long_text, sizeof(char), strlen(long_text), file);
    if (written != strlen(long_text)) {
        perror("fwrite");
    } else {
        printf("fwrite:OK\n");
    }
    if (fclose(file) != 0) {
        perror("fclose");
    } else {
        printf("fclose:OK\n");
    }

    // Test freopen, fread, feof, fseek, ftell, fgetpos, fsetpos, rewind
    puts("Testing freopen, fread, feof, fseek, ftell, fgetpos, fsetpos, and rewind");
    file = fopen("testfile.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    file = freopen("testfile.txt", "r", file);
    if (file == NULL) {
        perror("freopen");
        return;
    }
    size_t read = fread(buffer, sizeof(char), sizeof(buffer) - 1, file);
    if (read == 0 && ferror(file)) {
        perror("fread");
    } else {
        printf("fread:OK\n");
    }
    buffer[read] = '\0';
    printf("Read from file: %s\n", buffer);
    if (strcmp(buffer, long_text) == 0) {
        printf("fread content:OK\n");
    } else {
        printf("fread content:ERROR!\n");
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek");
    } else {
        printf("fseek:OK\n");
    }
    if (ftell(file) != 0) {
        perror("ftell");
    } else {
        printf("ftell:OK\n");
    }
    if (fgetpos(file, &pos) != 0) {
        perror("fgetpos");
    } else {
        printf("fgetpos:OK\n");
    }
    if (fsetpos(file, &pos) != 0) {
        perror("fsetpos");
    } else {
        printf("fsetpos:OK\n");
    }
    rewind(file);
    if (feof(file)) {
        perror("feof");
    } else {
        printf("feof:OK\n");
    }
    if (fclose(file) != 0) {
        perror("fclose");
    } else {
        printf("fclose:OK\n");
    }

    // Test longer read
    puts("Testing longer read");
    file = fopen("testfile.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    read = fread(buffer, sizeof(char), sizeof(buffer) - 1, file);
    if (read == 0 && ferror(file)) {
        perror("fread");
    } else {
        printf("fread:OK\n");
    }
    buffer[read] = '\0';
    printf("Read from file: %s\n", buffer);
    if (strcmp(buffer, long_text) == 0) {
        printf("fread content:OK\n");
    } else {
        printf("fread content:ERROR!\n");
    }
    if (fclose(file) != 0) {
        perror("fclose");
    } else {
        printf("fclose:OK\n");
    }

    // Test remove, rename
    puts("Testing remove and rename");
    if (rename("testfile.txt", "renamed.txt") != 0) {
        perror("rename");
    } else {
        file = fopen("renamed.txt", "r");
        if (file == NULL)            
            printf("rename:ERROR!\n");
        else {
            printf("rename:OK\n");
            fclose(file);
        }
    }
    if (remove("renamed.txt") != 0) {
        perror("remove");
    } else {
        file = fopen("renamed.txt", "r");
        if (file == NULL)            
            printf("remove:OK\n");
        else {
            printf("remove:ERROR!\n");
            fclose(file);
        }
    }

    // Test setbuf, setvbuf, fflush
    puts("Testing setbuf, setvbuf, and fflush");
    file = fopen("testfile.txt", "w+");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    setbuf(file, buffer);
    if (setvbuf(file, buffer, _IOFBF, BUFSIZ) != 0) {
        perror("setvbuf");
    } else {
        printf("setvbuf:OK\n");
    }
    if (fflush(file) != 0) {
        perror("fflush");
    } else {
        printf("fflush:OK\n");
    }
    if (fclose(file) != 0) {
        perror("fclose");
    } else {
        printf("fclose:OK\n");
    }

    // Test fputs, fputc, fgetc, fgets
    puts("Testing fputs, fputc, fgetc, and fgets");
    file = fopen("testfile.txt", "w+");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    if (fputs("Hello, World!", file) == EOF) {
        perror("fputs");
    } else {
        printf("fputs:OK\n");
    }
    if (fputc('A', file) == EOF) {
        perror("fputc");
    } else {
        printf("fputc:OK\n");
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek");
    } else {
        printf("fseek:OK\n");
    }
    c = fgetc(file);
    if (c == EOF) {
        perror("fgetc");
    } else {
        putchar(c);
        printf("fgetc:OK\n");
    }
    if (fgets(buffer, BUFSIZ, file) == NULL) {
        perror("fgets");
    } else {
        printf("Read from file: %s\n", buffer);
        printf("fgets:OK\n");
    }
    if (fclose(file) != 0) {
        perror("fclose");
    } else {
        printf("fclose:OK\n");
    }

    // Test edge cases
    puts("Testing edge cases");

    // fopen with invalid mode
    puts("Testing fopen with invalid mode");
    file = fopen("testfile.txt", "invalid_mode");
    if (file == NULL) {
        perror("fopen with invalid mode");
    } else {
        printf("fopen with invalid mode:OK\n");
    }

    // remove with non-existent file
    puts("Testing remove with non-existent file");
    if (remove("non_existent_file.txt") != 0) {
        perror("remove non-existent file");
    } else {
        printf("remove non-existent file:OK\n");
    }

    // rename with non-existent file
    puts("Testing rename with non-existent file");
    if (rename("non_existent_file.txt", "new_name.txt") != 0) {
        perror("rename non-existent file");
    } else {
        printf("rename non-existent file:OK\n");
    }

    // setbuf with NULL pointer
    puts("Testing setbuf with NULL pointer");
    file = fopen("testfile.txt", "r+");
    if (file != NULL) {
        setbuf(file, NULL);
        printf("setbuf:OK\n");
        fclose(file);
    }

    // setvbuf with invalid mode
    puts("Testing setvbuf with invalid mode");
    file = fopen("testfile.txt", "r+");
    if (file != NULL) {
        if (setvbuf(file, buffer, -1, BUFSIZ) != 0) {
            perror("setvbuf with invalid mode");
        } else {
            printf("setvbuf with invalid mode:OK\n");
        }
        fclose(file);
    }

    // fseek with invalid origin
    puts("Testing fseek with invalid origin");
    file = fopen("testfile.txt", "r+");
    if (file != NULL) {
        if (fseek(file, 0, -1) != 0) {
            perror("fseek with invalid origin");
        } else {
            printf("fseek with invalid origin:OK\n");
        }
        fclose(file);
    }
}

int main(void) {
    test_stdio_functions();
    return 0;
}