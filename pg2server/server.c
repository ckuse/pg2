/* This is the server program for Programming Assignment 2, with members Lauren Bakke, John (Jack) Fox, and Connor Kuse. */ 

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
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

#define SA struct sockaddr 

// port 41018

void usage(const char *progname, int status) {
	fprintf(stderr, "Usage: %s [port number]\n", progname);
	exit(status);
}

void respond_status(int sockfd, int status) {
	if (write(sockfd, &status, sizeof(status)) == -1) {
		fprintf(stderr, "Error executing write: %s\n", strerror(errno)); 
		exit(EXIT_FAILURE);
	} 
}

void dn_func(int sockfd) {
	return; 
}

void up_func(int sockfd) {

	char filename[BUFSIZ] = {0}; 
	int filelength = 0; 
        int filesize = 0;

	if (read(sockfd, &filelength, sizeof(filelength)) < 0) {
		fprintf(stderr, "server error: receiving filename length failed. \n", strerror(errno)); 
	}
	filelength = ntohs(filelength); 

	if (read(sockfd, filename, BUFSIZ) < 0) {
		fprintf(stderr, "server error: receiving filename failed. \n", strerror(errno)); 
	}

	respond_status(sockfd, 1); 

	if (read(sockfd, &filesize, sizeof(filesize)) < 0) {
		fprintf(stderr, "server error: receiving file size failed. \n", strerror(errno)); 
	}

	int fsize = ntohs(filesize); 
	fprintf(stdout, "file size %li \n", fsize); 
	fprintf(stdout, "file name length: %i \n", filelength); 
	fprintf(stdout, "filename: %s \n", filename); 
}

