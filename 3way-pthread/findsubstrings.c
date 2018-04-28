#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct substring {
  char *s;
  int len;
};

struct substring *lss(char *a, char *b);

int main(int argc, char **argv) {
  int err, numlines = atoi(argv[1]);
  FILE *fd;
  char **line = (char **)malloc(numlines * sizeof(char *));

  // Inspired by find_keys.c in Dr. Andresen's home directory
  fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
  for(int i = 0; i < numlines && err != EOF; i++) {
    line[i] = (char *)malloc(10000 * sizeof(char));
    err = fscanf(fd, "%[^\n]\n", line[i]);
  }
  fclose(fd);

  //for(int i = 0; i < numlines; i++) printf("%s\n", line[i]);
  
  char a[6] = "ababc\0";
  char b[6] = "babca\0";
  char c[6] = "abcba\0";

  struct substring *ss = lss(a, b);
  printf("%d: %s\n", ss->len, ss->s);
  fflush(stdout);
  //ss = lss(a, c);
  //printf("%d: %s\n", ss->len, ss->s);
  //ss = lss(b, c);
  //printf("%d: %s\n", ss->len, ss->s);
}

struct substring *lss(char *a, char *b) {
  char *ea = a + strlen(a) - 1, *eb = b + strlen(b) - 1;
  char len;
  struct substring *lss = malloc(sizeof(struct substring));
  lss->len = 0;
  lss->s = NULL;

  while(a++ < ea) {
    printf("Outmost while\n");
    fflush(stdout);
    while(b++ < eb) {
      printf("Middle while\n");
      fflush(stdout);
      len = 0;
      while(*a != 0 && *b != 0 && *a == *b && len >= lss->len) {
        printf("Innermost while\n");
        fflush(stdout);
        len++;
        a++;
        b++;
      }
      a -= len + 1;
      b -= len + 1;
      lss->len = len;
      lss->s = a - 1;
    }
  }
  return lss;
}
