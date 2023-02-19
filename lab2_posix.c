#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

typedef struct _bounds
{
    int up,down;
}bounds;

typedef struct _vectors
{
    double *a, *b, *result;
    bounds bounds;
}vectors;

double* init_rand_vector(int size){
    double* vec;
    vec = malloc(size * sizeof(double));
    for(int i = 0;i < size; i++){
        vec[i] = rand()%100;
    }
    return vec;
}

void* add_vec_thr(void* arg){
    vectors* vs = (vectors*)arg;
    for(int i=vs->bounds.up;i<vs->bounds.down;i++){
        vs->result[i] = vs->a[i] + vs->b[i];
    }
    return NULL;
}

void add_vec(double *a, double *b, double *c, int size, int threads_count){
    pthread_t *threads = malloc(threads_count*sizeof(pthread_t));
    vectors *vs = malloc(threads_count*sizeof(vectors));
    for(int i=0;i<threads_count;i++){
        bounds bs;
        bs.up=i*size/threads_count;
        bs.down=(i+1)*size/threads_count;
        vs[i].bounds=bs;
        vs[i].a=a;
        vs[i].b=b;
        vs[i].result=c;
        pthread_create(&threads[i],NULL,add_vec_thr,&vs[i]);
    }
    for(int i=0;i<threads_count;i++){
        pthread_join(threads[i],NULL);
    }
    free(threads);
    free(vs);
}

int main(int argc, char** argv){
    int threads_count=1;
    int size=1000;
    double *a, *b, *c;
    a = init_rand_vector(size);
    b = init_rand_vector(size);
    c = init_rand_vector(size);
    for(threads_count = 1; threads_count < 20; threads_count++){
        printf("Size: %d, Threads: %d\n",size,threads_count);
        struct timespec start,finish;
        clock_gettime(CLOCK_REALTIME,&start);
        add_vec(a, b, c, size, threads_count);
        clock_gettime(CLOCK_REALTIME,&finish);
        double sec,nsec;
        sec=finish.tv_sec-start.tv_sec;
        nsec=finish.tv_nsec-start.tv_nsec;
        double time=sec*1E6+nsec/1E3;
        printf("Addition complete, time = %fmcs\n", time);
    }
    free(a);
    free(b);
    free(c);
}