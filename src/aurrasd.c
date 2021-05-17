#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void sigter_handler(int signum) {
    printf("Finishing up current requests. No more requests allowed.\n");
    // codigo de término
}

int main(int argc, char* argv[]) {
    if (signal(SIGTERM, sigter_handler) == SIG_ERR) {
        perror("SIGQUIT");
        return 1;
    }
    // start_server(); {}
}

