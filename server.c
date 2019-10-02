#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void recv_file(int socket_fd, char *file_name, char *buff)
{
        FILE* received_file = fopen(file_name, "w");
        puts(file_name);
        if (received_file == NULL)
        {
                fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
        }

        printf("buffer is %s\n", buff);
        fputs(buff, received_file);
        fclose(received_file);

}

int my_send_file(int socket_fd, char *FILE_TO_SEND, char *command )
{
	int fd;
	char file_size[256];
        char buff[1000] = {0};
	int sent_bytes = 0;
        struct stat file_stat;

	fd = open(FILE_TO_SEND, O_RDONLY);
        if (fd == -1)
        {
                fprintf(stderr, "Error opening file --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }

        /* Get file stats */
        if (fstat(fd, &file_stat) < 0)
        {
                fprintf(stderr, "Error fstat --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }

        fprintf(stdout, "File Size: \n%d bytes\n", file_stat.st_size);

        int sock_len = sizeof(struct sockaddr_in);
        /* Accepting incoming peers */

        sprintf(file_size, "%d", file_stat.st_size);

        /* Sending file size */

	char command_send[2000] = {0};
		read(fd, buff, 1000);

	sprintf(command_send, "%s %s", command,buff);
		puts(buff);
		write(socket_fd, command_send, 2000);


        /* Sending file data */
		return 0;


}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "port no not provided. Program terminated\n");
		exit(1);
	}

	int sockfd, newsockfd, portno, n;
	char buffer[255];

	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		error("Error opening socket");
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error("binding Failed");
	}

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd < 0)
	{
		error("Error on Accept");
	}

	while (1)
	{
		bzero(buffer, 256);
		n = read(newsockfd, buffer, 255);
		if( strncmp(buffer, "/system", strlen("/system")) == 0)
                {
                        puts("gui file");
                	    char file_name[100] = {0} ;
                        char *content = NULL;
                        char command[100];
                        int file_size ;

                        sscanf(buffer,"%s %s",command, file_name);
                        content = buffer + strlen(command) + strlen(file_name) + 1;
                        //printf("command : %s\n", command);
                        //printf("file_name : %s\n", file_name);
                        recv_file(sockfd, file_name, content);
                }
		if(n < 0)
		{
			error("Error on reading");
		}
		printf("Client: %s\n", buffer);
		bzero(buffer, 255);
		fflush(stdin);
		fgets(buffer, 255, stdin);

		int f = 0;

		printf("print each element in buffer : \n");
		for (f = 0; f< strlen(buffer) ; f++)
		{
			putc(buffer[f], stdout);
		}

		if( strncmp(buffer, "/system", strlen("/system")  ) == 0)
		{
			puts("gui file");
			char file_name[100] = {0} ;
			char command[100];

			sscanf(buffer,"%s %s",command, file_name);
			printf("command is %s , file name is %s\n", command, file_name);
			my_send_file(newsockfd, file_name, buffer);

		}
		else {
			puts("message");
			puts(buffer);
			n = write(newsockfd, buffer, strlen(buffer));
			if (n < 0)
			{
				error("Error on writing");
			}
		}
		int i = strncmp("Bye", buffer, 3);
		if (0 == i)
		break;
	}
	
	close(newsockfd);
	close(sockfd);
	return 0;
}
