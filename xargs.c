// Diana Manzano
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <getopt.h>

int stat = EXIT_SUCCESS;

void print(char **args, int tflag){
    pid_t pid;
    pid = fork();
    
    //what pid returns if there's an error
    if (pid == -1) {
        fprintf(stderr, "fork() ERROR");
    }

    //if it is a child
    else if (pid == 0) {
        // printing out
        if (tflag == 1) {
            for (int i = 0; args[i] != NULL; i++){
                fprintf(stderr, "%s ", args[i]);
            }
            fprintf(stderr, "\n");
        }
        //execution taking place
        execvp(args[0], args);
        exit(EXIT_FAILURE);
    }
    //if it's a parent processes
    else {
        //parent should wait for the child to end the process
        if (waitpid(pid, &stat, 0) == -1){
            fprintf(stderr, "waiting ERROR");
        }
    }
}

int main(int argc, char *argv[]){
    //int jflag = 0;
    int nflag = 0;
    int n_read;
    int nlines = 50;
    int tflag = 0;
    int opt;
    size_t len = 0; 
    
    //using optopt for the cases
    //managedby input from the command line
    while ((opt = getopt(argc, argv, "n:t")) != -1){
        switch (opt){
        case 'n':
            nflag = 1;
            if (atoi(optarg))
                nlines = atoi(optarg);
            break;

        case 't':
            tflag = 1;
            break;

        default:
            return -1;

        //case 'j':
        //    jflag =1;
        //    break;
        }
    }
    //args for execvp
    char *args[nlines + 2]; 

    //base case when no command was given
    if (optind >= argc){
        args[0] = "/bin/echo";
    }
   //received command 
    else {
        args[0] = argv[optind];
    }

    while (1){
        int i;
        for (i = 1; i <= nlines; i++){
            args[i] = NULL;

            //if we dont have anything to read
            if ((n_read = getline(&args[i], &len, stdin)) == -1){ 
                break; 
            }
            //delete line
            args[i][strlen(args[i]) - 1] = '\0';
        }

        //extra turned to null
        for (int j = i; j <= nlines + 1; j++){
            args[j] = NULL;
        }

        //calling function that forks and executes
        print(args, tflag);

        if (n_read <= 0) { 
            break;
        }
    }
    return stat;
}