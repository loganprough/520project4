#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

//static char ** FileLines;
char * FilePath = "test.txt";

//void readfile();

void main()
{
    FILE * fptr;
    char * line;
    char ch;
    int totalLines = 0, maxLineLength = 0, templength = 0;
    
    /*  open the file for reading */
    fptr = fopen(FilePath,"r");
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
            if (maxLineLength < templength) maxLineLength = templength;
            templength = 0;
            totalLines++;
        }
        else 
        {
            templength++;
        }
        printf ("%c", ch);
        ch = fgetc(fptr);
    }
    printf("found %d lines for test.txt with max length of %d\n", totalLines, maxLineLength);
    //line = malloc(maxLineLength*sizeof(char));
    // FileLines = malloc(totalLines*sizeof(line));
    /*if (line == NULL)
    {
        printf("Failed to alloc memory \n");
        fclose(fptr);
        exit(0);
    }*/
    rewind(fptr);

    int i = 0;
    /*while (fscanf(fptr, line) != EOF)
    {
        printf("%s",line);
    //     memcpy (FileLines[i],line,strlen(line)+1);
    //     i++;
    }*/

    fclose(fptr);
    // for(i=0; i<totalLines; i++){
    //     printf(FileLines[i]);
    // }
    //if(FileLines) free(FileLines);
    //if(line) free(line);
    printf("end\n");
}
