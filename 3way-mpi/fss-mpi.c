// Finds common substrings from file using MPI 
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINES 1000000
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
  void *ret;
  // pthread_attr_t attr;
  int err; 
  int i, rc;
	int numtasks, rank;
  FILE *fd;
  chunkSize = LINES / NUM_THREADS;
  MPI_Status Status;

  rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  line = (char **)malloc(LINES * sizeof(char *));
  for(i = 0; i < LINES; i++) {
      line[i] = (char *)malloc(10000 * sizeof(char));
  }

  if ( rank == 0 ) {
		// Inspired by find_keys.c in Dr. Andresen's home directory
    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    for(i = 0; i < LINES && err != EOF; i++) {
      err = fscanf(fd, "%[^\n]\n", line[i]);
    }
    fclose(fd);
	}

  
  MPI_Bcast(line, LINES * 10000, MPI_CHAR, 0, MPI_COMM_WORLD);

  for(int i = chunksize * rank; i < chunkSize * (rank + 1) - 1; i++) ss[i] = lss(line[i], line[i+1]);
  //
  // Need to collect char* and len within substring from all threads
  //
  MPI_Barrior(MPI_COMM_WORLD);
  for(int i = chunksize * rank; i < chunkSize * (rank + 1) - 1; i++) {
    MPI_Gather(ss[i]->s, 1, MPI_CHAR, 
      ss[i]->s, 1, MPI_CHAR, 
      0, MPI_COMM_WORLD);

    MPI_Gather(ss[i]->len, 1, MPI_INT, 
      ss[i]->len, 1, MPI_INT, 
      0, MPI_COMM_WORLD);
  }
  MPI_Barrior(MPI_COMM_WORLD);


  // Print results
  if (rank = 0) {
    for(int i = 0; i < LINES - 1; i++) printf("%d-%d: %.*s\n", i, i+1, ss[i]->len, ss[i]->s);
  }
  MPI_Finalize();
  return 0;
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
