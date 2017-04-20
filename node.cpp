#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#define MAXDATASIZE 256

int main(int argc, char *argv[])
{

	if(argc < 4)
	{
		fprintf(stderr, "Usage client <hostname> <port number> <random number> <client>\n");
		exit(1);
	}

	struct hostent *server;
	int portno,data;
	portno = atoi(argv[2]);
	int pid=atoi(argv[4]);
	server = gethostbyname(argv[1]);
	if(server == NULL)
	{
		fprintf(stderr, "No such host exists\n");
		exit(1);
	}

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port = htons(portno);	


	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0)
	{
		fprintf(stderr, "Socket not formed \n");
		exit(0);
	}

	if(connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "Error connecting to daemon\n");
	}

	data = atoi(argv[3]);
	char buffer[MAXDATASIZE];
	bzero(buffer,MAXDATASIZE);

	*(int *)buffer = data;
	printf("%d : Initial clock of client: %d\n",pid, data);

	int n=read(socketfd,buffer,MAXDATASIZE);
	if(n<0){
		fprintf(stderr, "Error reading daemon's initial clock\n");
		exit(1);
	}
	printf("%d : Daemon's clock %d\n", pid,*(int *)buffer);
		
	int diff=data-*(int *)buffer;
	printf("%d : Sending clock difference %d to daemon\n",pid,diff);
	bzero(buffer,MAXDATASIZE);
	*(int *)buffer=diff;
	n = write(socketfd, buffer, MAXDATASIZE);
	if(n < 0)
	{
		fprintf(stderr, "%d : Error writing clock diff to daemon\n",pid);
		exit(1);	
	}

	//bzero(buffer, MAXDATASIZE);
	n = read(socketfd, buffer, MAXDATASIZE);
	if(n<0){
		fprintf(stderr, "Error reading clock time from daemon\n");
	}
	printf("%d : Received diff %d\n", pid, *(int*)buffer);
	data = data+*(int *)buffer;
	printf("%d : Sync Logical Clock : %d\n",pid,data);  	
	
	close(socketfd);
	exit(0);
}
