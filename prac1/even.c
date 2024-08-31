#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handle_sighup(int signum) {
    printf("Ouch!\n");
    fflush(stdout); // Ensure the message is printed immediately
}

void handle_sigint(int signum) {
    printf("Yeah!\n");
    fflush(stdout); 
}

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);

    // Set up signal handlers
    signal(SIGHUP, handle_sighup);
    signal(SIGINT, handle_sigint);

    for (int i = 0; i < n; i++) {
        printf("%d\n", i * 2);
        sleep(5); // Slow down the program to allow for signal testing
    }

    return 0;
}
