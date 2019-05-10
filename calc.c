#include<unistd.h>
#include<sys/wait.h>
// #include<sys/prctl.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#define P 97
#define G 15
#define DH_PATH "dh.c"

int main(int argc, char** argv)
{
  pid_t pid = 0;
  int inpipefd[2];
  int outpipefd[2];
  char buf[256];
  int status;
  char command[100];

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

    execl("/usr/bin/openssl", "openssl", (char *) NULL);
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
    printf("calcing hash\n");
    sprintf(command, "sha256 %s\n", DH_PATH);
    write(outpipefd[1], command, strlen(command));
    sleep(4);
    read(inpipefd[0], buf, 256);
    char* hash = strstr(buf, "=");
    hash += 3;
    printf("The hash is %s\n", hash);
    char first_byte = hash[0];
    int first_byte_int = (int)strtol(&first_byte, NULL, 16);
    printf("First byte int: %d\n", first_byte_int);
  }

  kill(pid, SIGKILL); //send SIGKILL signal to the child process
  waitpid(pid, &status, 0);
}