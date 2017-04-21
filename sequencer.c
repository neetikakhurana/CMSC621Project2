#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h> //for threading , link with lpthread

#define MAXDATASIZE 256
#define NPROCESS 4
#define PORT 8888

typedef struct {
	int socketid;
	char msg[35];
	int pid;
	int S;
}process;

int socketfd, newsockfd, portno,*new_sock;
struct sockaddr_in serveradd, clientaddr;
pthread_mutex_t lock; 
char buffer[MAXDATASIZE]; 
int k,i = 0,j=0,S=0;

int main(int argc, char *argv[])
{
 	process p[4];
 	socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
		fprintf(stderr, "Error creating a socket\n");
	bzero((char *)&serveradd, sizeof(serveradd));
	portno = PORT;
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = INADDR_ANY;
	serveradd.sin_port = htons(portno);
	
	if(bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
	{
		fprintf(stderr, "Error binding socket\n");
		exit(1);
	}

	listen(socketfd, 100);
	socklen_t clientlen = sizeof(clientaddr);
	i = 0;
	usleep(3000);
	
	while(i < NPROCESS){
		newsockfd = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen);
		printf("%d\n",newsockfd);
		p[i].socketid = newsockfd;
        if(newsockfd < 0)
		{
			fprintf(stderr, "Error accepting client request\n");
		} 
		bzero(buffer, MAXDATASIZE);

		//reading for data
		int n = read(newsockfd, buffer, MAXDATASIZE);
		if(n < 0){
			fprintf(stderr, "reading error\n");
		}
		else{
	    	strcpy(p[i].msg,buffer);
	    	printf("sock %d data %s\n", p[i].socketid,p[i].msg);
	    	char *pi;
	    	pi=strtok(buffer," ");
	    	pi=strtok(NULL," ");
	    	p[i].S=S+1;
	    	S++;
	    	printf("pid %d seq counter %d\n",atoi(pi),p[i].S);
	    	p[i].pid=atoi(pi);
		}
        i++;
	}
	for( i = 0;i < NPROCESS; i++){
		sprintf(p[i].msg,"%s %d",p[i].msg,p[i].S);	
		for(j = 0;j<NPROCESS; j++){
			printf("Sockets for sender %d and receivers %d \n",p[i].socketid,p[j].socketid);
			if(p[j].socketid!=p[i].socketid){	
				if(write(p[j].socketid, p[i].msg, MAXDATASIZE) < 0){
					fprintf(stderr, "Error writing to socket\n");
					exit(0);
				}
				else{
					printf("Written for %d %s to pid %d\n",p[i].pid,p[i].msg,p[j].pid);
				}	
			}
			else
				continue;
			
		}
	}
	for(int i = 0; i < NPROCESS; i++){
		close(p[i].socketid);
	}
	close(socketfd);
    return 0;
}
