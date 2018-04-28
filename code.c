#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char ** FileLines;
char * FilePath = "test.txt";

struct substring {
  char *s;
  int len;
};

struct substring *lss(char *a, char *b);

void main()
{
    FILE * fptr;
    //char * line;
    char ch;
    int totalLines = 1, maxLineLength = 0, templength = 0;
    int i = 0, j;    

    fptr = fopen(FilePath,"r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }

    /*  This section of code calculates the total number of lines within the file, 
        as well as the maximum number of characters a line from the file can have.*/
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
        ch = fgetc(fptr);
    }
    // printf("found %d lines for test.txt with max length of %d\n", totalLines, maxLineLength);

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
    /*  Load in each line from file */
    rewind(fptr);
    // printf("rewind file\n");
    i = 0;
    while (fscanf(fptr, "%[^\n]\n", FileLines[i]) != EOF)
    {
        // printf("%s\n",FileLines[i]);
        i++;
    }
    fclose(fptr);

    // DEBUG: what the program found
    // for(i=0; i<totalLines; i++){
    //     printf("%s", FileLines[i]); fflush(NULL);
    // }
    
    // DEBUG: Testing substring and lss()
    struct substring **foundSS =  malloc(totalLines*sizeof( struct substring ));
    for(i=0; i<totalLines-1; i++){
        foundSS[i] = lss(FileLines[i], FileLines[i+1]);
    }
    // printf("foundSS[%d] %d: %s\t", i, foundSS[totalLines-2]->len, foundSS[totalLines-2]->s);
    // if(foundSS[totalLines-2]) printf("exists\n");
    // if(foundSS[totalLines-2] == NULL) printf("equals NULL\n");
    // if(foundSS[totalLines-2]->s == NULL) printf("the string equals NULL\n");
    // if(foundSS[totalLines-2]->len == 0) printf("the length is 0\n");

    // DEBUG: print the output
    char * output = (char *) calloc(maxLineLength, sizeof( char ));
    i=-1;
    while(foundSS[++i]->s != NULL){
        strncpy ( output, foundSS[i]->s, foundSS[i]->len );
        //printf("foundSS[%d] %d: %s\t", i, foundSS[i]->len, foundSS[i]->s);
        printf("%d-%d: %s\n", i+1, i+2, output);
        j=-1;
        while(++j < maxLineLength){
            output[j] = '\0';
        }
    }

    // Job well done
    for(i=0; i<totalLines; i++ ){
        if(FileLines[i]) free(FileLines[i]);
        if(foundSS[i]) free(foundSS[i]);
    }
    if(foundSS) free(foundSS);
    if(FileLines) free(FileLines);
    if(output) free(output);
    printf("end\n"); fflush(NULL);
}

struct substring *lss(char *a, char *b) {
  char *ea = a + strlen(a) - 1, *eb = b + strlen(b) - 1, *tb = b;
  int len;
  struct substring *lss = malloc(sizeof(struct substring));
  lss->len = 0;
  lss->s = NULL;

  a--;
  while(++a <= ea) {
    tb = b - 1;
    while(++tb <= eb) {
      len = 0;
      while(a <= ea && tb <= eb && *a == *tb) {
        len++;
        a++;
        tb++;
      }
      if (len > 0) {
        a -= len;
        tb -= len;
        if (len > lss->len) {
          lss->len = len;
          lss->s = a;
        }
      }
    }
  }
  return lss;
}