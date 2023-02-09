//Diana Ma
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern char **environ;
static int i = 0; 

// changing adding environment variable
static void add(char *arg){
    if (i > 0) {
        (putenv(arg));{
        perror("adding:....");
        }
    }
    else{
        fprintf(stderr, "%s\n", arg);
    }
}
//removing the  environment variable from the environment of the calling process
static void delete(char *arg){
    if (unsetenv(arg) == -1){
        perror("ERROR"); 
    }
    if (i) {
        fprintf(stderr, "deleting:%s\n", arg);
    }
}

//printing all the environs 
static void print(char *env[]){
    for (int i = 0; env[i]!= NULL; i++) {
        puts(env[i]);
    }
}

// exxecute the command with the given arguments
//not working xsddd
static void exec(char *argv[]){
        for (int i = 0; argv[i]; i++) {
            fprintf(stderr, "arg[%d]'%s'\n", i, argv[i]);
        }
    execvp(argv[0], argv);
    perror("ERROR");
    EXIT_FAILURE;
}

//searches for the argument passed
int search(const char *arg){
    int s_bar = strchr(arg, '=') != NULL;
    return s_bar;
} 

int main(int argc, char *argv[]){
    char *nargv[argc];
    int opt;
    int nargc = 0;

    //uSing optopt for the cases
    //different cases, managedby inout from the command line
    //and with its respective defaulgt
    while ((opt = getopt(argc, argv, "vu:")) != -1) {
        switch (opt) {

        //for v should trace the pathe but idk how to do it
        case 'v':
            fprintf(stderr, "v has been chosen\n");
            i = 1;
            break;

        case 'u':
            fprintf(stderr, "u has been chosen\n");
            delete(optarg);
            fprintf(stderr, "%s has been deleted\n\n", optarg);
            break; 
   
        default: 
            fprintf(stderr, "returning default\n");
            fprintf(stderr,  "Usage: chose [-v] or [-u name] "
                            "[NAME=VALUE]... COMMAND[ARG...]]\n");
            exit(EXIT_FAILURE);
            }
        }
        
        //adding the sequence pair to th env
        for (int i = optind; i < argc; i++) {
        if (search(argv[i])) {
            add(argv[i]);
            }
            else{
                nargv[nargc++] = argv[i];
            }
        }

        //no arguments passed, prints the current environment
        nargv[nargc] = NULL;
        if (nargv[0] == NULL) {
            print(environ);
        }

        //calling funtion that  executes
        //the command with the given arguments
        else{
            exec(nargv);
            }
            return EXIT_SUCCESS;
}