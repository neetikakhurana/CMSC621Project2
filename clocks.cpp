/*Client program to implement client - server architecture */
/*NAME : ISHA POTNIS*/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <time.h>  //for timestamp
#include <pthread.h> //for threading , link with lpthread


#define MAXDATASIZE 256


typedef struct {
            double timestamp;
            int acc_no; 
            char trans_type[2];
            int amount;
} Transaction;

typedef struct {
            int acc_no; 
            char name[100];
            int amount;
} Test;

Transaction transaction[30];
char *item1;
static int reccount1 = 0,counter = 0;
int socketfd, newsockfd, portno, *new_sock;
struct sockaddr_in serveradd, clientaddr;
pthread_mutex_t lock;
char buffer[MAXDATASIZE]; 
Test record[30];
FILE * filehandle;
char lyne[121],line[121];
char *item;
int k,reccount = 0,i = 0;

void *connection_handler(void *);

int main(int argc, char *argv[]) {
    int funct = atoi(argv[3]);
    struct hostent *server;
    server = gethostbyname(argv[1]);
   // printf("Hello World %d", funct);
    if(argc < 4)   
	{
		fprintf(stderr, "Usage client <hostname> <port number> <daemon(0)/node(1)> <processes>\n");
		exit(1);
	}
	if(server == NULL)
	{
		fprintf(stderr, "No such host exists\n");
		exit(1);
	}
	if(funct == 1){
		socketfd = socket(AF_INET,SOCK_STREAM, 0);
		if(socketfd < 0)
	  		fprintf(stderr, "Error creating a socket\n");

	bzero((char *)&serveradd, sizeof(serveradd)); // zero out the buffer

	portno = atoi(argv[1]);
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = INADDR_ANY; //address ethernet
	serveradd.sin_port = htons(portno); // takes port no and convert into network
	fprintf(stderr,"Hello... reached binding stage");
	if(bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) < 0)
	{
			fprintf(stderr, "Error binding socket\n");
	}

	listen(socketfd, 100); //clients to connect to
	pthread_t sniffer_thread;
	socklen_t clientlen = sizeof(clientaddr);
	//int c = sizeof(struct sockaddr_in);
	while(newsockfd = accept(socketfd, (struct sockaddr *)&clientaddr, &clientlen)){
		puts("Connection accepted");
        new_sock = (int *)malloc(1);
        *new_sock = newsockfd;
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
	}
	if(newsockfd < 0)
	{
			fprintf(stderr, "Error accepting client request\n");
	} 
	for(int i = 0; i < 100; i++){
		pthread_join(sniffer_thread,NULL);
	}
	close(socketfd);
	fclose(filehandle);

	} else if(funct == 0){
		fprintf(stderr,"Hi");
		 FILE * filehandle;
    clock_t tin, tout,tfinal;
    char line[120],lyne[120];
    int k, reccount = 0;
    struct hostent *server;
	int portno;
	double wait  = atof(argv[3]);
	portno = atoi(argv[2]);
	server = gethostbyname(argv[1]);
	char filename[40] ;
	strcpy(filename,argv[4]);
	if(server == NULL)
	{
		fprintf(stderr, "No such host exists\n");
		exit(1);
	}

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port = htons(portno);	

	int socketfd = socket(AF_INET, SOCK_STREAM, 0); //create end point for communication
	if(socketfd < 0)
	{
		fprintf(stderr, "Socket not formed \n");
		exit(0);
	}

	if(connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "Error connecting\n");
	}

	filehandle = fopen(filename,"r");    
    if(filehandle == 0){
        printf("Unable to open file");
        exit(1);
    }
    char buffer[256];
    fgets(line,120,filehandle);

    double stamp = 0.0;
    do{
    	reccount++;
    	strcpy(lyne,line);
    	char *item = strtok(lyne," ");
    	double timestamp = atof(item);
    	timestamp = timestamp * wait;
    	if(stamp != timestamp){
    		double x = timestamp - stamp; 
    		sleep(x);
    		stamp = timestamp;
    	}
    	printf("%lf",stamp);
    	strcpy(buffer, line);
    	
    	int n = write(socketfd, buffer, strlen(buffer));
    	printf("Data sent by client : %s\n",buffer); 
		if(n < 0)
		{	
			fprintf(stderr, "Error with writing to socket\n");
			exit(1);	
		}
		n = read(socketfd, buffer, 255);
		printf("Data Received : %s\n",buffer);           
    }while (fgets(line,120,filehandle));
   	fclose(filehandle);    
    close(socketfd);
	exit(0);
	}
	exit(1);
}

	
void *connection_handler(void *socketfd){
	//counter++;
	//pthread_id_np_t tid;
	//tid = pthread_getthreadid_np();
	bzero(buffer, MAXDATASIZE);
	int newsocket = *(int *)socketfd;
	int n = read(newsocket, buffer, MAXDATASIZE);
	do{
		
		if(n < 0)
		{
			fprintf(stderr, "Error in receiving data\n");
			exit(1);
		}
	
		//printf("Received Message: %s from client %d\n ",buffer,tid);
	
			item = strtok(buffer," ");
    		transaction[reccount].timestamp = atof(item);

    		item = strtok(NULL," ");
		    transaction[reccount].acc_no = atoi(item);

    		item = strtok(NULL," ");
   			strcpy(transaction[reccount].trans_type,item);

    		item = strtok(NULL," ");
    		transaction[reccount].amount = atoi(item);
	
    		if(strcmp(transaction[reccount].trans_type,"w") == 0){
    			
    			for (i = 0; i < reccount1; i++)
    			{
					if(record[i].acc_no == transaction[reccount].acc_no){
						pthread_mutex_lock(&lock);
						if(record[i].amount >= transaction[reccount].amount){   // condition to check if withdrawn amount is less than amount present in account 
							
							record[i].amount = record[i].amount - transaction[reccount].amount;
							pthread_mutex_unlock(&lock);
							break;
						}
						else {
							fprintf(stderr,"Insufficient funds in account error.");
						}
						pthread_mutex_unlock(&lock);
					}    		
    			}
    			if(i == reccount1){
    				n = write(newsocket,"NACK\n", 4);
    				if(n < 0){
    					fprintf(stderr,"No such account exists in the database");
    				}
    			}
    		} else if(strcmp(transaction[reccount].trans_type,"d") == 0){
    			for (i = 0; i < reccount1; i++)
    			{
					if(record[i].acc_no == transaction[reccount].acc_no){
						pthread_mutex_lock(&lock);
						record[i].amount = record[i].amount + transaction[reccount].amount;
						pthread_mutex_unlock(&lock);
						break;
					}    		
    			}
    			if(i == reccount1){
    				n = write(newsocket,"NACK\n", 4);
    				if(n < 0){
    					fprintf(stderr,"No such account exists in the database");
    				}
    			}
    		}

  			n = write(newsocket, "ACK\n", 4);
			if(n < 0)
				fprintf(stderr, "Error writing to socket\n");
			bzero(buffer, MAXDATASIZE);
	}while(read(newsocket, buffer, MAXDATASIZE));
	printf("Records at end of transaction : \n");
	
	close(newsocket);

}