#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int main(int argc, char **argv) {
  int err, numlines = atoi(argv[1]);
  FILE *fd;
  char **line = (char **)malloc(numlines * sizeof(char *));
  char *buf = malloc(100000);

  // Inspired by find_keys.c in Dr. Andresen's home directory
  fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
  for(int i = 0; i < numlines && err != EOF; i++) {
    err = fscanf(fd, "%[^\n]\n", buf);
    int l = strlen(buf);
    line[i] = malloc(l * sizeof(char));
    strncpy(line[i], buf, l);
  }
  fclose(fd);
  for(int i = 0; i < numlines; i++) printf("%s\n", line[i]);
}
