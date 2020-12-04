#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { fprintf(stderr, msg); exit(0); } // Error function used for reporting issues

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


	//ensure connecting to correct daemon by sending the name of this program to the server
	//server will verify it is the correct program and send a response back of either 0 or 1
	//0 being correct and 1 indicating a connection to the wrong daemon
	charsWritten = send(socketFD, "otp_dec", strlen("otp_dec"), 0);
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if(strcmp(buffer, "1") == 0){
		close(socketFD);
		exit(2);
	}

	//Variable declarations for sending the key and file to be decoded
	FILE *fp;
	FILE *keyFile;
	char ch;
	char *enc;
	char *key;
	int fileLength;
	int keyLength;
	int count = 0;


	//get input file length by counting the number of characters in the file
	//then store that count for later
	//allocate enough space in the enc string for the file
	//and finally, copy the file into enc string
	fp = fopen(argv[1], "r");
	while((ch = fgetc(fp)) != EOF){
		count++;
	}
	fileLength = count;
	enc = (char*)malloc(count);
	fclose(fp);
	fp = fopen(argv[1], "r");
	if(!fp){
		error("Failed to open file\n");
	}
	fgets(enc, fileLength, fp);
	fclose(fp);


	//print the length of the string to buffer so it can be sent to the server
	//send that length and then wait for a response
	//if response is 0, that means the fileLength sent was > 0
	//otherwise, it means there was some sort of error or the file does not exist
	sprintf(buffer, "%d", fileLength);
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if(strcmp(buffer, "1") == 0){
		close(socketFD);
		exit(2);
	}


	//send file to server
	charsWritten = send(socketFD, enc, strlen(enc), 0); // Write to the server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(enc)) printf("CLIENT: WARNING: Not all data written to socket!\n");


	//get the length of the key by counting all the chars in the file
	//Then allocate enough space in the key string to hold key
	//finally, copy the key from the keyFile to the key string
	keyFile = fopen(argv[2], "r");
	if(!keyFile){
		error("Failed to open file\n");
	}
	count = 0;
	while((ch = fgetc(keyFile)) != EOF){
//		printf("example\n");
		count++;
	}
	keyLength = count;
	key = (char*)malloc(count);
	fclose(keyFile);
	keyFile = fopen(argv[2], "r");
	fgets(key, keyLength, keyFile);
	fclose(keyFile);


	//send the string length to server
	//server will send back a 1 if the size is < 1, otherwise the server will send back 0
	sprintf(buffer, "%d", keyLength);
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if(strcmp(buffer, "1") == 0){
		close(socketFD);
		exit(2);
	}


	//send key to server
	charsWritten = send(socketFD, key, strlen(key), 0); // Write to the server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(key)) printf("CLIENT: WARNING: Not all data written to socket!\n");


	//The server will report back after obtaining the key
	//if the server sends a 1, then the file is longer than the key
	//if the server sends a 2, then the file sent has characters outside the range of this program
	//if the server sends a 0 then everything is good to go
	if(strcmp(buffer, "1") == 0){
		error("File longer than key\n");
		exit(1);
	}

        if(strcmp(buffer, "2") == 0){
                error("Bad Input\n");
                exit(1);
        }



	//After the server has encrypted the file, it will send it back to this process to be printed out
	//A while loop is used to ensure the entire file is received
	//the file will be received in packets of 255 bytes
	memset(enc, '\0', sizeof(enc));
	int counter = 0;
	while(counter < fileLength - 1){
		memset(buffer, '\0', 256);
		charsRead = recv(socketFD, buffer, 255, 0); // Read the client's message from the socket
		counter += charsRead;
		strncat(enc, buffer, 255);
	}
	charsRead = send(socketFD, "0", strlen("0"), 0);

	printf("%s\n", enc);

	//free the memory allocated and close the socket
	free(key);
	free(enc);
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	close(socketFD); // Close the socket
	return 0;
}
