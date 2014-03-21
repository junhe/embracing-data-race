/* 
    Example code copied from:
    https://computing.llnl.gov/tutorials/pthreads/
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_N_THREADS 128

long long global_counter = 0;
long long cnt_per_thread = 0; // will be initialized in main()

void *BusyWork(void *t)
{
    long long i;
    long tid;
    double result=0.0;
    tid = (long)t;
    printf("Thread %ld starting...\n",tid);
    for (i=0; i<cnt_per_thread; i++)
    {
        global_counter++;
    }
    printf("Thread %ld done. Result = %e\n",tid, result);
    pthread_exit((void*) t);
}

int main (int argc, char *argv[])
{
    pthread_t thread[MAX_N_THREADS];
    pthread_attr_t attr;
    int rc;
    int nthreads;
    long t;
    void *status;

    if ( argc != 3 ) {
        printf("Usage: %s nthreads cnt_per_thread \n", argv[0]);
        exit(1);
    } 

    nthreads = atoi(argv[1]);
    cnt_per_thread = atol(argv[2]);

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(t=0; t<nthreads; t++) {
        printf("Main: creating thread %ld\n", t);
        rc = pthread_create(&thread[t], &attr, BusyWork, (void *)t); 
        if (rc) {
            printf("ERROR; return code from pthread_create() "
                   " is %d\n", rc);
            exit(-1);
        }
    }

    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for(t=0; t<nthreads; t++) {
        rc = pthread_join(thread[t], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread"
               "%ld having a status of %ld\n",t,(long)status);
    }

    printf("global counter: %lld", global_counter);
    pthread_exit(NULL);
}
