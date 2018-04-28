
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <omp.h>

double sum = 0.0;

void count(int threads, int NUM_ITER){
  int i, tID;
  double x = 0.0, localSum = 0.0;
  double st = 1.0/((double) NUM_ITER);
  #pragma omp private(tID,x,localSum,startPos,endPos,i)
  {
    tID = omp_get_thread_num();
    int chunksize = (NUM_ITER / threads);

    #pragma omp for schedule(dynamic, chunksize)
    for(i = 0; i < NUM_ITER; i++)
    {
      x = (i + 0.25)*st;
      localSum += 4.0/(x*x+1);
    }

    #pragma omp critical
    {
      printf("host = %s tID = %d localSum = %f \n", getenv("HOSTNAME"), tID, localSum); fflush(stdout);
      sum += localSum;
    }
  }
}

int main(int argc, char **argv) {

  const int NUM_ITER = 100000000;
  unsigned int threads = (argc > 1) ? atoi(argv[1]) : 1;

  struct timeval t1, t2;
  double elapsedTime;
  struct rusage usage;

  omp_set_num_threads(threads);
  printf("Running OpenMP code on host '%s' with %i threads.\n", getenv("HOSTNAME"), threads);

  getrusage(RUSAGE_SELF,&usage);
  gettimeofday(&t1, NULL);
  #pragma omp parallel shared(sum)
	{
    count(threads, NUM_ITER);
  }
  
  getrusage(RUSAGE_SELF,&usage);
  gettimeofday(&t2, NULL);
  
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms

  printf("sum: %f\n",sum);
  printf("DATA, %s, %d, %f, %ld, %ld, %ld, %f\n", getenv("NSLOTS"),  threads, elapsedTime, usage.ru_maxrss, usage.ru_minflt, usage.ru_majflt, sum);
}