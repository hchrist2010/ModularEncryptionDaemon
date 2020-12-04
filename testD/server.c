#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void sendMsg(char *msg, int socketFD){
	int charsWritten;
//	char buffer[256];
	charsWritten = send(socketFD, msg, strlen(msg), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(msg)) printf("CLIENT: WARNING: Not all data written to socket!\n");
//	sleep(0.1);

}

int receiveMsg(char *msg, int socketFD, int size){
	memset(msg, '\0', sizeof(msg));
	int charsRead;
	int count = 0;
	char buffer[256];

	int i = 0;
	memset(buffer, '\0', size);
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	count += charsRead;
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	strncat(msg, buffer, 255);
//	printf("Count: %d\n", count);
	return charsRead;
}

void sendFile(char *msg, int size, int socketFD){
	int charsWritten;
	int count;
	int index = 0;
	char *buffer;
	buffer = (char*)malloc(256);
	while(index < size - 1){
		memset(buffer, '\0', sizeof(buffer));
		strncpy(buffer, msg[index], 255);
		index += 255;
		charsWritten = send(socketFD, buffer, 255, 0);
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(msg)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	}
	charsWritten = send(socketFD, "#", sizeof("#"), 0);
}

void receiveFile(char *msg, int size, int socketFD){
//	msg = malloc(size);
	int charsRead;
	int count = 0;
	char buffer[256];
	memset(buffer, '\0', 256);
	while(strcmp(buffer, "#") != 0){
		memset(buffer, '\0', 256);
		charsRead = recv(socketFD, buffer, 255, 0); // Read the client's message from the socket
		count += charsRead;
		strncat(msg, buffer, 255);
		if(count == size - 5){
			return;
		}
	}
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");

	char *enc;
	char *key;
	int encSize;
	int keySize;
	int temp;
	int count = 0;
	int i;
	memset(buffer, '\0', sizeof(buffer));
	temp = receiveMsg(buffer, establishedConnectionFD, sizeof(buffer));
	if(strcmp(buffer, "enc") == 0){
		sendMsg("0", establishedConnectionFD);
	}
	else{
		sendMsg("1", establishedConnectionFD);
	}

	//receive file string
	memset(buffer, '\0', sizeof(buffer));
	temp = receiveMsg(buffer, establishedConnectionFD, sizeof(buffer));
	encSize = atoi(buffer);
	sendMsg("0", establishedConnectionFD);
	enc = (char*)malloc(encSize + 256);
	memset(buffer, '\0', sizeof(buffer));
	receiveFile(enc, encSize, establishedConnectionFD);
	printf("Enc: %s\n", enc);

	memset(buffer, '\0', sizeof(buffer));
	temp = receiveMsg(buffer, establishedConnectionFD, sizeof(buffer));
	keySize = atoi(buffer);
	sendMsg("0", establishedConnectionFD);
	key = (char*)malloc(keySize + 256);
	receiveFile(key, keySize, establishedConnectionFD);
//	printf("Key: %s\n", key);
//	sendMsg("0", establishedConnectionFD);

	for(i = 0; i < encSize-1; i++){
		if(enc[i] == ' '){
			enc[i] = '[';
		}
		if(enc[i] < 65 || enc[i] > 91){
			sendMsg("2", establishedConnectionFD);
//			charsRead = send(establishedConnectionFD, "2", strlen("2"), 0);
			exit(1);
		}

	}
//	charsRead = send(establishedConnectionFD, "0", strlen("0"), 0);
	sendMsg("0", establishedConnectionFD);
	for(i = 0; i < encSize - 1; i++){
		enc[i] -= 65;
		temp = (int)enc[i] + (int)key[i];
		temp = temp % 27;
		enc[i] = (char)(temp + 65);
	}
	for(i = 0; i < encSize - 1; i++){
		if(enc[i] == '['){
			enc[i] = ' ';
		}
	}
	count = 0;
//	while(count < fileLength){
	sendMsg(enc, establishedConnectionFD);
//		charsRead = send(establishedConnectionFD, enc, strlen(enc), 0); // Write to the server
//		count += charsRead;
//	}
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse

	receiveMsg(buffer, establishedConnectionFD, sizeof(buffer));
//	charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
//	if (charsRead < 0) error("CLIENT: ERROR writing to socket");
//	printf("5\n");


	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0;
}
