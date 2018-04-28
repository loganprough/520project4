
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <pthread.h>

pthread_mutex_t mutexsum;

#define NUM_ITER 100000000

double sum = 0.0;

struct thread_data{
  int  tID;
  int  num_threads;
};

void *count(void *tData){
  struct thread_data *my_data;
  my_data = (struct thread_data *) tData;
  int i;
  double x = 0.0, localSum = 0.0;
  double st = 1.0/((double) NUM_ITER);
  int startPos = (my_data->tID) * (NUM_ITER / my_data->num_threads);
  int endPos = startPos + (NUM_ITER / my_data->num_threads);
        
  for(i = startPos; i < endPos; i++)
  {
    x = (i + 0.25)*st;
    localSum += 4.0/(x*x+1);
  }

    
  pthread_mutex_lock (&mutexsum);
  printf("host = %s tID = %d localSum = %f \n", getenv("HOSTNAME"),my_data->tID, localSum); fflush(stdout);
  sum += localSum;
  pthread_mutex_unlock (&mutexsum);
}

int main(int argc, char **argv) {
  struct timeval t1, t2;
  double elapsedTime;
  struct rusage usage;
  int num_threads = (argc > 1) ? atoi(argv[1]) : 1;
  int i, rc;
  pthread_t threads[num_threads];
	pthread_attr_t attr;
  void *status;
  struct thread_data tData[num_threads];
  for (i = 0; i < num_threads; i++){
    tData[i].tID = i;
    tData[i].num_threads = num_threads;
  }
  
  /* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&mutexsum, NULL);

  printf("Running pthread code on host '%s' with %i threads.\n", getenv("HOSTNAME"), num_threads);

  getrusage(RUSAGE_SELF,&usage);
  gettimeofday(&t1, NULL);

  for (i = 0; i < num_threads; i++ ) {
    rc = pthread_create(&threads[i], &attr, count, (void *) &tData[i]);
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
}
  
  /* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(i=0; i<num_threads; i++) {
	     rc = pthread_join(threads[i], &status);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
	}

  gettimeofday(&t2, NULL);
  getrusage(RUSAGE_SELF,&usage);
  
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; // us to ms

  printf("sum: %f\n",sum);
  printf("DATA, %s, %d, %f, %ld, %ld, %ld, %f\n", getenv("NSLOTS"),  num_threads, elapsedTime, usage.ru_maxrss, usage.ru_minflt, usage.ru_majflt, sum);

  pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
}