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

struct node
{
    int num;                          // the num should be 1 for each node
    node * next;                      // in order to count how many nodes added
};                                  // into the hash table

struct thread_data                  
{
    int thread_id;
    int table_size;
    int insertion_num_per_thread;
};

node** table;                     // this is the shared table
pthread_mutex_t insert_mutex;     // this is the lock




void hash_table_init(int table_size)
{
    table = new node*[table_size];
    for(int i =0; i < table_size; i++)
    {
        table[i] = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//data race free
void *insert_per_thread_DRF(void *threadarg)
{
    struct thread_data *my_data;                                           //pass the data into the thread
    my_data = (struct thread_data *) threadarg;
    int table_size = my_data -> table_size;
    int insertion_num_per_thread = my_data->insertion_num_per_thread;
    int random; 

    for(int i =0; i < insertion_num_per_thread;++i){

        pthread_mutex_lock(&insert_mutex);                                     // hash table index is calculated by 
        random = rand()%table_size;                                            // taking mod of the table size
        int index = random;
        if(table[index] == NULL)                                              // if the table[index] has not been inserted
        {
            table[index] = new node;
            table[index]->num = 1;
            table[index]->next = NULL;
        }
        else                                                                  // if the table[index] has already been inserted
        {
            node * current = table[index];
            while(current->next !=NULL)
            {
                current = current->next;
            }
            node * add_node = new node;
            current->next = add_node;
            add_node->num = 1;
            add_node->next =NULL;
        }
        pthread_mutex_unlock(&insert_mutex);
    }
    pthread_exit((void*)threadarg);
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//data race
void *insert_per_thread_DR(void *threadarg)
{
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    int table_size = my_data -> table_size;
    int insertion_num_per_thread = my_data->insertion_num_per_thread;
    int random;

    for(int i =0; i < insertion_num_per_thread;++i){

        random = rand()%table_size;
        int index = random;
        if(table[index] == NULL)
        {
            table[index] = new node;
            table[index]->num = 1;
            table[index]->next = NULL;
        }
        else
        {
            node * current = table[index];
            while(current->next !=NULL)
            {
                current = current->next;
            }

            node * add_node = new node;
            current->next = add_node;
            add_node->num = 1;
            add_node->next =NULL;
        }
    }
    pthread_exit((void*)threadarg);
}


///////////////////////////////////////////////////////////////////////////////////
//this function is to calculate how many actual nodes inserted in the hash table//
//////////////////////////////////////////////////////////////////////////////////
int  actual_insertion_times(int table_size)
{
    int counter =0;

    for(int i=0; i < table_size; ++i)
    {
        node *temp = table[i];
        while (temp != NULL)
        {
            counter = counter + temp->num;
            temp = temp->next;
        }

    }

    return counter;
}


//////////////////////////////////////////////////////////////////////////////
//delete all the dynamic allocated memory////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void remove_all(int table_size)
{
    for (int i =0; i< table_size;++i)
    {
        node *temp = table[i];
        while (temp!=NULL)
        {
            node * next = temp->next;
            delete temp;
            temp = next;
        }
    }
}


int main(int argc, char *argv[])
{
    //  pthread_t thread[MAX_N_THREADS];
    pthread_attr_t attr;
    int rc;
    int nthreads;
    int mode;
    int table_size;
    long insertions_per_thread;
    long t;
    void * status;
    Performance perf;
    struct timeval start;

    if( argc != 5 ){
        printf("Usage: %s nthreads tablesize insertions_per_thread mode \n"
                "mode: 0-data race, 1-data race free\n", argv[0]);
        exit(1);
    }
    nthreads = atoi(argv[1]);
    table_size = atoi(argv[2]);
    insertions_per_thread = atol(argv[3]);
    mode = atol(argv[4]);

    hash_table_init(table_size);           //call hash table initialization
    // set it to a fix number so it is more fair
    // for drf and dr
    srand(0);                              //set the random seed



    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&insert_mutex,NULL);

    thread_data * ptr;
    ptr = (thread_data *) malloc (sizeof(thread_data)*nthreads);

    pthread_t * thread;
    thread = (pthread_t *) malloc(sizeof(pthread_t)*nthreads);

    start_timer(&start);                                           //time start
    for(t=0;t<nthreads;t++){
        void * (*routine)(void*);
        if ( mode ==0 ){
            routine = &insert_per_thread_DR;
        }else{
            routine = &insert_per_thread_DRF;
        }
        ptr[t].thread_id = t;
        ptr[t].table_size = table_size;
        ptr[t].insertion_num_per_thread = insertions_per_thread;
        rc = pthread_create(&thread[t], &attr, routine, (void *) (&ptr[t]));
        if (rc) {
            printf("ERROR; return code from pthread_create()"
                    " is %d\n", rc);
            exit(-1);
        }
    }
    pthread_attr_destroy(&attr);
    for(t=0;t<nthreads;t++){
        rc = pthread_join(thread[t],&status);
        if(rc){
            printf("ERROR; return code from pthread_join() is %d\n",rc);
            exit(-1);
        }
    }

    double dur = end_timer_get_duration(&start);

    {
        ostringstream oss;
        oss << actual_insertion_times(table_size);
        perf.put("actual.insertions", oss.str().c_str());
    }


    {
        ostringstream oss;
        oss << insertions_per_thread * nthreads;
        perf.put("correct.insertions", oss.str().c_str());
    }

    perf.put("duration", dur);
    perf.put("num_threads", nthreads);
    {
        ostringstream oss;
        if ( mode == 0 ) {
            oss << "DR";
        } else {
            oss << "DRF";
        }
        perf.put("mode", oss.str().c_str());
    }
    //printf("Usage: %s nthreads tablesize insertions mode \n"
    perf.put("tablesize", table_size);

    perf._colwidth = 25;
    cout << perf.showColumns();
    pthread_mutex_destroy(&insert_mutex);


    delete ptr;
    delete thread;
    remove_all(table_size);
    pthread_exit(NULL);
    return 0;

}



