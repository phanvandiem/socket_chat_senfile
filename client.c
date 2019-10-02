#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

void error(const char *msg)
{
	perror(msg);
	exit(0);
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
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[2000];
	if (argc < 3)
	{
		fprintf(stderr, "usage %s hostname port\n",argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		error("ERROR opening socket");
	}
	server = gethostbyname(argv[1]);
	if (NULL == server)
	{
		fprintf(stderr, "Error, no such host");
	}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *) server -> h_addr, (char *) & serv_addr.sin_addr, server -> h_length);
		serv_addr.sin_port = htons(portno);
		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
			error("Connection Failed");
		}
		while(1)
		{
			bzero(buffer, 2000);
			fgets(buffer, 2000, stdin);
			if( strncmp(buffer, "/system", strlen("/system")  ) == 0)
                	{
                        	puts("gui file");
                        	char file_name[100] = {0} ;
                        	char command[100];

                        	sscanf(buffer,"%s %s",command, file_name);
                        	printf("command is %s , file name is %s\n", command, file_name);
                        	my_send_file(sockfd, file_name, buffer);

                	}
			else
			{
				n = write(sockfd, buffer, strlen(buffer));
				if (n < 0)
				{
					error("Error on writing");
				}
			}
			bzero(buffer, 2000);
			n = read(sockfd, buffer, 2000);
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

			if (n < 0)
			{
				error("Error on reading");
			}
			printf("Server: %s", buffer);
			int i = strncmp("Bye", buffer, 3);
			if (0 == i)
				break;

		}

	close(sockfd);
	return 0;
}
