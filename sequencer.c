/*Sequencer program to implement TCP multicast architecture */
/*NAME : ISHA POTNIS*/
/*STUD ID : NE12771*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h> //for threading , link with lpthread

#define MAXDATASIZE 256
#define MAXPROCESS 4

typedef struct {
	int fid;
	char str[35];
}record;

int socketfd, newsockfd, portno, *new_sock;
struct sockaddr_in serveradd, clientaddr;
pthread_mutex_t lock; 
char buffer[MAXDATASIZE]; 
int k,reccount = 0,i = 0,j=0;


int main(int argc, char *argv[])
{
 	record rec[4];
 	socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
		fprintf(stderr, "Error creating a socket\n");
	bzero((char *)&serveradd, sizeof(serveradd)); // zero out the buffer
	portno = 8880;
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = INADDR_ANY; //address ethernet
	serveradd.sin_port = htons(portno); // takes port no and convert into network
	
	if(bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
	{
		fprintf(stderr, "Error binding socket\n");
		exit(1);
	}

	listen(socketfd, 100); //clients to connect to
	pthread_t sniffer_thread;
	socklen_t clientlen = sizeof(clientaddr);
	i = 0;
	while(i < MAXPROCESS){
		newsockfd = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen);
		printf("%d\n",newsockfd);
		//puts("Connection accepted");
		//printf("Accepted\n");
		rec[i].fid = newsockfd;
         if(newsockfd < 0)
		{
			fprintf(stderr, "Error accepting client request\n");
		} 
		bzero(buffer, MAXDATASIZE);
        //sleep(2);
		int n = read(newsockfd, buffer, MAXDATASIZE);
		if(n < 0){
			fprintf(stderr, "reading error\n");
		}
		else{
	    	strcpy(rec[i].str,buffer);
		}
       /* new_sock = (int *)malloc(1);
        *new_sock = newsockfd;*/
		//int newsocket = *(int *)socketfd;
//		int n = read(newsockfd, buffer, MAXDATASIZE);
//        printf("New message: %s",buffer);
      //  printf("HIeeeee");
        i++;
	}
	for( i = 0;i < MAXPROCESS; i++){
		for(j = 0;j<MAXPROCESS; j++){
			
				if(write(rec[j].fid, rec[j].str, MAXDATASIZE) < 0){
					fprintf(stderr, "Error writing to socket\n");
					exit(0);
				}
				else{
					printf("WE DID IT %s",rec[j].str);
				}	
			
		}			
	}
	//for(int i = 0; i < 100; i++){
//		pthread_join(sniffer_thread,NULL);
//	}
	close(socketfd);
    return 0;
}
