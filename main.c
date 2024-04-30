#include<stdio.h>
#include<string.h>

#ifndef _PC_BUILD_

#include "include/types.h"
#include "include/bdos.h"
#include "include/stdio.h"
#else
#include <stdbool.h>
#endif

const char loremIpsum[] = "<<Lorem ipsum dolor sit amet. Eos corrupti ipsam in obcaecati molestias et quisquam quasi sit amet molestias "     \
                          "et doloremque quis! In cupiditate commodi ut odio nisi sit aspernatur ipsa vel dolores consectetur ut galisum "  \
                          "quia.\n\r\n\rSed soluta tempore non quaerat maiores aut commodi autem in autem placeat! Nam molestias eius vel "    \
                          "quasi quia sit ratione facere et possimus excepturi est facilis quia et consequatur officiis ea ducimus "        \
                          "velit.\n\r\n\rSit autem odio et consequatur iusto ut consequatur odit et voluptatem accusantium a eveniet "         \
                          "temporibus et veritatis voluptates et saepe voluptatem. Et aspernatur natus et dolores repudiandae est "         \
                          "rerum veritatis ea recusandae dicta et iste nesciunt. Et ipsa fugit quo illo incidunt 33 alias odit. Aut "       \
                          "enim dolorum id ipsam nihil in nihil suscipit rem nisi eligendi.>>\n\r\n\r\n\r";

bool writeLoremIpsum(FILE* stream)
{
    const char* p;
    for(p = loremIpsum; *p; p++)
    {
        if (fputc(*p, stream) == EOF)
            return false;
    }

    return true;
}

bool verifyLoremIpsum(FILE* stream)
{
    const char* p;
    int c;
    for(p = loremIpsum; *p; )
    {
        c = fgetc(stream);

        if (c == EOF || c != *p)
            return false;

        p++;
    }

    return true;
}

enum {
    ST_AppendCreate,
    ST_AppendReadEOF,
    ST_AppendWrite,
    ST_AppendCloseAndReopen,
    St_ReadAppend,
    ST_RewindAppendAndRead,
    ST_AppendMore,
    ST_DoubleReadAppend,
    ST_CreateWriteUpdate,
    ST_CopyFromAppendToWrite,
    ST_ScanFirstWords,
    ST_End,
    ST_Fail,
};

int curState = ST_AppendCreate;
FILE* fFileSrc = NULL;
FILE* fFileDst = NULL;

bool stdio_test()
{
    int c;
    char buf[200];
    char word1[50], word2[50], word3[50];
    while(curState != ST_End && curState != ST_Fail)
    {
        switch (curState)
        {
            case ST_AppendCreate:
                remove("testsrc.txt");
                remove("testdst.txt");
                printf("Create append file testsrc.txt...");
                if (!(fFileSrc = fopen("testsrc.txt", "a")))
                    curState = ST_Fail;
            break;
            case ST_AppendReadEOF:
                printf("Reading from created append file...");
                if (fgetc(fFileSrc) != EOF)
                    curState = ST_Fail;
            break;
            case ST_AppendWrite:
                printf("Writing to append file...");
                if (!writeLoremIpsum(fFileSrc))
                    curState = ST_Fail;
            break;
            case ST_AppendCloseAndReopen:
                printf("Closing and reopening append update file testsrc.txt...");
                if (fclose(fFileSrc))
                    curState = ST_Fail;
                else 
                    if (!(fFileSrc = fopen("testsrc.txt", "a+")))
                        curState = ST_Fail;                    
            break;
            case St_ReadAppend:
                printf("Reading from append update file...");
                if (!verifyLoremIpsum(fFileSrc))
                    curState = ST_Fail;
            break;
            case ST_RewindAppendAndRead:
                printf("Rewinding append update and re-read...");
                rewind(fFileSrc);
                if (!verifyLoremIpsum(fFileSrc))
                    curState = ST_Fail;
            break;
            case ST_AppendMore:
                printf("Rewinding while appending more...");
                rewind(fFileSrc);
                if (!writeLoremIpsum(fFileSrc))
                    curState = ST_Fail;
            break;
            case ST_DoubleReadAppend:
                printf("Rewinding anbd double reading append update...");
                rewind(fFileSrc);
                if (!verifyLoremIpsum(fFileSrc))
                    curState = ST_Fail;
                if (!verifyLoremIpsum(fFileSrc))
                    curState = ST_Fail;                
            break;
            case ST_CreateWriteUpdate:
                printf("Creating testdst.txt file...");           
                if (!(fFileDst = fopen("testdst.txt", "wb")))
                   curState = ST_Fail;              
            break;
            case ST_CopyFromAppendToWrite:
                printf("Copying from testsrc.txt to testdst.txt...");
                rewind(fFileSrc);
                while(!feof(fFileSrc))
                {
                    int n = fread(buf, 1, 200, fFileSrc);
                    if (fwrite(buf, 1, n, fFileDst) != n)
                    {
                        curState = ST_Fail;
                        break;                    
                    }                    
                }
            break;
            case ST_ScanFirstWords:
                rewind(fFileSrc);
                printf("Scanning first words from file...");
                if (fgets(buf, 200, fFileSrc))
                {
                    sscanf(buf, "%s %s %s", word1, word2, word3);
                    printf("(%s) (%s) (%s).", word1, word2, word3);
                }
                else curState = ST_Fail;             
          }

        if (curState != ST_Fail)
        {
            puts("Ok");
            curState++;
        }
        else
        {
            puts("FAILED");
        }
    }

    fclose(fFileSrc);
    fclose(fFileDst);

    return curState == ST_End;
}


int main(int argc, char** argv)
{
    int num;
    argc; argv;

    printf("STDIO lib for SDCC\n");

    if (argc > 1)
    {
        printf("%d parameters parsed:\n", argc-1);
        for(int i = 1; i < argc; i++)
        {
            printf("%d : %s\n", i, argv[i]);
        }
    } else puts("No paramters parsed");

    stdio_test();

    return 0;
}
