#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXDATASIZE 256

char str[19];
char ip_addr[25];
void *sendReq(void * socketfd);
void *listenReq(void * socketfd);

int main(int argc, char *argv[]){
	struct sockaddr_in server_addr;
    struct hostent *server;
    int portno , i = 0,j,n;
	char buffer[MAXDATASIZE];
	pthread_t tid1,tid2;
	int *new_sock;
	u_int yes=1;
	portno = 8880;
	server = gethostbyname(argv[2]);
	if(server == NULL)
	{
		fprintf(stderr, "No such host exists\n");
		exit(1);
	}
	//printf("Hi");
	strcpy(str,argv[1]);
	//strcpy(ip_addr,argv[2]);
	int socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
		fprintf(stderr, "Error creating a socket\n");

	if (setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       	perror("Reusing ADDR failed");
       	exit(1);
    }
    if (setsockopt(socketfd,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(yes)) < 0) {
       perror("Reusing PORT failed");
       exit(1);
    }

	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port = htons(portno);	
	
    if(connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "Error connecting\n");
	}
	new_sock = (int *)malloc(1);
    *new_sock = socketfd;    
	pthread_create(&tid1,NULL,sendReq,(void*)new_sock);
	pthread_create(&tid2,NULL,listenReq,(void*)new_sock);
	
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	exit(0);
}

void *sendReq(void * socketfd){
	char buffer[256];
	int newsocket = *(int *)socketfd;
	//printf("HEllo");
	bzero(buffer, MAXDATASIZE);	
	strcpy(buffer, str);
	printf("UMBC");
	int n = write(newsocket, "ho", MAXDATASIZE);
	if(n < 0)
		fprintf(stderr, "Error writing to socket\n");
	else{
		printf("write successful\n");
	}
}

void *listenReq(void * socketfd){
	struct hostent *server;
	int portno,data,n;
	int newsocket = *(int *)socketfd;
	char buffer[256];
	printf("Inside listen");
	bzero(buffer, 256);
	while(read(newsocket, buffer, 256)){
		printf("Data received : %s",buffer);
		bzero(buffer, 256);	
	}
	
}