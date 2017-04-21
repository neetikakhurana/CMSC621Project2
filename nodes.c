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
int S=0;
char str[19];
char ip_addr[25];
int port=9000;
void *sendRequests(void * socketfd);
void *listenRequests(void * socketfd);
int pid;
int nprocess;
int main(int argc, char *argv[]){
	struct sockaddr_in server_addr;
    struct hostent *server;
    int portno , i = 0,j,n;
	char buffer[MAXDATASIZE];
	pthread_t sender,listener;
	int *new_sock;
	u_int yes=1;

	if(argc<4){
		fprintf(stderr, "Usage client <hostname> <process> <no of processes>\n");
		exit(1);
	}
	pid=atoi(argv[2]);
	nprocess=atoi(argv[3]);
	portno = 8888;
	server = gethostbyname(argv[1]);

	if(server == NULL)
	{
		fprintf(stderr, "No such host exists\n");
		exit(1);
	}

	strcpy(str,argv[1]);

	int socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
		fprintf(stderr, "Error creating a socket\n");

	//reusing the same host
	if (setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       	perror("Reusing ADDR failed");
       	exit(1);
    }

    //reusing the same port
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
	else{
		printf("Connected to sequencer\n");
	}

	new_sock = (int *)malloc(1);
    *new_sock = socketfd;    
	pthread_create(&sender,NULL,sendRequests,(void*)new_sock);
	pthread_create(&listener,NULL,listenRequests,(void*)new_sock);
	
	pthread_join(sender,NULL);
	pthread_join(listener,NULL);
	exit(0);
}


//thread to send the messages to the sequencer as well as multicast the message to other processes
void *sendRequests(void * socketfd){
	printf("%d : In sender thread\n",pid);
	char buffer[256];
	int newsocket = *(int *)socketfd;
	bzero(buffer, MAXDATASIZE);	
	strcpy(buffer, "Hello");
	sprintf(buffer,"%s %d",buffer,pid);
	printf("Sending to sequencer %s\n", buffer);
	int n = write(newsocket, buffer, MAXDATASIZE);
	if(n < 0)
		fprintf(stderr, "Error writing to socket\n");
	else{
		printf("%d : Write successful to sequencer\n",pid);
		/*for(int i=0;i<nprocess;i++){

		}*/
	}
}


//thread to receive the message from the sequencer as well as other processes
void *listenRequests(void * socketfd){
	printf("%d : In receiver thread\n",pid);
	struct hostent *server;
	int newsock[nprocess];
	int portno,data,n;
	int newsocket = *(int *)socketfd;
	char buffer[256];
	int i=0,j=1;
	bzero(buffer, 256);

	int socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
		fprintf(stderr, "Error creating a socket\n");
	bzero((char *)&serveradd, sizeof(serveradd)); // zero out the buffer
	portno = port+pid;
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = INADDR_ANY; //address ethernet
	serveradd.sin_port = htons(portno); // takes port no and convert into network
	
	if(bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
	{
		fprintf(stderr, "Error binding socket\n");
		exit(1);
	}

	listen(socketfd, 100);



	while(read(newsocket, buffer, 256) && j<nprocess){
		printf("%d : Data received : %s\n",pid,buffer);
		bzero(buffer, 256);
		j++;	
	}
	if(j==nprocess){
		printf("Closing..........................\n");
		close(newsocket);
	}
	while(i < nprocess){
		newsock[i] = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen);

	
}