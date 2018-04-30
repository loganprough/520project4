// Finds common substrings from file using pthreads
// and parameters defined below for LINES and NUM_THREADS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define LINES 100
#define NUM_THREADS 16

struct substring {
  char *s;
  int len;
} *ss[LINES];

struct substring *lss(char *a, char *b);
void *fss(void *start);

int chunkSize;
char **line;

int main(int argc, char **argv) {
  long tids[NUM_THREADS];
  void *ret;
  pthread_attr_t attr;
  int err; 
  FILE *fd;
  line = (char **)malloc(LINES * sizeof(char *));
  chunkSize = LINES / NUM_THREADS;

  // Make pthreads joinable
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // Inspired by find_keys.c in Dr. Andresen's home directory
  fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
  for(int i = 0; i < LINES && err != EOF; i++) {
    line[i] = (char *)malloc(10000 * sizeof(char));
    err = fscanf(fd, "%[^\n]\n", line[i]);
  }
  fclose(fd);

  // Make threads to do stuff
  for(int i = 0; i < LINES; i += chunkSize) pthread_create(&tids[i/chunkSize], NULL, fss, (void *)(__intptr_t)i);

  // Wait for threads to finish
  pthread_attr_destroy(&attr);
  for(int i = 0; i < NUM_THREADS; i++) pthread_join(tids[i], &ret);
  
  // Print results
  for(int i = 0; i < LINES - 1; i++) printf("%d-%d: %.*s\n", i, i+1, ss[i]->len, ss[i]->s);
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

void *fss(void *start) {
  int s = (__intptr_t)start;
  for(int i = s; i < s + chunkSize && i < LINES - 1; i++) ss[i] = lss(line[i], line[i+1]);
  pthread_exit(0);
}
