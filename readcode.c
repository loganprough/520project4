#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <Windows.h>

static char ** FileLines;
char * FilePath = "test.txt";

//void readfile();

void main()
{
    FILE * fptr;
    //char * line;
    char ch;
    int totalLines = 0, maxLineLength = 0, templength = 0;
    int i = 0;    

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
        //printf ("%c", ch);
        ch = fgetc(fptr);
    }
    //printf("found %d lines for test.txt with max length of %d\n", totalLines, maxLineLength);

    /* Allocate memory for total number of lines found */
    FileLines = (char **) malloc(totalLines*sizeof( char * ));
     if (FileLines == NULL)
    {
        printf("Failed to alloc memory \n");
        fclose(fptr);
        exit(0);
    }
    for(i=0; i<totalLines; i++ ){
        FileLines[i] = malloc(maxLineLength*sizeof( char ));
        if (FileLines == NULL)
        {
            printf("Failed to alloc memory of line %d\n", i);
            fclose(fptr);
            exit(0);
        }
    }

    // Load in each line from file
    rewind(fptr);
    i = 0;
    while (fscanf(fptr, "%[^\n]\n", FileLines[i]) != EOF)
    {
        //printf("%s\n",FileLines[i]);
        i++;
    }

    fclose(fptr);
    for(i=0; i<totalLines; i++){
        printf("%s", FileLines[i]); fflush(NULL);
    }
    for(i=0; i<totalLines; i++ ){
        if(FileLines[i]) free(FileLines[i]);
    }
    if(FileLines) free(FileLines);
    // if(line) free(line);
    printf("end\n"); fflush(NULL);
}
