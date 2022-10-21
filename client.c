// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <stdbool.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/types.h>




// taken from: https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length
//be careful as this function will pickup the last keystroke which is usually <enter>?
//in these cases must place a scanf before inputstring to pickup the <enter>
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



void commandMenu()
{
	printf("Welcome to client portal. Please type the option of your choice\n");
	printf("help\n");
	printf("myip\n");
	printf("myport\n");
	printf("connect\n");
	printf("list\n");
	printf("send\n");
	printf("exit\n");
}	

// function to get my ip address. Modified code from here: http://man7.org/linux/man-pages/man3/getifaddrs.3.html
void myIPAddress(){
	struct ifaddrs *ifaddr, *ifa;
           int family, s, n;
           char host[NI_MAXHOST];

           if (getifaddrs(&ifaddr) == -1) {
               perror("getifaddrs");
               exit(EXIT_FAILURE);
           }

           /* Walk through linked list, maintaining head pointer so we
              can free list later */

           for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
               if (ifa->ifa_addr == NULL)
                   continue;

               family = ifa->ifa_addr->sa_family;

               if (family == AF_INET || family == AF_INET6) {
                   s = getnameinfo(ifa->ifa_addr,
                           (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                                 sizeof(struct sockaddr_in6),
                           host, NI_MAXHOST,
                           NULL, 0, NI_NUMERICHOST);
                   if (s != 0) {
                       printf("getnameinfo() failed: %s\n", gai_strerror(s));
                       exit(EXIT_FAILURE);
                   }

                   printf("\t\taddress: <%s>\n", host);

               } 
           }

           freeifaddrs(ifaddr);
}

int get_free_socket(bool sockets[6])
{
	for (int i = 0; i < 6; i++)
	{
		if (sockets[i] == 0)
			return i;
	}

	printf("All sockets in use.");
}



   
int main(int argc, char *argv[]) 
{ 
	int PORT = atoi (argv[1]);
	//used to pickup empty strings
	char msg[100];

	char buffer[1024] = {0}; 
	int sock[6];

	//INITIALIZE 6 SOCKETS
	for (int i = 0; i < 6; i++)
	{
		if ((sock[i] = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		{ 
			printf("\n Socket creation error \n"); 
			return -1; 
		} 
	}

	//array keeps track of which sockets are in use. all initialized to not be in use.
	bool sockets_in_use[6];
	for(int i = 0; i < 6; i++)
	{
		sockets_in_use[i] = false;
	}

	
	commandMenu();

	//list of ip addresses for connected peers
	char peerAddrs[10][20];
	int peerPorts[10];
	

	//initialize all peers addresses and ports to arbitrary value.
	for (int i = 0; i < 10; i++)
	{
		strcpy(peerAddrs[i], "a"); 
		peerPorts[i] = 0;	

	}
	
	bool exit = false;
	char *choice;
	while(exit == false)
	{

		printf("\nselection: ");
		choice = inputString(stdin, 10);

		//help
		if(strcmp(choice,"help") == 0)
		{
			commandMenu();
		}

		//my ip address
		else if(strcmp(choice,"myip") == 0)
		{
			myIPAddress();
			printf("\n");
		}

		//my port
		else if(strcmp(choice,"myport") == 0)
		{
			printf("Port: %i in use\n", PORT);
		}

		//connect
		else if(strcmp(choice,"connect") == 0)
		{
			//following data structure will hold address of requested server
			struct sockaddr_in serv_addr; 	
			memset(&serv_addr, '0', sizeof(serv_addr)); 
			serv_addr.sin_family = AF_INET; 
			 
			//get destination address and port from user.
			printf("Enter the destination ip address: ");
			const char *destAddress;
			destAddress= inputString(stdin, 100);
			printf("Enter the destination port number: ");
			int newPORT;
			scanf("%i", &newPORT);

	
			serv_addr.sin_port = htons(newPORT);
			
			//sending requests by default to my ip address this needs to send requests to a user selected ip address
			// Convert IPv4 and IPv6 addresses from text to binary form and stores it in serv_addr.sin_addr
			if(inet_pton(AF_INET, destAddress, &serv_addr.sin_addr)<=0)  
			{ 
				printf("\nInvalid address/ Address not supported \n"); 
				continue;
			} 
			
			int free_socket = get_free_socket(sockets_in_use);
			if (connect(sock[free_socket], (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
				printf("\nConnection Failed \n"); 
			
			else
			{
				sockets_in_use[free_socket] = 1; //mark the curr socket as in use.
				printf("The connection to peer (%s) is successfully established\n", destAddress);
				//pickup empty string
				fgets(msg, 100, stdin);
				//add to peer list
				for(int i = 0; i < 10; i++)
				{	
					if(strcmp(peerAddrs[i],"a") == 0)
					{
						strcpy(peerAddrs[i], destAddress);
						peerPorts[i] = newPORT;
						break;
					}	
				}
			}
		}
	

		//list
		else if(strcmp(choice,"list") == 0)
		{
			printf("id:  IP address \t\tPort Number\n");
			for(int i = 0; i < 10; i++)
			{
				if(strcmp(peerAddrs[i],"a") != 0)
				{
					printf("%i: %s \t\t %i\n", i,peerAddrs[i],peerPorts[i]);
				}
			}
		}

		
		//send
		else if(strcmp(choice,"send") == 0)
		{
			printf("enter connection id: ");
			int connectionID;			
			scanf("%i", &connectionID);
			//pickup the empty string
			fgets(msg, 100, stdin);	
			printf("enter message: ");
			char *message;
			message = inputString(stdin, 100);
	
			//second argument should be a pointer to a buffer of a message to send
			send(sock[connectionID] , message , strlen(message) , 0 );

			message = "";

			char buffer[1024] = {0}; 
			int valread;
			valread = read( sock[connectionID] , buffer, 1024); 
			if (strlen(buffer) > 0)
				printf("received message: %s\n",buffer ); 

		}


		else if(strcmp(choice,"exit") == 0)
		{
			exit = true;
		}

	}//end of while.

	
	
		

	

		
	

	 
} 


