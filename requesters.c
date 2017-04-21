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
#define PORT 8888

typedef struct{
	int socketid;
	char msg[35];
	int S;
	int pid;
	char *queue[3];
	char *seq[3];
}process;

pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER;
char ch;
void *sendMessage(void * sock);
void *listenMessage(void * sock);
int pid;
int nprocess;
process p[4];
struct hostent *server;
FILE* myfile;
int d=0;
int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr;
    
    int portno , i = 0,j,n;
	char buffer[MAXDATASIZE];
	pthread_t sender,listener;
	int *new_sock;
	u_int yes=1;
	
	if(argc<4)
	{
		fprintf(stderr, "Usage client <hostname> <process> <no of processes>\n");
		exit(1);
	}
	pid=atoi(argv[2]);
	nprocess=atoi(argv[3]);
	//reading the file
	myfile=fopen("Sample.txt","r");
	if(myfile==NULL){
		fprintf(stderr, "%d : Error opening file\n", pid);
	}
	else{
		printf("%d : File opened\n", pid);
	}

	do{
		ch=fgetc(myfile);
		//printf("%c\n", ch);
		break;
	}while(ch!=EOF);
	
	printf("%d : File contents %c\n",pid,ch);
	rewind(myfile);

	fclose(myfile);

	
	portno = PORT;
	server = gethostbyname(argv[1]);

	if(server == NULL)
	{
		fprintf(stderr, "No such host exists\n");
		exit(1);
	}

	//Creating socket to connect to the sequencer
	int socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
		fprintf(stderr, "%d : Error creating a socket\n",pid);

	//reusing the same host
	if (setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       	fprintf(stderr,"%d : Reusing ADDR failed",pid);
       	exit(1);
    }

    //reusing the same port
    if (setsockopt(socketfd,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(yes)) < 0) {
       fprintf(stderr,"%d : Reusing PORT failed",pid);
       exit(1);
    }

	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port = htons(portno);	
	
    if(connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "%d : Error connecting to sequencer\n",pid);
		exit(1);
	}
	else
	{
		printf("%d : Connected to sequencer\n",pid);
	}

	new_sock = (int *)malloc(1);
    *new_sock = socketfd;    
	pthread_create(&sender,NULL,sendMessage,(void*)new_sock);
	pthread_create(&listener,NULL,listenMessage,(void*)new_sock);
	
	pthread_join(sender,NULL);
	pthread_join(listener,NULL);
	exit(0);
}

//thread to send the messages to the sequencer as well as multicast the message to other processes
void *sendMessage(void * sock)
{
	printf("%d : In sender thread\n",pid);
	char buffer[MAXDATASIZE];
	int newsocket = *(int *)sock;
	struct hostent *server;
	int i=0;
	bzero(buffer, MAXDATASIZE);

//send msg to sequencer to access the file
	*(int *)buffer=pid;
	//sprintf(buffer, "%d",pid);
	printf("%d : Sending to sequencer %d\n", pid, *(int *)buffer);
	
	int n = write(newsocket, buffer, MAXDATASIZE);
	if(n < 0)
	{
		fprintf(stderr, "%d : Error writing to socket\n",pid);
	}
	else
	{
		printf("%d : Request sent to sequencer\n",pid);
	}

//done writing to file
	if(d==1){
		printf("%d : Done with writing to file\n", pid);
		bzero(buffer,MAXDATASIZE);
		strcpy(buffer,"DONE");
		n = write(newsocket, buffer, MAXDATASIZE);
		if(n < 0)
		{
			fprintf(stderr, "%d : Error writing to socket\n",pid);
		}
		else
		{
			printf("%d : Completion msg sent to sequencer\n",pid);
			d=0;
		}
	}
}


//thread to receive the message from the sequencer as well as other processes
void *listenMessage(void * sock)
{
	printf("%d : In receiver thread\n",pid);
	int n;
	int newsocket = *(int *)sock;
	char buffer[MAXDATASIZE];
	int i=0,j;
			FILE *file;

	bzero(buffer, MAXDATASIZE);
	//nothing will be sent in case file is not available
	n=read(newsocket, buffer, MAXDATASIZE);
	if(n<0){
		fprintf(stderr, "Error reading response from sequencer\n");
		exit(1);
	}
	printf("%d : Data received : %s\n",pid,buffer);
		if(strcmp(buffer,"OK")==0){
			file=fopen("Sample.txt","r+");
			do{
					ch=fgetc(myfile);
						break;
				}while(ch!=EOF);
				printf("%d : New read value %c\n", pid,ch);
				rewind(file);
				printf("%d : Acquiring lock\n",pid);
			pthread_mutex_lock(&mut);
			fseek(file,0,SEEK_SET);
			fprintf(file, "%d\n",((ch-'0')+1));
			pthread_mutex_unlock(&mut);
			printf("%d : Releasing lock\n", pid);
			d=1;
			fclose(file);
			pthread_yield();
		}
		/*elseif(strcmp(buffer,"NO")==0){
			sleep(2000);
		}*/
			fflush(file);
}