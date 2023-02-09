//Diana Manzano
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>

uint64_t lower, upper;
int num = 1;

static int
is_perfect(uint64_t num);

void *helper(void *arg){
    static uint64_t ret[10];
    uint64_t lowerbou, upperbou;

    int divisor =  *(int*)arg;

    int diff = (upper - lower)/num; 

    if(divisor == 1){
        lowerbou = lower;
    }
    else{
        int temp = (divisor-1)*diff; 
        lowerbou = lower + temp + 1; 
    }

    if(divisor == 1){
        upperbou = diff + 1;
    // upper bound
    }else{
        upperbou = lowerbou + ((divisor-1)*diff) + 1; 
    }

    if (upperbou > upper){
        upperbou = upper;
    }

    printf("perfect: t%d searching [%llu , %llu]\n", divisor-1, lowerbou, upperbou);
    int j=0;
    for(uint64_t i = lowerbou; i <= upperbou; i++){
        if(is_perfect(i) != 0){
            ret[j] = i;
            j++;
            printf("%llu\n", i);
        }   
    }
    printf("perfect: t%d finishing\n", divisor-1);
    free(arg);
    return NULL;

}
//function given to check if its perfect
static int
is_perfect(uint64_t num){
    uint64_t i, sum;
    if (num < 2) {
        return 0;
}
for (i = 2, sum = 1; i*i <= num; i++) {
    if (num % i == 0) {
        sum += (i*i == num) ? i : i + num / i;
        }
    }
    return (sum == num);
}

int main(int argc, char *argv[]){
    int opt;

    double sum = 0;
    double add = 1;

/*    // Start measuring time
    clock_t start = clock();
    
    int iterations = 1000*1000*1000;
    for (int i=0; i<iterations; i++) {
        sum += add;
        add /= 2.0;
    }
*/

    if(argc == 1){
        lower = atoi("1");
        upper = atoi("10000");
    }else{
        while ((opt = getopt(argc, argv, "s:e:t:v")) != -1){
        //case conditions
            switch (opt){
                case 's':
                if(optarg != NULL){
                    lower = atoi(optarg);
                }
                break;

                case 'e':
                if(optarg != NULL){
                    lower = atoi(optarg);
                }
                break;
        
                case 't':
                if(optarg != NULL){
                    num= atoi(optarg);
                }
                break;
        
                //case 'v':
                //if(optarg != NULL){
                //}
                //break;

                default:
                break;
            }

        }
    }

//setting default lower and upper bound
    if(lower == 0){
        lower = atoi("1");
    }
    if(upper == 0){
        upper = atoi("10000");
    }

    //check how many threads to run
    pthread_t thread;
    pthread_t arrthread[num];

    if(num == 1){
        int* a = malloc(sizeof(int));
        *a = 1;
        pthread_create(&thread, NULL, helper, a);

        pthread_join(thread, NULL);
    }
    else{  
        for(int i=1; i<=num; i++){
            int* a = malloc(sizeof(int));
            *a = i;
            pthread_create(&arrthread[i], NULL, helper, a);
        }

        for(int i=1; i<=num; i++){
            pthread_join(arrthread[i], NULL);
        }

    }

/*  clock_t end = clock();
    double elapsed;
    elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;        
    printf("Time measured: %.3f seconds.\n", elapsed);
*/
    return 0;
}
