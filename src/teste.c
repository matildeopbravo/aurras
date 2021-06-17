#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int   pipe_gr[2];
  int   max    = 1024;
  char* buffer = malloc(sizeof(char) * max);

  pipe(pipe_gr);

  bool li_algo = false;

  int status;
  // int resul = fcntl(pipe_gr[0], F_SETFL, O_NONBLOCK);

  if (fork() == 0) {

    while (!li_algo) {

      // int resul = fcntl(pipe_gr[0], F_SETFL, O_NONBLOCK);
      // printf("%d\n", getpid);
      //  if (fork() == 0) {

      //    int resul = fcntl(pipe_gr[0], F_SETFL, O_NONBLOCK);
      //    exit(resul);
      //  };

      // wait(&status);
      printf("wtf\n");

      read(pipe_gr[0], &buffer, max);
      printf("printei: %s\n", buffer);
      // if (read(pipe_gr[0], &buffer, max) > 0) printf("printei: %s\n",
      // buffer);
    }

    printf("o filho acabou\n");
    exit(0);
  }

  sleep(1);
  char string[4] = "Olaa";
  write(pipe_gr[1], string, sizeof(string));
  wait(&status);
  printf("Acabei %d -> %s\n", status, buffer);
  free(buffer);
}
