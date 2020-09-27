/* This is the client program for Programming Project 2 with members Lauren Bakke, John (Jack) Fox, and Connor Kuse. */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define SA struct sockaddr

void usage(const char *progname, int status) {
	fprintf(stderr, "Usage: %s [host name] [port number]\n", progname);
	exit(status);
}

void down_func(int sockfd) {
	puts("bruh");
}

void up_func(int sockfd) {
	char filename[BUFSIZ] = {0};
	int filenameLength = 0;

	fprintf(stdout, "Input filename:\n> ");
	fgets(filename, BUFSIZ, stdin);
	filenameLength = strlen(filename)-1;
        filename[filenameLength] = 0; // get rid of newline

	fprintf(stdout, "filename length %i\n", filenameLength);
	filenameLength = htons(filenameLength);
	if (write(sockfd, &filenameLength, sizeof(filenameLength)) < 0) {
		fprintf(stderr, "Client error: sending filename length failed: %s \n", strerror(errno));
	}

	fprintf(stdout, "filename %s \n", filename);
	if (write(sockfd, filename, BUFSIZ) < 0) {
		fprintf(stderr, "Client error: sending filename failed: %s \n", strerror(errno));
	}

	int response_message;
	// read confirmation
	if (read(sockfd, &response_message, sizeof(response_message)) == -1) {
		fprintf(stderr, "Error reading client confirmation: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (response_message != 1) {
		fprintf(stderr, "Did not receive confirmation from server: %s. \n");
		return;
	}

	filename[strcspn(filename, "\n")] = 0;
	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {
		printf("File not found! \n");
		return;
	}

	fseek(fp, 0, SEEK_END);

	int len = 0;
	len = ftell(fp);

	fprintf(stdout, "file size - %i \n", len);
	len = htons(len);
	// send back file size
	if (write(sockfd, &len, sizeof(len)) < 0) {
		fprintf(stderr, "Error sending file size %s. \n", strerror(errno));
	}
}

void head_func(int sockfd) {
	// printf("BUFSIZ%d", BUFSIZ);
	char filename[BUFSIZ] = {0};
	int filenameLength = 0;
        long int headerSize = 0;
        char header[BUFSIZ] = {0};
        int lenlanyu = 0;
        printf("START: HS=%li\n", headerSize);
	printf("Input filename:\n> ");
	fgets(filename, BUFSIZ, stdin);

	// send filename length
	filenameLength = htons(strlen(filename));
	if ((lenlanyu=write(sockfd, &filenameLength, sizeof(filenameLength))) < 0) {
		fprintf(stderr, "Client Error: sending filename length failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("\n**************after 1st write in head: %d\n\n", lenlanyu);
	// send filename
	// if ((lenlanyu=write(sockfd, filename, BUFSIZ)) < 0) {
	if ((lenlanyu=write(sockfd, filename, strlen(filename))) < 0) {
		fprintf(stderr, "Client Error: sending filename failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("\n**************after 2 write in head: %d\n\n", lenlanyu);
	// receive header size
	if ((lenlanyu=read(sockfd, &headerSize, sizeof(headerSize))) < 0) {
		fprintf(stderr, "Client Error: receiving header size failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("\n**************after 3 read in head: %d\n\n", lenlanyu);
        printf("Header Size: %li\n", headerSize);

        // check header size for status
        if (headerSize < 0) {
		printf("File not found on server.\n");
		return;
	} else {
                headerSize = ntohl(headerSize);
                puts("test");
        }

	// receive header
	// if ((lenlanyu=read(sockfd, header, BUFSIZ)) < 0) {
	if ((lenlanyu=read(sockfd, header, headerSize)) < 0) {
		fprintf(stderr, "Client Error: receiving header failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("\n**************after 4 read in head: %d\n\n", lenlanyu);

	// print header
	printf("%s\n", header);
}

void cd_func(int sockfd) {
	char user_message[BUFSIZ] = {0};
	int dir_name_length = 0;
	int response_message;
	printf("Input Directory Name\n >");
	fgets(user_message, BUFSIZ, stdin);
	dir_name_length = strlen(user_message);
	// send directory name length
	if (write(sockfd, &dir_name_length, sizeof(dir_name_length)) == -1) {
		fprintf(stderr, "Error sending directory name length to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	// send directory name
	if (write(sockfd, user_message, BUFSIZ) == -1) {
		fprintf(stderr, "Error sending directory name to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// read confirmation
	if (read(sockfd, &response_message, sizeof(response_message)) == -1) {
		fprintf(stderr, "Error reading client confirmation: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(response_message == -2) {
		printf("The directory does not exist on server\n");
	} else if(response_message == -1) {
		printf("Error in changing directory\n");
	} else if(response_message > 0) {
		printf("Changed current directory\n");
	}
}

void mkdir_func(int sockfd) {
	char user_message[BUFSIZ] = {0};
	int dir_name_length = 0;
	int response_message;
	printf("Input Directory Name\n >");
	fgets(user_message, BUFSIZ, stdin);
	dir_name_length = strlen(user_message);

	// send directory name length
	if (write(sockfd, &dir_name_length, sizeof(dir_name_length)) == -1) {
		fprintf(stderr, "Error sending directory name length to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	// send directory name
	if (write(sockfd, user_message, BUFSIZ) == -1) {
		fprintf(stderr, "Error sending directory name to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// read confirmation
	if (read(sockfd, &response_message, sizeof(response_message)) == -1) {
		fprintf(stderr, "Error reading client confirmation: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (response_message == -1) {
		printf("Error in making directory\n");
	}
	else if (response_message == -2) {
		printf("The directory already exists on the server\n");
	}
	else if (response_message > 0) {
		printf("The directory was successfully made\n");
	}

}

void rmdir_func(int sockfd) {
	char user_message[BUFSIZ] = {0};
	int dir_name_length = 0;
	int response_message;
	printf("Input Directory Name\n >");
	fgets(user_message, BUFSIZ, stdin);
	dir_name_length = strlen(user_message);

	// send directory name length
	if (write(sockfd, &dir_name_length, sizeof(dir_name_length)) == -1) {
		fprintf(stderr, "Error sending directory name length to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	// send directory name
	if (write(sockfd, user_message, BUFSIZ) == -1) {
		fprintf(stderr, "Error sending directory name to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// read confirmation
	if (read(sockfd, &response_message, sizeof(response_message)) == -1) {
		fprintf(stderr, "Error reading client confirmation: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (response_message == -1) {
		printf("The directory does not exist on the server. \n");
	}
	else if (response_message == -2) {
		printf("The directory is not empty. \n");
	}
	else if (response_message > 0) {
		printf("Do you REALLY want to delete the directory?\n");

		char confirmation[BUFSIZ] = {0};
		fgets(confirmation, BUFSIZ, stdin);
		printf("Received: %s \n", confirmation);

		// send confirmation to server
		if (write(sockfd, &confirmation, sizeof(confirmation)) == -1) {
			fprintf(stderr, "Error sending confirmation of rmdir to server: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (!strcmp(confirmation, "No\n")) {
			printf("Delete abandoned by user! \n");
		}
		else {
			// read result of rmdir
			if (read(sockfd, &response_message, sizeof(response_message)) == -1) {
				fprintf(stderr, "Error receiving deletion response: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			if (response_message == -1) {
				printf("Failed to delete directory. \n");
			}
			else {
				printf("Directory deleted. \n");
			}
		}
	}

}

void rm_func(int sockfd) {
	char user_message[BUFSIZ] = {0};
	int dir_name_length = 0;
	int response_message;
	printf("Input File Name\n >");
	fgets(user_message, BUFSIZ, stdin);
	dir_name_length = strlen(user_message);

	// send file name length
	if (write(sockfd, &dir_name_length, sizeof(dir_name_length)) == -1) {
		fprintf(stderr, "Error sending file name length to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	// send file name
	if (write(sockfd, user_message, BUFSIZ) == -1) {
		fprintf(stderr, "Error sending file name to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// read confirmation
	if (read(sockfd, &response_message, sizeof(response_message)) == -1) {
		fprintf(stderr, "Error reading client confirmation: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (response_message < 0) {
		printf("Error: File does not exist\n");
		return;
	}

	// get user confirmation
	printf("Are you sure you want to delete this file? (Yes/No)\n >");
	while (1) {
		fgets(user_message, BUFSIZ, stdin);
		user_message[strcspn(user_message, "\n")] = 0;
		if (!strcmp(user_message,"Yes") || !strcmp(user_message,"No")) {
			break;
		}
		printf("Please input a \"Yes\" or \"No\"\n >");
	}

	if (!strcmp(user_message,"Yes")) {
		dir_name_length = 1;
	} else {
		dir_name_length = 0;
	}
	if (write(sockfd, &dir_name_length, sizeof(dir_name_length)) == -1) {
		fprintf(stderr, "Error rm confirmation to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// read confirmation
	if (read(sockfd, &response_message, sizeof(response_message)) == -1) {
		fprintf(stderr, "Error reading client confirmation: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(response_message) {
		printf("File deleted\n");
	}

}

int main(int argc, char *argv[]) {

	/* get host and port info */

	if (argc != 3) {
		usage(argv[0], EXIT_FAILURE);
	}

	char *hostname = argv[1];
	int port = atoi(argv[2]);

	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Socket creation failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
	bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT

	struct hostent *he;
	if ((he = gethostbyname(hostname)) == NULL) {
		fprintf(stderr, "Error with gethostbyname: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	char *server = "127.0.0.1";
	if (he->h_addrtype == AF_INET) {
		struct in_addr **address_list = (struct in_addr **)he->h_addr_list;
		for (int i = 0; address_list[i] != NULL; i++) {
			server = inet_ntoa(*address_list[i]);
		}
	}

    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_addr.s_addr = inet_addr(server);
    servaddr.sin_port = htons(port);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        fprintf(stderr, "Connection with the server failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
	char user_message[BUFSIZ];
	while (1) {
		bzero(user_message, sizeof(user_message));
		fprintf(stdout, "Input a command: \n> ");
		fgets(user_message, BUFSIZ, stdin);
		fprintf(stdout, "Your user message is: %s\n", user_message);
		if (write(sockfd, user_message, BUFSIZ) == -1) {
			fprintf(stderr, "Error sending command to server: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		user_message[strcspn(user_message, "\n")] = 0;
		if (!strcmp(user_message, "LS")) {
			if (read(sockfd, user_message, BUFSIZ) == -1) {
				fprintf(stderr, "Error getting directory name length from client: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			fprintf(stdout, user_message);
		}
		if (!strcmp(user_message, "DOWN")) {
			down_func(sockfd);
		}
		if (!strcmp(user_message, "UP")) {
			up_func(sockfd);
		}
		if (!strcmp(user_message, "HEAD")) {
			head_func(sockfd);
		}
		if (!strcmp(user_message, "CD")) {
			cd_func(sockfd);
		}
		if (!strcmp(user_message, "MKDIR")) {
			mkdir_func(sockfd);
		}
		if (!strcmp(user_message, "RMDIR")) {
			rmdir_func(sockfd);
		}
		if (!strcmp(user_message, "RM")) {
			rm_func(sockfd);
		}
	}


    // close the socket
    close(sockfd);

    return 0;

}
