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

char str[19];
//int port=9000;
void *sendMessage(void * sock);
void *listenMessage(void * sock);
int pid;
int nprocess;
process p[4];
struct hostent *server;


#define MULTICAST_PORT 12000
#define MULTICAST_GROUP "225.0.0.37"

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
	int i=pid;
	bzero(buffer, MAXDATASIZE);	
	strcpy(buffer, "Hello");
	sprintf(buffer,"%s %d",buffer,pid);
	printf("%d : Sending to sequencer %s\n", pid, buffer);
	
	int n = write(newsocket, buffer, MAXDATASIZE);
	if(n < 0)
	{
		fprintf(stderr, "%d : Error writing to socket\n",pid);
	}
	else
	{
		printf("%d : Write successful to sequencer\n",pid);
	}

	//********************************************multicast**********************************
	 struct sockaddr_in addr;
     int fd;
     struct ip_mreq mreq;
  
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  fprintf(stderr, "%d :Socket creation error\n", pid);
	  exit(1);
     }

     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(MULTICAST_GROUP);
     addr.sin_port=htons(MULTICAST_PORT);
     i=0;
     printf("%d : Multicasting message\n", pid);
     while (i<nprocess) {
     	p[pid].socketid=fd;
     	p[pid].pid=pid;
     	strcpy(p[pid].msg,buffer);
		  if (sendto(fd,buffer,MAXDATASIZE,0,(struct sockaddr *) &addr,
			     sizeof(addr)) < 0) {
		       fprintf(stderr,"%d : sendto error",pid);
		       exit(1);
		  }
		  else{
		  //	printf("Message %s sent from %d to process \n", p[pid].msg, p[pid].pid);
		  }
		  i++;
		  sleep(1);
     }

    //***************************************************************************************

	//usleep(2000);
	//pthread_yield();
	//******************************connect to other processes***************************

			/*struct sockaddr_in process_server_addr;
	u_int yes=1;
	int socketfd = socket(AF_INET,SOCK_STREAM, 0);
	
	
	if(socketfd < 0)
	{
		fprintf(stderr, "Error creating a socket\n");
		exit(1);
	}else{
		printf("created socket client\n");
			printf("Is process %d coming here?\n", pid);

	}
		printf("Isgvhgbn process %d coming here?\n", pid);
	
	process_server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&process_server_addr.sin_addr.s_addr,server->h_length);
	//printf("MiewMiew\n");
	process_server_addr.sin_port = htons(port+pid);
	printf("%d : port %d\n", pid, ntohs(process_server_addr.sin_port));

    if(connect(socketfd, (struct sockaddr *) &process_server_addr, sizeof(process_server_addr)) < 0)
	{
		fprintf(stderr, "Error connecting\n");
		exit(1);
	}
	else
	{
		printf("%d: Connected to process %d\n",pid,i);
		rec[i].fid=socketfd;
		strcpy(rec[i].str,buffer);
	}*/

	//reuse address
	/*if (setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
	{
       	perror("Reusing ADDR failed");
       	exit(1);
    }

    //reusing the same port
    if (setsockopt(socketfd,SOL_SOCKET,SO_REUSEPORT,&yes,sizeof(yes)) < 0)
    {
       perror("Reusing PORT failed");
       exit(1);
    }*/
	


	/*rec[i].fid=socketfd;
	strcpy(rec[i].str,buffer);
	for(int j = 0;j<nprocess; j++)
	{
		printf("Sockets for sender %d and receivers %d \n",rec[pid].fid,rec[j].fid);
		if(write(rec[pid].fid, rec[pid].str, MAXDATASIZE) < 0)
		{
			fprintf(stderr, "Error writing to socket\n");
			exit(1);
		}
		else
		{
			printf("WE DID IT for %d %s to pid %d\n",pid,rec[pid].str,j);
		}	
	}*/
	//********************************************************************************	
}


