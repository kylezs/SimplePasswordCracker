/*
* Completes a diffie hellman key exchange using the first byte of the hash
* of itself as "a"
* Much of the pipe and exec code was sourced from: https://stackoverflow.com/a/6172578
* Author: Kyle Zsembery
*/
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

#define USERNAME "kzsembery\n"
#define DH_PATH "dh.c"
#define P 97
#define G 15

int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr);

int first_byte();
int calc_gbmodp(int b);
int calc_gbamodp(int gamodp, int first_byte_int);

int main(int argc, char* argv[]) {
	struct sockaddr_in serv_addr;
	char* server;
	int port;
	int sockfd;
	char buffer[256];
    int n;

	if (argc < 3) {
		fprintf(stderr, "usage: %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	port = atoi(argv[2]);
	server = argv[1];

	/* Make connection */
	sockfd = setup_client_socket(port, server, &serv_addr);
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) <
		0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}

    printf("Sending username: %s\n", USERNAME);
    n = write(sockfd, USERNAME, strlen(USERNAME));
    if (n < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Send gbmodp
    printf("Calcing hash\n");
    int first_byte_int = first_byte();
    int first_send = calc_gbmodp(first_byte_int);
    printf("first send: %d, first byte: %d\n", first_send, first_byte_int);
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "%d\n", first_send);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    memset(buffer, '\0', sizeof(buffer));
    /* Read initial message */
    n = read(sockfd, buffer, 2047);
    if (n < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }


    // This is the iteration number
    int serv_response_num = atoi(buffer);

    printf("From server g^((b)a) modp(int): %d\n", serv_response_num);
    int second_send = calc_gbamodp(serv_response_num, first_byte_int);

    memset(buffer, '\0', sizeof(buffer));

    sprintf(buffer, "%d\n", second_send);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    memset(buffer, '\0', sizeof(buffer));

    /* Read initial message */
    n = read(sockfd, buffer, 2047);
    if (n < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("From server, status: %s\n", buffer);

	/* Close to let server know that we've finished sending our message */
	close(sockfd);

}

/* Create and return a socket bound to the given port and server */
int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr) {
	int sockfd;
	struct hostent* server;

	server = gethostbyname(server_name);
	if (!server) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}
	bzero((char*)serv_addr, sizeof(serv_addr));
	serv_addr->sin_family = AF_INET;
	bcopy(server->h_addr_list[0], (char*)&serv_addr->sin_addr.s_addr,
		  server->h_length);
	serv_addr->sin_port = htons(port);

	/* Create socket */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

int first_byte() {
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
    int first_byte_int = -1;
    if (pid > 0)
    {
      sprintf(command, "sha256 %s\n", DH_PATH);
      write(outpipefd[1], command, strlen(command));
      sleep(4);
      read(inpipefd[0], buf, 256);
      char* hash = strstr(buf, "=");
      hash += 2;
      printf("Full hash: %s\n", hash);
      printf("End hash\n");
      char first_byte[2];
      strncpy(first_byte, hash, 2);
      first_byte_int = (int)strtol(first_byte, NULL, 16);
    }

    kill(pid, SIGKILL); //send SIGKILL signal to the child process
    waitpid(pid, &status, 0);
    printf("First byte int just before return: %d\n", first_byte_int);
    return first_byte_int;
}

int calc_gbamodp(int gamodp, int first_byte_int) {
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
    int ans = -1;
    if (pid > 0)
    {
        // g^b (mod p) = > g\nb ^ p
        // strcpy(msg, "2\n16^p\n9 % p\n");
        sprintf(msg, "%d\n%d ^ p\n%d %% p\n", gamodp, first_byte_int, P);
        write(outpipefd[1], msg, strlen(msg));
        sleep(1);
        memset(buf, '\0', 256);
        read(inpipefd[0], buf, 256);
        printf("The buf next line:\n%s\n", buf);
        int len_buf = strlen(buf);
        char num[3];
        strncpy(num, &buf[len_buf-3], 3);
        ans = atoi(num);
    }

    kill(pid, SIGKILL); //send SIGKILL signal to the child process
    waitpid(pid, &status, 0);
    printf("2nd ans just before return %d\n", ans);
    return ans;
}

int calc_gbmodp(int b) {
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
    int ans = -1;
    if (pid > 0)
    {
        // g^b (mod p) = > g\nb ^ p
        // strcpy(msg, "2\n16^p\n9 % p\n");
        sprintf(msg, "%d\n%d ^ p\n%d %% p\n", G, b, P);
        write(outpipefd[1], msg, strlen(msg));
        sleep(1);
        memset(buf, '\0', 256);
        read(inpipefd[0], buf, 256);
        printf("The buf next line:\n%s\n", buf);
        int len_buf = strlen(buf);
        char num[3];
        strncpy(num, &buf[len_buf-3], 3);
        ans = atoi(num);
    }

    kill(pid, SIGKILL); //send SIGKILL signal to the child process
    waitpid(pid, &status, 0);
    printf("ans just before return %d\n", ans);
    return ans;
}
