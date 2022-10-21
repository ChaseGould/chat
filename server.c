// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <stdbool.h>
#include <arpa/inet.h>



// taken from: https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
char *inputString(FILE* fp, size_t size){
//The size is extended by the input with the value of the provisional
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(char)*size);//size is start size
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(char)*len);
}



int main(int argc, char *argv[]) 
{ 
	//port number input from command line.
	int PORT = atoi (argv[1]);
	
	printf("server waiting");

	
	// Creating socket file descriptor 
	int server_fd; 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

	
	// Forcefully attaching socket to the specified port
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,  &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 

	//populate local address structure
	struct sockaddr_in address; 
	int addrlen = sizeof(address);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 

	// Forcefully attaching socket to the port
	if (bind(server_fd, (struct sockaddr *)&address,  
		                 sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	
	//address is populated by the accept function. it is address of incoming request. make it global so it can be shared to other
	struct sockaddr_in incomingAddress; 
	int incomingaddrlen, new_socket;

	if ((new_socket = accept(server_fd, (struct sockaddr *)&incomingAddress, (socklen_t*)&incomingaddrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 
	else	{

		//output accepted connection
		char buffer[incomingaddrlen];
		const char* result = inet_ntop(AF_INET,&incomingAddress.sin_addr,buffer,sizeof(buffer));
		if (result==0) 
			printf("error converting incoming address.");
		 
		printf("The connection to peer %s is successfully established\n", buffer);
	}


	bool repeat = true;
	int valread;
	while(repeat)
	{
		char buffer[1024] = {0}; 
		valread = read( new_socket , buffer, 1024); 
		printf("Received message: %s\n",buffer ); 
	
		printf("send message: ");
		char *message; 
		message = inputString(stdin, 100);

		send(new_socket , message , strlen(message) , 0 ); 
		printf("message sent\n");
		//clear message variable 
		message = "";
	}
	
} 