//thread to receive the message from the sequencer as well as other processes
void *listenMessage(void * sock)
{
	printf("%d : In receiver thread\n",pid);
	struct hostent *server;
	int n;
	int newsocket = *(int *)sock;
	char buffer[MAXDATASIZE];
	int i=0,j;

	bzero(buffer, MAXDATASIZE);
	while(read(newsocket, buffer, MAXDATASIZE) && j<nprocess)
	{
		printf("%d : Data received : %s\n",pid,buffer);
		p[pid].seq[j]=buffer;
		printf("seq queue %d %s\n", pid,p[pid].seq[j]);
		bzero(buffer, MAXDATASIZE);
		j++;	
	}
	if(j==nprocess)
	{
		printf("%d : Closing connection with sequencer..........................\n",pid);
		close(newsocket);
	}

//********************************multicast*******************************************
	 struct sockaddr_in addr;
     int fd, nb,addrlen;
     struct ip_mreq mreq;

     u_int yes=1;

     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  fprintf(stderr,"%d : socket creation error",pid);
	  exit(1);
     }

    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       fprintf(stderr,"%d : Reusing ADDR failed",pid);
       exit(1);
       }

     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=htonl(INADDR_ANY);
     addr.sin_port=htons(MULTICAST_PORT);
     
     if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
	  fprintf(stderr, "%d : Bind error\n", pid);
	  exit(1);
     }
     
     mreq.imr_multiaddr.s_addr=inet_addr(MULTICAST_GROUP);
     mreq.imr_interface.s_addr=htonl(INADDR_ANY);
     if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
	  fprintf(stderr,"%d : setsockopt error",pid);
	  exit(1);
     }
     i=0;
     j=0;
     while (i<nprocess) {
	  addrlen=sizeof(addr);
	  bzero(buffer,MAXDATASIZE);
	 if ((nb=recvfrom(fd,buffer,MAXDATASIZE,0,(struct sockaddr *) &addr,&addrlen)) < 0) {
	       fprintf(stderr,"%d : recvfrom",pid);
	       exit(1);
	  }
	  else{
	  	//printf("Message %s %s received in pid %d\n", buffer,rec[pid].str,pid);
	  	if(strcmp(buffer,p[pid].msg)!=0){
	  		p[pid].queue[j]=buffer;
	  		printf("%d in pos %d %s %s %s\n", pid , j,p[pid].queue[j],buffer, p[pid].seq[j]);
	  		j++;
	  	}
	  }
	  i++;
     }
     if(i==nprocess){
     for(j=0;j<3;j++){
     	printf("queue for pid %d at %d: %s\n", pid, j,p[pid].queue[j]);
     }
 }
     //****************************************************************************

	//usleep(2000);

	//*******************************bind to other processes**********************************
	
/*int newsock;
struct sockaddr_in serveradd, clientaddr;
	int socketfd = socket(AF_INET,SOCK_STREAM, 0);
	if(socketfd < 0)
	{
		fprintf(stderr, "Error creating a socket\n");
		exit(1);
	}
	bzero((char *)&serveradd, sizeof(serveradd));
	portno = port+pid;
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = INADDR_ANY;
	serveradd.sin_port = htons(port+pid);
	
	if(bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
	{
		fprintf(stderr, "Error binding socket\n");
		exit(1);
	}
	else
	{
		printf("Socket bound with other processes on port %d\n",ntohs(serveradd.sin_port));
	}

	listen(socketfd, 100);
	printf("Listening to other processes\n");
	
	socklen_t clientlen = sizeof(clientaddr);
	//i=0;
	bzero(buffer,MAXDATASIZE);
	for(i=0;i<nprocess;i++){
				printf("Is process %d reading?\n",pid );

		
		if((newsock = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen))<0){
			fprintf(stderr, "%d : Error accepting requests from other processes\n", pid);
		}
		else{
			printf("%d : Accepted\n", pid);
			rec[i].fid=newsock;
			//rec[i].str=buffer;
			rec[i].pid=pid;
		}
*/		/*int x=read(newsock,buffer,MAXDATASIZE);
		if(x<0)
		{
			fprintf(stderr, "Error reading data from other processes\n");
			exit(1);
		}
		else
		{
			printf("Data from other processes%s\n", buffer);
		}
		i++;*/
//	}
	//****************************************************************************************
}