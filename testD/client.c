#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

void sendMsg(char *msg, int socketFD){
	int charsWritten;
//	char buffer[256];
	charsWritten = send(socketFD, msg, strlen(msg), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(msg)) printf("CLIENT: WARNING: Not all data written to socket!\n");
//	sleep(0.1);
}

int receiveMsg(char *msg, int socketFD){
	memset(msg, '\0', sizeof(msg));
	int charsRead;
	int count = 0;
	int size;
	size = sizeof(msg);
	char buffer[256];
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(socketFD, msg, sizeof(msg) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	return charsRead;
//	printf("server receive: %s\n", msg);
//	sleep(0.1);

//	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
}

void receiveFile(char *msg, int size, int socketFD){
//	msg = malloc(size);
	int charsRead;
	int count = 0;
	char buffer[256];
	while(count < size - 1){
		memset(buffer, '\0', 256);
		charsRead = recv(socketFD, buffer, 255, 0); // Read the client's message from the socket
		count += charsRead;
		strncat(msg, buffer, 255);
	}
}

void sendFile(char *msg, int size, int socketFD){
	int charsWritten;
	int count;
	int index = 0;
	char buffer[256];
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


int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];

	if (argc < 2) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	FILE *fp;
	FILE *fk;
	char *enc;
	char *key;
	char ch;
	int encSize;
	int keySize;
	int count = 0;
	int temp;

	fp = fopen(argv[1], "r");
	while((ch = fgetc(fp)) != EOF){
		count++;
	}
	encSize = count;
	fclose(fp);
	enc = (char*)malloc(count);
	fp = fopen(argv[1], "r");
	fgets(enc, encSize, fp);
	fclose(fp);

	count = 0;
	fk = fopen(argv[2], "r");
	while((ch = fgetc(fp)) != EOF){
		count++;
	}
	keySize = count;
	fclose(fk);
	key = (char*)malloc(count);
	fk = fopen(argv[2], "r");
	fgets(key, keySize, fk);
	fclose(fk);

	sendMsg("enc", socketFD);
	temp = receiveMsg(buffer, socketFD);
	if(strcmp(buffer, "0") == 0){
		printf("connected\n");
	}
	else{
		printf("connection failed\n");
	}

	sprintf(buffer, "%d", encSize);
	sendMsg(buffer, socketFD);
	temp = receiveMsg(buffer, socketFD);
	sendFile(enc, socketFD, encSize);
//	sendMsg(enc, socketFD);

	memset(buffer, '\0', sizeof(buffer));
	sprintf(buffer, "%d", keySize);
	sendMsg(buffer, socketFD);
	temp = receiveMsg(buffer, socketFD);
	sendFile(key, socketFD, keySize);
//	sendMsg(key, socketFD);
	temp = receiveMsg(buffer, socketFD);

	free(enc);
	enc = (char*)malloc(encSize);
	receiveFile(enc, encSize, socketFD);
	sendMsg("0", socketFD);
	printf("ENC: %s\n", enc);


/*
	sendMsg("1", socketFD);
	receiveMsg(buffer, socketFD);
	printf("client: %s\n", buffer);

	sendMsg("2", socketFD);
	receiveMsg(buffer, socketFD);
	printf("client: %s\n", buffer);

	sendMsg("3", socketFD);
	receiveMsg(buffer, socketFD);
	printf("client: %s\n", buffer);
*/

	// // Get input message from user
	// printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	// memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	// fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	// buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
	//
	// // Send message to server
	// charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	// if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	// if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	//
	// // Get return message from server
	// memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	// charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	// if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	// printf("CLIENT: I received this from the server: \"%s\"\n", buffer);






	close(socketFD); // Close the socket
	return 0;
}
