#include <stdio.h>
#include <pthread.h>
#include <math.h>

void* work(void* arg) {
    long id = (long)arg;
    double sum = 0;
    for (long i = 0; i < 1e9; i++) {
        sum += sin(i) * cos(i);
    }
    printf("thread %ld done, sum=%f\n", id, sum);
    return NULL;
}

int main() {
    printf("pid=%d\n",(int) getpid());
    int num_threads = 16;   
    pthread_t threads[num_threads];

    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, work, (void*)i);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
