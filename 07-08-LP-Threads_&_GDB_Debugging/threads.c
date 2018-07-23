#include <stdio.h>
#include <threads.h>
#include <stdlib.h>

typedef struct
    {
    double* a;
    double* b;
    double sum;
    int veclen;
} DOTDATA;

#define NUMTHRDS 5
#define VECLEN 20000

DOTDATA dotstr;
thrd_t callThd[NUMTHRDS];
mtx_t mutexsum;

int dotprod(void* arg)
{
    /* Define and use local variables for convenience */

    int i, start, end, len;
    long offset;
    double mysum, *x, *y;
    offset = (long)arg;

    len = dotstr.veclen;
    start = offset * len;
    end = start + len;
    x = dotstr.a;
    y = dotstr.b;

    mysum = 0;
    for (i = start; i < end; i++) {
        mysum += (x[i] * y[i]);
    }

    /*
Lock a mutex prior to updating the value in the shared
structure, and unlock it upon updating.
*/
    mtx_lock(&mutexsum);
    dotstr.sum += mysum;
    printf("Thread %ld did %d to %d:  mysum=%f global sum=%f\n", offset, start, end, mysum, dotstr.sum);
    mtx_unlock(&mutexsum);

    thrd_exit(0);
}

int main(int argc, char* argv[])
{
    long i;
    double *a, *b;
    int status;

    /* Assign storage and initialize values */

    a = (double*)malloc(NUMTHRDS * VECLEN * sizeof(double));
    b = (double*)malloc(NUMTHRDS * VECLEN * sizeof(double));

    for (i = 0; i < VECLEN * NUMTHRDS; i++) {
        a[i] = 1;
        b[i] = a[i];
    }

    dotstr.veclen = VECLEN;
    dotstr.a = a;
    dotstr.b = b;
    dotstr.sum = 0;

    mtx_init(&mutexsum, mtx_plain);

    for (i = 0; i < NUMTHRDS; i++) {
        /* Each thread works on a different set of data.
   * The offset is specified by 'i'. The size of
   * the data for each thread is indicated by VECLEN.
   */
        thrd_create(&callThd[i], dotprod, (void*)i);
    }
    /* Wait on the other threads */

    for (i = 0; i < NUMTHRDS; i++) {
        thrd_join(callThd[i], &status);
    }
    /* After joining, print out the results and cleanup */

    printf("Sum =  %f \n", dotstr.sum);
    free(a);
    free(b);
    mtx_destroy(&mutexsum);
    thrd_exit(0);
}