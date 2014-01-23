//=================================================
// Name: Daniel Jordan
// Email: daniel_jordan@csu.fullerton.edu
// Date: 3/31/13
// File: server.cpp
// Assignment: UDP file transfer
//=================================================
#include <sys/socket.h> /* For sockets */
#include <netinet/in.h> /* For the necessary types */
#include <stdio.h>	/* For basic I/O */
#include <stdlib.h>	/* For standard functions */
#include <string.h>	/* For memset() */

struct file{
	int size;
	char * buffer;
};
int main(int argc, char** argv)
{
	/* Holds data for file */
	file destinationFile;

	/* To store the port number */
	int port;
	
	/* To store the socket file descriptor */
	int socketfd;
	
	/* The address structures for the server and the client */
	sockaddr_in servaddr, cliaddr;
	
	/* The size of sockaddr_in structure */
	socklen_t sockaddr_in_len = 0;
	
	/* The number of bytes received and sent */
	int numRecv = -1, numSent = -1;	
	
	/* The maximum sizes for sending and receiving */

	#define FILENAME_BUFF_SIZE 100

	/* The buffer to store the received string */
	char filenameBuff[FILENAME_BUFF_SIZE];
		
	/* Check the command line */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <PORT>\n", argv[0]);
		exit(-1);
	}
		
	/* Convert the port number to an integer */
	port = atoi(argv[1]);

	/* Make sure the port range is valid */
	if(port < 0 || port > 65535)	
	{
		fprintf(stderr, "Invalid port number\n");
		exit(-1);
	} 	
	/* Print the port number */	
	fprintf(stderr, "Port  = %d\n", port); 	
		
	/* Create a UDP socket */
	if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		exit(-1);
	}
		
	/* Clear the structure for the server address */
	memset(&servaddr, 0, sizeof(servaddr));
	
	/* Populate the server address structure */
	servaddr.sin_family = AF_INET;
   	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	
	/* Bind the socket to the port */
	if(bind(socketfd,(sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind");
		exit(-1);
	}
	
	/* Keep receiving forever */
	while(true)
	{
		/* Get the size of the client address data structure */
		sockaddr_in_len = sizeof(cliaddr);	
		

		/* Get file size from server */
		if((numRecv = recvfrom(socketfd, &destinationFile.size, 4, 0, 
			(sockaddr *)&cliaddr, &sockaddr_in_len)) < 0)
		{
			perror("recvfrom");
			exit(-1);
		}

		/* Get filename from server */
		if((numRecv = recvfrom(socketfd, filenameBuff, FILENAME_BUFF_SIZE, 0, 
			(sockaddr *)&cliaddr, &sockaddr_in_len)) < 0)
		{
			perror("recvfrom");
			exit(-1);
		}

		/* Get file contents from client */
		int numReceived = 0, totalReceived = 0;
		while(totalReceived != destinationFile.size)
		{
			if((numReceived = recvfrom(socketfd, destinationFile.buffer + totalReceived, 1500, 0, 
			(sockaddr *)&cliaddr, &sockaddr_in_len)) < 0)
			{
				perror("read");
				exit(-1);
			}
			totalReceived += numReceived;
		}


		fprintf(stderr, "File Name: %s\n", filenameBuff); 
		fprintf(stderr, "File Size: %i bytes\n", destinationFile.size); 
		fprintf(stderr, "Bytes written: %i bytes\n", totalReceived); 

		if(totalReceived == destinationFile.size)
			fprintf(stderr, "%s successfully transfered\n", filenameBuff); 
		else
			fprintf(stderr, "Error during file transfer\n"); 
	
		FILE * destFile;
		destFile = fopen(filenameBuff, "wb");
		fwrite(destinationFile.buffer, 1 , destinationFile.size , destFile);
		fclose(destFile);			
	}
	
	return 0;
}
