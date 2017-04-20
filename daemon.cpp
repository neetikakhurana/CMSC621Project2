#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define MAXDATASIZE 256
#define MAXPROCESS 5

int main(int argc, char *argv[])
{
	int socketfd, portno , i = 0,j,n;
	int newsockfd[MAXPROCESS];
	int arr[MAXPROCESS];
	int average = 0;
	struct sockaddr_in serveradd, clientaddr;
	char buffer[MAXDATASIZE];

	if(argc < 2)
	{
		fprintf(stderr, "Usage: server <port_number>\n");
		exit(1);
	}

	socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
	  fprintf(stderr, "Error creating a socket\n");

	bzero((char *)&serveradd, sizeof(serveradd));

	portno = atoi(argv[1]);
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = INADDR_ANY;
	serveradd.sin_port = htons(portno);
	
	if(bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
	  {
		fprintf(stderr, "Error binding socket\n");
	  }

	listen(socketfd, 5);

	socklen_t clientlen = sizeof(clientaddr);
	while(i < MAXPROCESS){
		newsockfd[i] = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen);
		if(newsockfd < 0)
		{
			fprintf(stderr, "Error accepting client request\n");
		} 

		bzero(buffer, MAXDATASIZE);

		int n = read(newsockfd[i], buffer, MAXDATASIZE);
		if(n < 0)
		{
			fprintf(stderr, "Error in receiving data\n");
			exit(1);
		}
		arr[i] = *(int *)buffer;
		i++;

	}	
	for(j = 0;j < MAXPROCESS;j++){
		average = average + arr[j];
	}
	average = average / MAXPROCESS;
	*(int *)buffer = average;
	for (j = 0;j < MAXPROCESS;j++){
		n = write(newsockfd[j], buffer, strlen(buffer));
		if(n < 0)
			fprintf(stderr, "Error writing to socket\n");

	}
	//printf("Received Message: %d\n",*(int *)buffer);
	//int data = rand() % 6;
	//char buffer[256];
	//	printf("%d",random);
	for(j = 0;j < MAXPROCESS; j++)
		close(newsockfd[j]);
	close(socketfd);

	return 0;
}
