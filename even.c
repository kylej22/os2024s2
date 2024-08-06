#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handle_sighup(int sig){
    printf("Ouch!\n");
}

void handle_sigint(int sig){
    printf("Yeah!\n");
}

int main(int argc, char*argv[]){
    int n = atoi(argv[1]);

    signal(SIGHUP, handle_sighup);
    signal(SIGINT, handle_sigint);

    // print first n even numbers
    for(int i = 1; i <= n; i++){
        printf("%d ", 2*i);
        sleep(5);
    }

    printf("\n");
}