//Diana Manzano 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <getopt.h>

//declaring drawers
typedef struct drawer {
    int number;
    pthread_mutex_t lock;
} drawer_t;
static drawer_t drawers[100];

//declaring prisoners
typedef struct prisoner {
    int number;
    pthread_t thread;
} prisoner_t;
static prisoner_t prisoners[100];

//unlocking mutex
static void unlock(pthread_mutex_t *mutex, const char *func){
    int rec;
    rec = pthread_mutex_unlock(mutex);
    if (rec) {
        char *error_unlock= "ERROR! unlocking mutex\n ";
        pthread_exit(error_unlock);
    }
}
//locking mutex
static void lock(pthread_mutex_t *mutex, const char *func){
    int rec;
    rec = pthread_mutex_lock(mutex);
    if (rec) {
        char *error_lock= "ERROR! locking mutex\n";
        pthread_exit(error_lock);
    }
}
//initializing prisioners
//starting amount should be 100 prisoners
static void init_prisoners(void){
    for (int i = 0; i < 100; i++) {
        prisoners[i].number = i;
    }
}
//initializing drawers
static void init_drawers(void){
    int rec;
    for (int i = 0; i < 100; i++) { 
        int rnum, solo;
        do{
            rnum = rand() % 100;
            solo = 1;
            for (int x = 0; x < i; x++) {
            if (rnum == drawers[x].number) {
            solo = 0;
            break;
            }
        }
    } 
        while (! solo);
        drawers[i].number = rnum;
        rec = pthread_mutex_init(&drawers[i].lock, NULL);
        if (rec){
            fprintf(stderr, "ERROR- couldn't create mutex\n");
            exit(EXIT_FAILURE);
        }
    }
}

static pthread_mutex_t global_drawer = PTHREAD_MUTEX_INITIALIZER;


// random drawers using a lock for each
static void *random_drawer(void *arg){
    char *not_match = "-1";
    char *match = "0";
    void *result  = not_match;
    prisoner_t *prisoner = (prisoner_t *) arg;

    for (int i = 0; i < 50; i++) {
        int num = rand()%100;
        lock(&drawers[num].lock, __func__);
        if (drawers[num].number == prisoner->number) {
            result = match;
        }
        unlock(&drawers[num].lock, __func__);
        if (result == not_match) {
        break; 
    }
}
    return result;
}
//global lock w/ random drawer while just going to random 
static void *random_global(void *arg){
    char *not_match = "-1";
    char *match = "0";
    void *result = not_match;
    prisoner_t *prisoner = (prisoner_t *) arg;
    lock(&global_drawer, __func__);

    for (int i = 0; i < 50; i++) {
        int num = rand()%100;
        if (drawers[num].number == prisoner->number) {
            result = match;
            break; 
        }
    }
    unlock(&global_drawer, __func__);
    return result;
}

//global lock w/ a sequence drawer 
//following a path 
static void *strategy_global(void *arg){
    char *not_match = "-1";
    char *match = "0";
    void *result = not_match;
    prisoner_t *prisoner = (prisoner_t *) arg;
    int number = prisoner->number;

    lock(&global_drawer, __func__);
    for (int i = 0; i < 50; i++) {
        if (drawers[number].number == prisoner->number) {
            result = match;
            break;
        }
        number = drawers[number].number;
    }

    unlock(&global_drawer, __func__);
    return result;
}

//individual lock w/ sequence of drawe
//following a path
static void *strategy_drawer(void *arg){
    char *not_match = "-1";
    char *match = "0";
    void *result = not_match;
    prisoner_t *prisoner = (prisoner_t *) arg;
    int number = prisoner->number;
 
    for (int i=0; i < 50; i++){
        lock(&drawers[number].lock, __func__);
        if (drawers[number].number == prisoner->number) {
            result = match;
        }

        unlock(&drawers[number].lock, __func__);
        if (result == match) {
            break; 
        }
        number = drawers[number].number;
    }
    return result;
}

//game itself
static int round(void* (*proc)(void*)){
    char *not_match = "-1";
    char *match = "0";
    init_prisoners();
    init_drawers();
    int rec;
    int equal = 0;

    for (int i = 0; i < 100; i++) {
        rec = pthread_create(&prisoners[i].thread, NULL, proc, &prisoners[i]);
        if (rec) {
            fprintf(stderr, "ERROR, creating thread\n" );
        return -1; 
        }
    }

    for (int i = 0; i < 100; i++) {
        if (prisoners[i].thread) {
            void * ret;
            rec = pthread_join(prisoners[i].thread, &ret);
            if (rec) {
                fprintf(stderr, "ERROR, joining thread\n");
            }
            if (ret == match) {
                equal++; 
            }
        }
    }
    return equal == 100;
}

//going through all the runs and calculating number of wins 
static int runs(int games, void*(*proc)(void *)){
    int wins, i = 0;
    for (i = 0; i < games; i++) {
        if (round(proc)) {
            wins++; 
        }
    }
    return wins;
}

//measuring run times and percentage
static void timeit(char *method, int games, void*(*proc)(void *)){
    int wins;
    double secs;

    clock_t start, finish;
    start = clock();
    wins = runs(games, proc);
    finish = clock();

    secs = (double)(finish - start) / CLOCKS_PER_SEC * 100;
    printf("%s ___ %d/%d wins = %.3f%% ___ %.3f ms\n",  
    method, wins, games, wins * 100.0/ games, secs);
}

int main(int argc, char *argv[]){
    //default number of games 
    int games = 100;
    int opt;
    while ((opt = getopt(argc, argv, "n:s")) >= 0) {
        switch (opt) {
        //setting number generator with a non-static value
        case 's':
            srand(time(NULL));
            break;
        //changing the amount of games playing
        case 'n':
            games = atoi(optarg);
            if (games < 1) {
                fprintf(stderr, "ERROR - input must be a natural number\n");
                exit(EXIT_FAILURE);
            }
            break;

        default:
            fprintf(stderr, "ERROR in default\n");
            exit(EXIT_FAILURE);
        }
    }
    if (argc != optind) {
        fprintf(stderr, "ERROR\nUsage: [-n #games] [-s]\n");
        exit(EXIT_FAILURE);
    }
    //printing probabilities and execution times
    timeit("method random_global", games, random_global);
    timeit("method random_drawer", games, random_drawer);
    timeit("method strategy_global", games, strategy_global);
    timeit("method strategy_drawer", games, strategy_drawer);
    return 0;
}