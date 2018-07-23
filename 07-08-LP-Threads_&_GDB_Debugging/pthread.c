#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
    {
    double* a;
    double* b;
    double sum;
    int veclen;
} DATA;

#define NUMTHRDS 5
#define VECLEN 20000
DATA data;
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;

void* dotprod(void* arg)
{
    int i, start, end, len;
    long offset;
    double mysum, *x, *y;
    offset = (long)arg;

    len = data.veclen;
    start = offset * len;
    end = start + len;
    x = data.a;
    y = data.b;

    mysum = 0;
    for (i = start; i < end; i++) {
        mysum += (x[i] * y[i]);
    }
/*
Lock a mutex prior to updating the value in the shared
structure, and unlock it upon updating.
*/
    pthread_mutex_lock(&mutexsum);
    data.sum += mysum;
    printf("Thread %ld did %d to %d:  mysum=%f global sum=%f\n", offset, start, end, mysum, data.sum);
    pthread_mutex_unlock(&mutexsum);

    pthread_exit((void*)0);
}
int main(int argc, char* argv[])
{
    long i;
    double *a, *b;
    void* status;
    pthread_attr_t attr;

    /* Assign storage and initialize values */

    a = (double*)malloc(NUMTHRDS * VECLEN * sizeof(double));
    b = (double*)malloc(NUMTHRDS * VECLEN * sizeof(double));

    for (i = 0; i < VECLEN * NUMTHRDS; i++) {
        a[i] = 1;
        b[i] = a[i];
    }

    data.veclen = VECLEN;
    data.a = a;
    data.b = b;
    data.sum = 0;

    pthread_mutex_init(&mutexsum, NULL);

    /* Create threads to perform the dotproduct  */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < NUMTHRDS; i++) {
   /* 
   * Each thread works on a different set of data.
   * The offset is specified by 'i'. The size of
   * the data for each thread is indicated by VECLEN.
   */
        pthread_create(&callThd[i], &attr, dotprod, (void*)i);
    }

    pthread_attr_destroy(&attr);
    /* Wait on the other threads */

    for (i = 0; i < NUMTHRDS; i++) {
        pthread_join(callThd[i], &status);
    }
    /* After joining, print out the results and cleanup */

    printf("Sum =  %f \n", data.sum);
    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
}