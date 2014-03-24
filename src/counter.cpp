/* 
    Example code copied from:
    https://computing.llnl.gov/tutorials/pthreads/
*/
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sstream>
#include "Util.h"
#define MAX_N_THREADS 128

using namespace std;

long long global_counter = 0;
long long cnt_per_thread = 0; // will be initialized in main()

struct timeval start, end;

/* the following two function MUST be used
 * in pair. Cannot be nested.
 * start_timer()
 * DO SOMETHING
 * duration = end_timer_get_duration()
 */
void start_timer()
{
    gettimeofday(&start, NULL);
}

double end_timer_get_duration()
{
    struct timeval result;
    gettimeofday(&end, NULL);
    timersub( &end, &start, &result );
    double duration = result.tv_sec + result.tv_usec/1000000.0;
    return duration;
}


void *BusyWork(void *t)
{
    long long i;
    //long tid;
    //double result=0.0;
    //tid = (long)t;
    for (i=0; i<cnt_per_thread; i++)
    {
        global_counter++;
    }
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
    Performance perf;

    if ( argc != 3 ) {
        printf("Usage: %s nthreads cnt_per_thread \n", argv[0]);
        exit(1);
    } 

    nthreads = atoi(argv[1]);
    cnt_per_thread = atol(argv[2]);

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    start_timer();
    for(t=0; t<nthreads; t++) {
        //printf("Main: creating thread %ld\n", t);
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
        //printf("Main: completed join with thread"
               //"%ld having a status of %ld\n",t,(long)status);
    }

    double dur = end_timer_get_duration();

    perf.put("duration", dur);
    // hate to use different names for oss
    // so, make it local
    {
        ostringstream oss;
        oss << global_counter;
        perf.put("counter" , oss.str().c_str());
    }
    long long correct_count = nthreads * cnt_per_thread;
    {
        ostringstream oss;
        oss << correct_count;
        perf.put("correct_count", oss.str().c_str());
    }
        
    cout << perf.showColumns();
    pthread_exit(NULL);
}
