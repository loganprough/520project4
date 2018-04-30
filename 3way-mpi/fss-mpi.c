// Finds common substrings from file using MPI 
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  void *ret;
  // pthread_attr_t attr;
  int err; 
  int i, rc;
	int numtasks, rank;
  FILE *fd;
  chunkSize = LINES / NUM_THREADS;
  line = (char **)malloc(LINES * sizeof(char *));
  MPI_Status Status;


  rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  // Make main process read file and send chunks to the others
  if ( rank == 0 ) {
		// Inspired by find_keys.c in Dr. Andresen's home directory
    fd = fopen("/homes/dan/625/wiki_dump.txt", "r");
    for(i = 0; i < LINES && err != EOF; i++) {
      line[i] = (char *)malloc(10000 * sizeof(char));
      err = fscanf(fd, "%[^\n]\n", line[i]);
    }
    fclose(fd);

    // For each chunk
    for(int i = chunkSize; i < LINES; i += chunkSize)
      // Send it to its process
      for(int k = i; k < chunkSize + i && k < LINES - 1; k++) MPI_Send(line[k], 10000, MPI_CHAR, i / chunkSize, 0, MPI_COMM_WORLD);

    // Do my part
    for(int i = 0; i < chunkSize; i++) ss[i] = lss(line[i], line[i+1]);

    // Receive the rest of the data from my friends
    int offset;
    // For each chunk
    for(int i = chunkSize; i < LINES; i += chunkSize)
      // Get the offset and set ss accordingly
      for(int k = i; k < chunkSize + i && k < LINES - 1; k++) {
        MPI_Recv(&offset, 1, MPI_INT, i / chunkSize, 1, MPI_COMM_WORLD, &Status);
        ss[k]->s = line[k] + offset;
      }

    // For each chunk
    for(int i = chunkSize; i < LINES; i += chunkSize)
      // Get the length and set ss accordingly. Yes, we're reusing the offset int and
      // that makes this code less clear.
      for(int k = i; k < chunkSize + i && k < LINES - 1; k++) {
        MPI_Recv(&offset, 1, MPI_INT, i / chunkSize, 1, MPI_COMM_WORLD, &Status);
        ss[k]->len = offset;
      }
    
    // Print results
    for(int i = 0; i < LINES - 1; i++) printf("%d-%d: %.*s\n", i, i+1, ss[i]->len, ss[i]->s);
	} 
  else {
    // Not main process, so malloc mine and get data
    char **mine = (char **)malloc(chunkSize * sizeof(char *));
    for(int i = 0; i < chunkSize; i++) {
      mine[i] = (char *)malloc(10000 * sizeof(char));
      MPI_Recv(mine[i], 10000, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &Status);
    }

    struct substring *myss[chunkSize + 1];

    // Find lss
    for(int i = 0; i < chunkSize && i + chunkSize * rank < LINES - 1; i++) myss[i] = lss(mine[i], mine[i+1]);

    // Send back offset of lss
    int tmp;
    for(int i = 0; i < chunkSize; i++) {
      tmp = (int)(myss[i]->s - line[i]); // since pointer from different process would be useless
      MPI_Send(&tmp, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    // Send back lengths of lss
    for(int i = 0; i < chunkSize; i++) MPI_Send(&myss[i]->len, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
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
