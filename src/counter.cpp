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
pthread_mutex_t counter_mutex;

long long delay_factor;
long long intra_delay;

void *counter_per_thread_DRF(void *t)
{
    long long i;
    //long long delay;
    //long tid;
    
    //We are baseline, we don't need delay
    //tid = (long)t;
    //delay = delay_factor * tid;
    //for (i=0; i<delay; i++);

    for (i=0; i<cnt_per_thread; i++)
    {
        pthread_mutex_lock(&counter_mutex);
        global_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    pthread_exit((void*) t);
}

void *counter_per_thread_DRF_coarselock(void *t)
{
    long long i;
    //long long delay;
    //long tid;
    
    //We are baseline, we don't need delay
    //tid = (long)t;
    //delay = delay_factor * tid;
    //for (i=0; i<delay; i++);

    pthread_mutex_lock(&counter_mutex);
    for (i=0; i<cnt_per_thread; i++)
    {
        global_counter++;
    }
    pthread_mutex_unlock(&counter_mutex);
    pthread_exit((void*) t);
}


void *counter_per_thread_DR(void *t)
{
    long long i;
    //long long j;
    long long delay;
    long tid;
    
    tid = (long)t;
    delay = delay_factor * tid;
    for (i=0; i<delay; i++);

    for (i=0; i<cnt_per_thread; i++)
    {
        //for (j=0; j<intra_delay; j++);
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
    int mode;
    long t;
    void *status;
    Performance perf;
    struct timeval start;

    if ( argc != 6 ) {
        printf("Usage: %s nthreads cnt_per_thread mode delay_factor intra_delay \n"
               "mode: 0-data race, 1-data race free\n", argv[0]);
        exit(1);
    } 

    nthreads = atoi(argv[1]);
    cnt_per_thread = atol(argv[2]);
    mode = atol(argv[3]);
    delay_factor = atol(argv[4]);
    intra_delay = atol(argv[5]);

    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&counter_mutex, NULL);

    start_timer(&start);
    for(t=0; t<nthreads; t++) {
        void *(*routine)(void*);
        if ( mode == 0 ) {
            routine = &counter_per_thread_DR;
        } else if ( mode == 1 ) {
            routine = &counter_per_thread_DRF;
        } else if ( mode == 2 ) {
            routine = &counter_per_thread_DRF_coarselock;
        }
        rc = pthread_create(&thread[t], &attr, routine, (void *)t); 
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

    double dur = end_timer_get_duration(&start);

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
    perf.put("num_threads", nthreads);
    {
        ostringstream oss;
        if ( mode == 0 ) {
            oss << "DR";
        } else if ( mode == 1 ) {
            oss << "DRF";
        } else if ( mode == 2 ) {
            oss << "DRF_COARSE";
        }
        perf.put("mode", oss.str().c_str());
    }
    {
        ostringstream oss;
        oss << delay_factor;
        perf.put("delay_factor", oss.str().c_str());
    }
    {
        ostringstream oss;
        oss << intra_delay;
        perf.put("intra_delay", oss.str().c_str());
    }
        
    cout << perf.showColumns();
    pthread_mutex_destroy(&counter_mutex);
    pthread_exit(NULL);
}
