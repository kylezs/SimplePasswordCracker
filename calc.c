#include<unistd.h>
#include<sys/wait.h>
// #include<sys/prctl.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#define P 97
#define G 15

int main(int argc, char** argv)
{
  pid_t pid = 0;
  int inpipefd[2];
  int outpipefd[2];
  char buf[256];
  char msg[256];
  int status;

  pipe(inpipefd);
  pipe(outpipefd);
  pid = fork();
  if (pid == 0)
  {
    // Child
    dup2(outpipefd[0], STDIN_FILENO);
    dup2(inpipefd[1], STDOUT_FILENO);
    dup2(inpipefd[1], STDERR_FILENO);

    //ask kernel to deliver SIGTERM in case the parent dies
    // prctl(PR_SET_PDEATHSIG, SIGTERM);


    //replace tee with your process
    execl("/usr/bin/dc", "dc", (char *) NULL);
    // Nothing below this line should be executed by child process. If so,
    // it means that the execl function wasn't successfull, so lets exit:
    exit(1);
  }
  // The code below will be executed only by parent. You can write and read
  // from the child using pipefd descriptors, and you can send signals to
  // the process using its pid by kill() function. If the child process will
  // exit unexpectedly, the parent process will obtain SIGCHLD signal that
  // can be handled (e.g. you can respawn the child process).

  //close unused pipe ends
  close(outpipefd[0]);
  close(inpipefd[1]);

  // Now, you can write to outpipefd[1] and read from inpipefd[0] :
  if (pid > 0)
  {
    printf("Doing maths\n");
    // g^b (mod p) = > g\nb ^ p
    // strcpy(msg, "2\n16^p\n9 % p\n");
    sprintf(msg, "%d\n%d ^ p\n%d %% p\n", 2, G, P);
    printf("%d\n%d ^ p\n%d\n", 2, G, P);
    write(outpipefd[1], msg, strlen(msg));
    sleep(1);
    read(inpipefd[0], buf, 256);
    int len_buf = strlen(buf);
    char num[3];
    strncpy(num, &buf[len_buf-3], 2);
    int ans = atoi(num);
    printf("Received answer: %s, strlen buf: %lu\n", buf, strlen(buf));

    printf("\nReceived answer, num: %d\n", ans);
  }

  kill(pid, SIGKILL); //send SIGKILL signal to the child process
  waitpid(pid, &status, 0);
}