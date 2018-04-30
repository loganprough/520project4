#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/resource.h>

#include <mpi.h>

int NUM_THREADS;

double myclock();

int main(int argc, char **argv)
{
        
    int nwords, maxwords = 50000;
    int nlines, maxlines = 1000000;
    int i, k, n, err, *count, nthreads = 24;
    double nchars = 0;
    double tstart, ttotal;
    FILE *fd;
    char **word, **line;
    int *local_count = (int *) malloc( maxwords * sizeof( int ) );
    
    int startPos, endPos;

    int rc;
	int numtasks, rank;
    MPI_Status Status;
    
    struct rusage usage;
    printf("DATA, getenv('HOSTNAME'), getenv('NSLOTS'),  NUM_THREADS, 0.0, usage.ru_maxrss, usage.ru_minflt, usage.ru_majflt\n");
    getrusage(RUSAGE_SELF,&usage);

    rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    NUM_THREADS = numtasks;
	printf("size = %d rank = %d\n", numtasks, rank);
    fflush(stdout);
    
    // Malloc space for the word list and lines

   word = (char **) malloc( maxwords * sizeof( char * ) );
   count = (int *) malloc( maxwords * sizeof( int ) );
   for( i = 0; i < maxwords; i++ ) {
      word[i] = malloc( 10 );
      count[i] = 0;
   }

   line = (char **) malloc( maxlines * sizeof( char * ) );
   for( i = 0; i < maxlines; i++ ) {
      line[i] = malloc( 2001 );
   }

   // Read in the dictionary words
   if(rank==0){
        printf( "DEBUG: Started reading words\t"); fflush(stdout);
        fd = fopen( "/homes/dan/625/keywords.txt", "r" );
        nwords = -1;
        do {
            err = fscanf( fd, "%[^\n]\n", word[++nwords] );
        } while( err != EOF && nwords < maxwords );
        fclose( fd );
 
        printf( "Read in %d words\n", nwords); fflush(stdout);
    }

    // Read in the lines from the data file
    printf( "DEBUG: Started reading data\t", nwords); fflush(stdout);
    fd = fopen( "/homes/dan/625/wiki_dump.txt", "r" );
    nlines = -1;
    do {
        err = fscanf( fd, "%[^\n]\n", line[++nlines] );
        if( line[nlines] != NULL ) nchars += (double) strlen( line[nlines] );
    } while( err != EOF && nlines < maxlines);
    fclose( fd );
 
    printf( "Read in %d lines averaging %.0lf chars/line\n", nlines, nchars / nlines);
    fflush(stdout);
    printf("DATA, %s, %s, %d, %f, %ld, %ld, %ld\n", getenv("HOSTNAME"), getenv("NSLOTS"),  NUM_THREADS, 0.0, usage.ru_maxrss, usage.ru_minflt, usage.ru_majflt);
    fflush(stdout);

    // Broadcast lists
    printf( "DEBUG: Broadcasting key words\n"); fflush(stdout);
    MPI_Bcast(word, maxwords, MPI_CHAR, 0, MPI_COMM_WORLD);
    printf("DATA, %s, %s, %d, %f, %ld, %ld, %ld\n", getenv("HOSTNAME"), getenv("NSLOTS"),  NUM_THREADS, 0.0, usage.ru_maxrss, usage.ru_minflt, usage.ru_majflt);
    fflush(stdout);
    /*
    printf( "DEBUG: Broadcasting data: %d\n", maxlines * ((int) nchars)); fflush(stdout);
    MPI_Bcast(line, maxlines * ((int) nchars), MPI_CHAR, 0, MPI_COMM_WORLD);
    printf("DATA, %s, %s, %d, %f, %ld, %ld, %ld\n", getenv("HOSTNAME"), getenv("NSLOTS"),  NUM_THREADS, 0.0, usage.ru_maxrss, usage.ru_minflt, usage.ru_majflt);
    fflush(stdout);
    */
   // Loop over the word list
   startPos = ((long) rank) * (maxlines / NUM_THREADS);
   endPos = startPos + (maxlines / NUM_THREADS);
   
   printf("Rank = %d startPos = %d endPos = %d \n", rank, startPos, endPos); fflush(stdout);

   tstart = myclock();  // Set the zero time
   tstart = myclock();  // Start the clock

   for( i = 0; i < nwords; i++ ) {
    
        for( k = startPos; k < endPos; k++ ) {
            if( strstr( line[k], word[i] ) != NULL ) local_count[i]++;
        }
    }
    ttotal = myclock() - tstart;

    MPI_Reduce(local_count, count, maxwords, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    printf( "The run on %d cores took %lf seconds for %d words\n", 
        nthreads, ttotal, nwords);

    if ( rank == 0 ){
        // Dump out the word counts

        fd = fopen( "wiki.out", "w" );
        for( i = 0; i < nwords; i++ ) {
            fprintf( fd, "%d %s %d\n", i, word[i], count[i] );
        }
        fprintf( fd, "The run on %d cores took %lf seconds for %d words\n",
            nthreads, ttotal, nwords);
        fclose( fd );
    }

    MPI_Finalize();
}

double myclock() {
    static time_t t_start = 0;  // Save and subtract off each time
 
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    if( t_start == 0 ) t_start = ts.tv_sec;
 
    return (double) (ts.tv_sec - t_start) + ts.tv_nsec * 1.0e-9;
 }