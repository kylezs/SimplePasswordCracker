/* Client for 5.2 */
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

#define USERNAME "kzsembery"
#define P 97
#define G 15

int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr);

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

	while (1) {
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


        printf("Sending g^b(mod p): %d\n", 88);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        char buff[2048];
        /* Read initial message */
        n = read(sockfd, buffer, 2047);
        if (n < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
		// strtok(buffer, "\n");

        printf("g^b(mod p)\n");

        // else {
		// 	fprintf(stderr, "Invalid command\n");
		// }

		/* Close to let server know that we've finished sending our message */
		close(sockfd);
	}
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
