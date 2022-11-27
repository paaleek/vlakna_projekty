#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <malloc.h>

bool isPerfectSquare(int x) {
    int s = sqrt(x);
    return (s * s == x);
}
bool isFibonacci(int n) {
    return isPerfectSquare(5 * n * n + 4) || isPerfectSquare(5 * n * n -4);
}

void printIfFibonachi(int n) {
    if (isFibonacci(n)) {
        printf("%d je Fibbonachi cislo\n", n);
    }
}

typedef struct arguments {
    const int a;
    const int b;
    const int buffer_max_capacity;
    int* buffer;
    int buffer_index;
    pthread_mutex_t* mutex;
    pthread_cond_t* full;
    pthread_cond_t* empty;
} ARGUMENTS;

void* produce(void* args) {
    ARGUMENTS* data = (ARGUMENTS*) args;

    for (int i = data->a; i < data->b; i++) {
        pthread_mutex_lock(data->mutex);
        while (data->buffer_index >= data->buffer_max_capacity) {
            pthread_cond_wait(data->full, data->mutex);
        }
        data->buffer[data->buffer_index] = i;
        data->buffer_index++;
        pthread_cond_signal(data->empty);
        pthread_mutex_unlock(data->mutex);
    }
    return NULL;
}

void* consume(void* args) {
    ARGUMENTS* data = (ARGUMENTS*) args;

    for (int i = data->a; i < data->b; i++) {
        pthread_mutex_lock(data->mutex);
        while (data->buffer_index == 0) {
            pthread_cond_wait(data->empty, data->mutex);
        }
        int value = data->buffer[data->buffer_index - 1];
        data->buffer_index--;
        if (data->buffer_index == 0) {
            pthread_cond_signal(data->full);
        }
        pthread_mutex_unlock(data->mutex);

        printIfFibonachi(value);
    }
    return NULL;
}


int main() {
//    for (int i = 1; i <= 100; i++) {
//        if (isFibonacci(i))
//            printf("%d is a Fibonacci Number \n", i);
//    }

    int a = 4;
    int b = 1000;
    int c = 3;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t full;
    pthread_cond_init(&full, NULL);

    pthread_cond_t empty;
    pthread_cond_init(&empty, NULL);

    ARGUMENTS args = {a, b, c , malloc(c * sizeof(int)), 0, &mutex, &full, &empty};

    pthread_t generator;
    pthread_create(&generator, NULL, produce, &args);

    pthread_t checker;
    pthread_create(&checker, NULL, consume, &args);

    pthread_join(generator, NULL);
    pthread_join(checker, NULL);

    free(args.buffer);
    args.buffer = NULL;

    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);


    return 0;
}
