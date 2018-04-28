
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omp.h>

int main(int argc, char **argv) {

    unsigned int threads = (argc > 1) ? 1 << atoi(argv[1]) : 1;

    const int NUM_ITER = 100000000;
    int i;
    double sum = 0.0, x = 0.0;
    double st = 1.0/((double) NUM_ITER);
    for(i = 50e6; i < 75e6; i++)
    {
      x = (i + 0.25)*st;
      sum += 4.0/(x*x+1);
    }

    printf("sum: %f",sum); fflush(stdout);
}