#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct argumnets {
    const double priprava_a;
    const double priprava_b;
    const double roznos_a;
    const double roznos_b;
    const int pocet_jedal;
    int pocet_spracovanych_jedal;
    const int buffer_capacity;
    int buffer_index;
    int* buffer;
    pthread_mutex_t* mutex;
    pthread_cond_t* empty;
    pthread_cond_t* full;
} ARGUMNETS;

void* produce(void* args) {
    ARGUMNETS* data = (ARGUMNETS*)args;

    for (int i = 0; i < data->pocet_jedal; i++) {
        double r = rand() / (double)RAND_MAX;
        r = data->priprava_a + r*(data->priprava_b - data->priprava_a);
        usleep(r*10000);

        pthread_mutex_lock(data->mutex);
        while (data->buffer_index >= data->buffer_capacity) {
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
    ARGUMNETS* data = (ARGUMNETS*)args;

    while (data->pocet_spracovanych_jedal < data->pocet_jedal) {

        if(data->pocet_spracovanych_jedal >= data->pocet_jedal) {
            printf("Vsetky jedla spracovane odpajam sa\n");
            break;
        }

        pthread_mutex_lock(data->mutex);
        while(data->buffer_index == 0) {
            pthread_cond_wait(data->empty, data->mutex);
        }

        int jedlo = data->buffer[data->buffer_index - 1];
        data->buffer_index--;
        data->pocet_spracovanych_jedal++;

        pthread_cond_signal(data->full);
        pthread_mutex_unlock(data->mutex);

        double r = rand() / (double)RAND_MAX;
        r = data->roznos_a + r*(data->roznos_b - data->roznos_a);
        usleep(r*10000);
        printf("Spracovane jedlo %d\n", jedlo);


    }
    return NULL;
}

int main() {
    const double priprava_a = 0.5;
    const double priprava_b = 1.5;
    const double roznos_a = 2;
    const double roznos_b = 5;
    const int pocet_jedal = 100;
    int pocet_spracovanych_jedal = 0;
    const int buffer_capacity = 3;
    int buffer_index = 0;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t full;
    pthread_cond_init(&full, NULL);

    pthread_cond_t empty;
    pthread_cond_init(&empty, NULL);

    ARGUMNETS args = {priprava_a, priprava_b, roznos_a, roznos_b, pocet_jedal, pocet_spracovanych_jedal,
                      buffer_capacity, buffer_index, malloc(buffer_capacity * sizeof(int)), &mutex, &empty, &full};



    pthread_t casnik1;
    pthread_t casnik2;

    pthread_create(&casnik1, NULL, consume, &args);
    pthread_create(&casnik2, NULL, consume, &args);

    produce(&args);

    pthread_join(casnik1, NULL);
    pthread_join(casnik2, NULL);

    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    pthread_mutex_destroy(&mutex);

    free(args.buffer);
    args.buffer = NULL;
    return 0;
}
