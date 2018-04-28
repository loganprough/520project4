#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main()
{
    FILE *fptr;
    //char * line;
    char ch;
    int lines = 0, maxlinelength = 0, templength = 0;
    
    /*  open the file for reading */
    fptr = fopen("test.txt"
     ,"r");

    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }
    ch = fgetc(fptr);
    while (ch != EOF)
    {
        if (ch == '\n') 
        {
            if (maxlinelength < templength) maxlinelength = templength;
            templength = 0;
            lines++;
        }
        else 
        {
            templength++;
        }
        printf ("%c", ch);
        ch = fgetc(fptr);
    }
    fclose(fptr);
    printf("found %d lines for test.txt with max length of %d\n", lines, maxlinelength);
    //if(line) free(line);
}