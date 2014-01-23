//=================================================
// Name: Daniel Jordan
// Email: daniel_jordan@csu.fullerton.edu
// Date: 3/31/13
// File: client.cpp
// Assignment: UDP file transfer
//=================================================
#include <sys/socket.h> /* For sockets */
#include <netinet/in.h> /* For the necessary types */
#include <stdio.h>	/* For basic I/O */
#include <stdlib.h>	/* For standard functions */
#include <string.h>	/* For memset() */
#include <arpa/inet.h>	/* For inet_pton() */
#include <unistd.h>	/* For close() */
#include <iostream>
using namespace std;
struct file{
	int size;
	char * buffer;
};

int main(int argc, char** argv)
{

	/* File name */
	char * filename = argv[3];

	/* File data being sent to server */
	file sourceFile;

	FILE * source = NULL;
    source = fopen(filename,"r");
    
    fseek(source,0,SEEK_END);
    /* Get file size of input file */
    sourceFile.size = ftell(source);
    fseek(source, 0, SEEK_SET);

    /* Allocate buffer size for file */
	sourceFile.buffer = new char[sourceFile.size+1];
	if (!sourceFile.buffer)
	{
		fprintf(stderr, "Memory error");
		fclose(source);
		exit(-1);
	}
	/* Read file into buffer */
	fread(sourceFile.buffer,1,sourceFile.size,source);
    fclose(source);

	/* To store the port number */
	int port = -1;
	
	/* To store the socket file descriptor */
	int socketfd = -1;
	
	/* The address structures for the server and the client */
	sockaddr_in servaddr, cliaddr;
	
	/* The size of sockaddr_in structure */
	socklen_t sockaddr_in_len = 0;
	
	#define FILENAME_BUFF_SIZE 100

	/* The buffer to store the name of transfered file */
	char filenameBuff[FILENAME_BUFF_SIZE];
	
	/* Check the command line */
	if(argc < 4)
	{
		fprintf(stderr, "USAGE: %s <IP ADRESS> <PORT> <FILENAME>  \n", argv[0]);
		exit(-1);
	}
		
	/* Convert the port number to an integer */
	port = atoi(argv[2]);
	
	/* Make sure the port range is valid */
	if(port < 0 || port > 65535)	
	{
		fprintf(stderr, "Invalid port number\n");
		exit(-1);
	} 	
		
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
   	
		
	/* Initialize the IP address of the server */
	if(!inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr))
	{
		perror("inet_pton");
		exit(-1);
	}
	
	/* Set the port */
	servaddr.sin_port = htons(port);
	
	/* Get the size of the client address data structure */
	sockaddr_in_len = sizeof(cliaddr);	
	
	/* Send the file size to the server */
	if((sendto(socketfd, &sourceFile.size, 4, 0,
					(sockaddr *)&servaddr, sizeof(servaddr))) < 0)
	{
		perror("sendto");
		exit(-1);
	}

	/* Send the filename to the server */
	if((sendto(socketfd, filename, FILENAME_BUFF_SIZE, 0, (sockaddr *)&servaddr, sizeof(servaddr))) < 0)
	{
		perror("sendto");
		exit(-1);
	}


	int numSent = 0; 
	int totalSent = 0;
	while(totalSent != sourceFile.size)
	{
		if((numSent = sendto(socketfd, sourceFile.buffer + totalSent, 1500, 0, (sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		{
			perror("write");
			exit(-1);
		}
		totalSent += numSent;

	return 0;
}
