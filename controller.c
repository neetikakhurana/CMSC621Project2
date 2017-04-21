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
char buffer[MAXDATASIZE]; 
int i=0,j=0;
int n;
int user=0;
int requests[4];

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
	//usleep(3000);
	int req=0;
	int grant=0;
	while(i < NPROCESS){
		newsockfd = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen);
		//printf("%d\n",newsockfd);
		p[i].socketid = newsockfd;
        if(newsockfd < 0)
		{
			fprintf(stderr, "Error accepting client request\n");
		} 
		bzero(buffer, MAXDATASIZE);

		//reading for data
		while(n=(read(newsockfd, buffer, MAXDATASIZE))){
			if(n < 0){
				fprintf(stderr, "reading error\n");
			}
			else{
				//read the requester id and grant access if u can
				if(strcmp(buffer,"DONE")!=0){
					p[i].pid=*(int *)buffer;
					printf("Done is not received from %d\n", p[i].pid);
					  	if(user==0){
					  		printf("Granting access to %d \n", p[i].pid);
				    		bzero(buffer,MAXDATASIZE);
				    		strcpy(buffer,"OK");
				    		n=write(p[i].socketid,buffer,MAXDATASIZE);
				    		if(n<0){
				    			fprintf(stderr, "Error sending OK response\n");
				    		}
				    		else{
				    			printf("OK response sent\n");
				    			user=p[i].pid;
				    			printf("Access is now with %d \n", user);
				    		}
				    	}
				    	else{
				    		requests[req]=p[i].pid;
				    		printf("Added to request queue %d\n", requests[req]);
							req++;
				    	}
			    }
			    else{
			    	printf("File released..\n");
			    	if((sizeof(requests)/sizeof(int))==0){
			    		//do nothing
			    		printf("NO pending requests\n");
			    	}
			    	else{
			    		printf("SEnding to other process\n");
			    		bzero(buffer,MAXDATASIZE);
			    		strcpy(buffer,"OK");
			    		for(int x=0;x<NPROCESS;x++){
			    			if(p[x].pid==requests[grant]){
			    				printf("Request granted to %d\n", requests[grant]);
			    				n=write(p[x].socketid,buffer,MAXDATASIZE);
					    		if(n<0){
					    			fprintf(stderr, "Error sending OK response\n");
					    		}
					    		else{
					    			printf("OK response sent\n");
					    			user=requests[grant];
					    		}
			    			}
			    		}
			    	
			    	}
			    }
			}
	    	//char *pi;
	    	//pi=strtok(buffer," ");
	    	//pi=strtok(NULL," ");
	    	//printf("pid %d seq counter %d\n",atoi(pi),p[i].S);
	    	//p[i].pid=atoi(pi);
		}
        i++;
	}
	/*for( i = 0;i < NPROCESS; i++){
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
	}*/
	for(int i = 0; i < NPROCESS; i++){
		close(p[i].socketid);
	}
	close(socketfd);
    return 0;
}
