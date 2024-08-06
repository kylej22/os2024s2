#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handle_sighup(int sig){
    printf("Ouch!\n");
    fflush(stdout);
}

void handle_sigint(int sig){
    printf("Yeah!\n");
    fflush(stdout);
}

int main(int argc, char*argv[]){
    int n = atoi(argv[1]);

    // print first n even numbers
    for(int i = 1; i <= n; i++){
        printf("%d ", 2*i);
        fflush(stdout);
        signal(SIGHUP, handle_sighup);
        signal(SIGINT, handle_sigint);
        sleep(5);
    }

    printf("\n");
    return 0;
}