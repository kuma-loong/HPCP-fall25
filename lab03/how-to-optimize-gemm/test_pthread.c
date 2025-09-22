#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* work(void* arg) {
    int id = *(int*)arg;
    printf("thread %d started\n", id);
    for (int i = 0; i < 5; i++) {
        sleep(1);
        printf("thread %d running...\n", id);
    }
    return NULL;
}

int main() {
    int num_threads = 8;
    pthread_t threads[num_threads];
    int ids[num_threads];

    for (int i = 0; i < num_threads; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, work, &ids[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
