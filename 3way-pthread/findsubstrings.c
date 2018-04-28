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

  struct substring *ss;
  for(int i = 0; i < numlines - 1; i++) {
    ss = lss(line[i], line[i+1]);
    printf("%d-%d: %.*s\n", i, i+1, ss->len, ss->s);
  }
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