void head_func(int sockfd) {
	// printf("BUFSIZ%d", BUFSIZ);
	char filename[BUFSIZ] = {0};
	int filenameLength = 0;
    long int returnSize = 0;
    char returnMessage[BUFSIZ] = {0};
    char returnBuffer[BUFSIZ] = {0};
    printf("before 1 read in head\n");
    int lenlanyu = 0;
	// receive length of filename from client
	if ((lenlanyu=read(sockfd, &filenameLength, sizeof(filenameLength))) < 0) {
		fprintf(stderr, "Server Error: receiving filename length failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	filenameLength = ntohs(filenameLength); // translate back
	printf("\n*************after 1 read in head: %d\n\n", lenlanyu);

	// receive filename from client
	// if ((lenlanyu=read(sockfd, filename, BUFSIZ)) < 0) {
	if ((lenlanyu=read(sockfd, filename, filenameLength)) < 0) {
		fprintf(stderr, "Server Error: receiving filename failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	filename[filenameLength-1] = 0; // remove extra characters
	printf("\n*************after 2 read in head: %d\n", lenlanyu);

	// check if the file exists
        printf("%s\n", filename);
	FILE* headfile = fopen(filename, "r");

        // file does not exist - send -1 status
	if (!headfile) {
                puts("File does not exist");
                long int status = -1;
                printf("Sending status: %li\n", status);
		if ((lenlanyu=write(sockfd, &status, sizeof(status))) < 0) {
			fprintf(stderr, "Server Error: sending error status failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		printf("\n*************after 3-1 write in head: %d\n", lenlanyu);

                return;
        }
        
        // file exists, continue
        puts("File is good");

	// read from file
	for (int i = 0; i < 10; i++) {
		if (fgets(returnBuffer, BUFSIZ, headfile)) { // not end of file
			returnSize = returnSize + strlen(returnBuffer);
			strcat(returnMessage, returnBuffer);
		} else {
			break;
		}
	}
        if (returnSize) { // file is not empty
                returnMessage[returnSize-1] = 0; // get rid of final newline
        }

        printf("Header: \n%s\n", returnMessage);

	// return size
        printf("Sending header size: %li\n", returnSize);
        if ((lenlanyu=write(sockfd, &returnSize, sizeof(returnSize))) < 0) {
		fprintf(stderr, "Server Error: sending header size failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		
	}
	printf("\n*************after 3-2 write in head: %d\n", lenlanyu);

	// char temp_buffer[BUFSIZ] = {0}; 
	// if (read(sockfd, temp_buffer, BUFSIZ) == -1) {
	// 	fprintf(stderr, "Error reading \n", strerror(errno)); 
	// }


	// return head of file
	// if ((lenlanyu=write(sockfd, returnMessage, BUFSIZ)) < 0) {
	if ((lenlanyu=write(sockfd, returnMessage, strlen(returnMessage)+1)) < 0) {
		fprintf(stderr, "Server Error: sending head of file failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("\n*************after 4 write in head: %d\n", lenlanyu);
	

        fclose(headfile);
}

void rm_func(int sockfd) {
	char buffer[BUFSIZ] = {0};
	int dir_name_len = -1;

	// retrieve length of file name as short
	if (read(sockfd, &dir_name_len, sizeof(dir_name_len)) == -1) {
		fprintf(stderr, "Error getting directory name length from client: %s\n", strerror(errno));	
		exit(EXIT_FAILURE);
	}

	// retrive name of file
	if (read(sockfd, buffer, BUFSIZ) == -1) {
		fprintf(stderr, "Error getting directory name from client: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// check that directory exists
	buffer[strcspn(buffer, "\n")] = 0;
	if( access( buffer, F_OK ) != -1 ) {
		// file exists
		respond_status(sockfd, 1);
	} else {
		// file doesn't exist
		respond_status(sockfd, -1);
	}

	// retrieve user confirmation
	if (read(sockfd, &dir_name_len, sizeof(dir_name_len)) == -1) {
		fprintf(stderr, "Error getting directory name length from client: %s\n", strerror(errno));	
		exit(EXIT_FAILURE);
	}
	if(!dir_name_len) {
		return;
	}
	
	// remove file
	if (remove(buffer) == -1) {
		fprintf(stderr, "Error getting directory name from client: %s\n", strerror(errno));
		respond_status(sockfd, 0);
		return;
	}
	respond_status(sockfd, 1);
	return; 
}

void ls_func(int sockfd) {
	FILE *fp;
	char buffer[BUFSIZ] = {0};
	char current_dir[BUFSIZ] = {0};

	// get current directory
	if (!getcwd(current_dir, sizeof(current_dir))) {
		fprintf(stderr, "Failed to get current directory\n" );
    	exit(EXIT_FAILURE);
	}

	// create string "/bin/ls {current_directory}"
	strcpy(buffer, "/bin/ls -l ");

	// execute ls command
	fp = popen(buffer, "r");
  	if (fp == NULL) {
    	fprintf(stderr, "Failed to execute ls\n" );
    	exit(EXIT_FAILURE);
  	}

	// write result to client
	bzero(buffer, sizeof(buffer));
	bzero(current_dir, sizeof(current_dir));
	// discard first line from ls
	fgets(current_dir,sizeof(current_dir),fp);
	// write each additional line to buffer
	while(fgets(current_dir,sizeof(current_dir),fp)){
		strcat(buffer,current_dir);
	}
	puts(buffer);
	if (write(sockfd, buffer, sizeof(buffer)) == -1) {
		fprintf(stderr, "Error executing write: %s\n", strerror(errno)); 
		exit(EXIT_FAILURE);
	} 

	return; 
}

void mkdir_func(int sockfd) {
	char buffer[BUFSIZ] = {0};
	int dir_name_len = -1;

	// retrieve length of directory name as short
	if (read(sockfd, &dir_name_len, sizeof(dir_name_len)) == -1) {
		fprintf(stderr, "Error getting directory name length from client: %s\n", strerror(errno));	
		exit(EXIT_FAILURE);
	}

	// retrive name of directory
	if (read(sockfd, buffer, BUFSIZ) == -1) {
		fprintf(stderr, "Error getting directory name from client: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	buffer[strcspn(buffer, "\n")] = 0;
	if(mkdir(buffer, 0777) == -1) {
		if (errno == EEXIST) {
			respond_status(sockfd, -2); 
		}
		fprintf(stderr, "Error creating directory with that name: %s\n", strerror(errno)); 
		respond_status(sockfd, -1);
	}

	char temp_buffer[BUFSIZ] = {0}; 
	if (read(sockfd, temp_buffer, BUFSIZ) == -1) {
		fprintf(stderr, "Error reading \n", strerror(errno)); 
	} 


	respond_status(sockfd, 1);
	return; 
}

void rmdir_func(int sockfd) {
	char buffer[BUFSIZ] = {0};
	int dir_name_len = -1;

	// retrieve length of directory name as short
	if (read(sockfd, &dir_name_len, sizeof(dir_name_len)) == -1) {
		fprintf(stderr, "Error getting directory name length from client: %s\n", strerror(errno));	
		exit(EXIT_FAILURE);
	}

	// retrive name of directory
	if (read(sockfd, buffer, BUFSIZ) == -1) {
		fprintf(stderr, "Error getting directory name from client: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("dir: %s\n", buffer); 

	buffer[strcspn(buffer, "\n")] = 0;
	DIR* dir = opendir(buffer);
	if (dir == NULL) {
		printf("buffer: %s \n", buffer); 
		respond_status(sockfd, -1); 
		return; 
	}

    int empty = true; 
	struct dirent *d;
	int n = 0;
  	while ((d = readdir(dir)) != NULL) {
		n++; 
    	if (n > 2) {
			empty = false; 
			break;
		}
	}

	if (empty == false) {
		respond_status(sockfd, -2); 
		return; 
	}

	char temp_buffer[BUFSIZ] = {0}; 
	if (read(sockfd, temp_buffer, BUFSIZ) == -1) {
		fprintf(stderr, "Error reading \n", strerror(errno)); 
	}


	respond_status(sockfd, 1);
	char response_buffer[BUFSIZ] = {0};
	// retrive confirmation
	if (read(sockfd, response_buffer, BUFSIZ) == -1) {
		fprintf(stderr, "Error getting confirmation from client: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	response_buffer[strcspn(response_buffer, "\n")] = 0;
	printf("response buffer: %s \n", response_buffer); 
	if (!(strcmp(response_buffer, "Yes"))) {
		if (rmdir(buffer) == -1) {
			fprintf(stderr, "Error removing : %s\n", strerror(errno));
			respond_status(sockfd, -1); 
		}
		else {
			printf("succcessfully deleted...? \n"); 
			respond_status(sockfd, 1); 
		}
	}



	return; 
}

void cd_func(int sockfd) {
	char buffer[BUFSIZ] = {0};
	int dir_name_len = -1;

	// retrieve length of directory name as short
	if (read(sockfd, &dir_name_len, sizeof(dir_name_len)) == -1) {
		fprintf(stderr, "Error getting directory name length from client: %s\n", strerror(errno));	
		exit(EXIT_FAILURE);
	}

	// retrive name of directory
	if (read(sockfd, buffer, BUFSIZ) == -1) {
		fprintf(stderr, "Error getting directory name from client: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// check that directory exists
	buffer[strcspn(buffer, "\n")] = 0;
	DIR* dir = opendir(buffer);
	if (dir) {
		// directory exists
		closedir(dir);
	} else if (ENOENT == errno) {
		// directory does not exist
		respond_status(sockfd, -2);
		return;
	} else {
		// other error
		respond_status(sockfd, -1);
		return;
	}
	
	// change to relevant directory
	if (chdir(buffer) == -1) {
		fprintf(stderr, "Error getting directory name from client: %s\n", strerror(errno));
		respond_status(sockfd, -1);
		return;
	}
	respond_status(sockfd, 1);
}

void handle_user(int sockfd) {
	bool quit = false;
	char buffer[BUFSIZ] = {0};
	while(!quit) {
		bzero(buffer, BUFSIZ);
		if (read(sockfd, buffer, BUFSIZ) == -1) {
			fprintf(stderr, "Error getting command from client: %s\n", strerror(errno));	
		}
		fprintf(stdout, "Handling User\n");
		fprintf(stdout, "Client Command: %s\n\n", buffer); 
		buffer[strcspn(buffer, "\n")] = 0;
		if (!strcmp(buffer, "DN")) {
			dn_func(sockfd);
		}
		else if (!strcmp(buffer, "UP")) {
			up_func(sockfd);
		}
		else if (!strcmp(buffer, "HEAD")) {
			head_func(sockfd);
		}
		else if (!strcmp(buffer, "RM")) {
			rm_func(sockfd);
		}
		else if (!strcmp(buffer, "LS")) {
			ls_func(sockfd);
		}
		else if (!strcmp(buffer, "MKDIR")) {
			mkdir_func(sockfd);
		}
		else if (!strcmp(buffer, "RMDIR")) {
			rmdir_func(sockfd);
		}
		else if (!strcmp(buffer, "CD")) {
			cd_func(sockfd);
		}
		else if (!strcmp(buffer, "QUIT")) {
			quit = true;
		}
		else {
			fprintf(stderr, "Invalid input: *%s*\n", buffer);
		}
		bzero(buffer, BUFSIZ); 
	}
}



int main(int argc, char *argv[]) {

	/* get port number */
	if (argc != 2) {
		usage(argv[0], EXIT_FAILURE);
	}
	
	int port = atoi(argv[1]); 

	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 
	socklen_t len;
 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr,"Failed to create socket: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
	
	// Binding newly created socket
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        fprintf(stderr,"Socket bind failed: %s\n", strerror(errno)); 
        return EXIT_FAILURE;
    } 
  
    // Listen at specified port
    if ((listen(sockfd, 5)) != 0) { 
        fprintf(stderr, "Listen failed: %s\n", strerror(errno)); 
        return EXIT_FAILURE; 
    }
	 
    len = sizeof(cli); 
  
	// continuously accept and handle user connections
	while (1) {
		// Accept the data packet from client and verification
		printf("Waiting for connections on port %d\n", port);

		connfd = accept(sockfd, (SA*)&cli, &len); 
		if (connfd < 0) { 
			fprintf(stderr, "Server accept failed: %s\n", strerror(errno)); 
			return EXIT_FAILURE; 
		} 

		handle_user(connfd);
	}
    
  
    // Function for chatting between client and server 
    // func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 

	return 0;
}
