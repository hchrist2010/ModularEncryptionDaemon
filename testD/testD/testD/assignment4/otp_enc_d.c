//Name: Hunter Christiansen
//Email: christhu@oregonstate.edu
//UserID: 932211550
//Assignment4 otp_enc_d

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define packet 32

int x = 0;
void d(){
//	printf("%d\n", x);
	x++;
}

void error(const char *msg) { fprintf(stderr, msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[packet];
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
	while(1){
		//establish a connection to a client
		//if connection is established, create a new process to handle it and continue listening for new clients
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		if(establishedConnectionFD >= 0){
			int pid = fork();
			//Child process
			if(pid == 0){
				d();
				//ensure connecting to correct daemon by receiving the typs of connection the program is looking for
				//if client sent "otp_enc" then return a 0 indicating it has connected to the correct daemon,
				//otherwise send back 1 indicating an error and close the connection
				charsRead = recv(establishedConnectionFD, buffer, (packet - 1), 0);
				if (charsRead < 0) error("ERROR reading from socket");
				if(strcmp(buffer, "otp_enc") == 0){
					charsRead = send(establishedConnectionFD, "0", strlen("0"), 0);
				}
				else{
					charsRead = send(establishedConnectionFD, "1", strlen("1"), 0);
					close(establishedConnectionFD);
					close(listenSocketFD);
					fprintf(stderr, "Attempted to connect to wrong daemon using port %d\n", portNumber);
					exit(2);
				}
				d();
				//receive the size of the incoming file then store it in the integer fileLength for later
				//if the length is greater than 0, send back '0' indicating everything worked
				//otherwise, send back '1' to indicate some sort of an issue, then close the socket and exit process
				memset(buffer, '\0', packet);
				charsRead = recv(establishedConnectionFD, buffer, (packet - 1), 0); // Read the client's message from the socket
				int fileLength = atoi(buffer);
				if(fileLength > 0){
					charsRead = send(establishedConnectionFD, "0", strlen("0"), 0);
				}
				else{
					charsRead = send(establishedConnectionFD, "1", strlen("1"), 0);
					close(establishedConnectionFD);
					close(listenSocketFD);
					fprintf(stderr, "File not valid\n");
					exit(1);
				}

				d();
				//allocate enough space for the incoming string in enc
				//reveive the file in packets of (packet - 1), continue to reveive until the full file has been received
				//store the file in the enc string and then send back a 0 indicating the string has been received
				int counter = 0;
				char *enc;
				enc = (char*)malloc(fileLength + packet);
				while(counter < fileLength - 1){
					memset(buffer, '\0', packet);
					charsRead = recv(establishedConnectionFD, buffer, (packet - 1), 0); // Read the client's message from the socket
					counter += charsRead;
					strncat(enc, buffer, (packet - 1));
				}
				charsRead = send(establishedConnectionFD, "0", strlen("0"), 0);

				d();
				//receive the size of the incoming key file, then store it in the integer keyLength for later
				//if the length is greater than 0, send back '0' indicating everything worked
				//otherwise, send back '1' to indicate some sort of issue occured, then close the socet and exit process
				char *key;
				int keyLength;
				memset(buffer, '\0', packet);
				charsRead = recv(establishedConnectionFD, buffer, (packet - 1), 0); // Read the client's message from the socket
				keyLength = atoi(buffer);
				if(keyLength > 0){
					charsRead = send(establishedConnectionFD, "0", strlen("0"), 0);
				}
				else{
					charsRead = send(establishedConnectionFD, "1", strlen("1"), 0);
					close(establishedConnectionFD);
					close(listenSocketFD);
					fprintf(stderr, "File not valid\n");
					exit(1);
				}
				d();
				//allocate enough space for the incoming key in key
				//receive the key in packets of (packet - 1), continue to receive until the full key has been received
				//store the key in teh key string then send back 0 indicating the key has been fully received
				counter = 0;
				key = (char*)malloc(keyLength + packet);
				while(counter < keyLength - 1){
					memset(buffer, '\0', packet);
					charsRead = recv(establishedConnectionFD, buffer, (packet - 1), 0); // Read the client's message from the socket
					counter += charsRead;
					strncat(key, buffer, (packet - 1));
				}
				d();
				//first check that the key length is greater than the file length, if not, send a '1' to the client indicating the error
				//then loop through the file changing any ' ' to '[', if anything is in the file aside form capital letters or a space
				//send '2' back to the client to indicate the error
				//lastly, loop through the key and change any ' ' to '['
				//when this is done, send a 0 to the client to indicate that everything is in place for encryption
				if(fileLength > keyLength){
					charsRead = send(establishedConnectionFD, "1", strlen("1"), 0);
					exit(1);
				}
				int i;
				int temp;
				for(i = 0; i < fileLength-1; i++){
					if(enc[i] == ' '){
						enc[i] = '[';
					}
					if(enc[i] < 65 || enc[i] > 91){
						charsRead = send(establishedConnectionFD, "2", strlen("2"), 0);
						exit(1);
					}
				}
				for(i = 0; i < keyLength - 1; i++){
					if(key[i] == ' '){
						key[i] = '[';
					}
				}
				charsRead = send(establishedConnectionFD, "0", strlen("0"), 0);

				d();
				//encrypt the file by subtracting 65 from the ascii value, this will produce a number between 0 and 26
				//add the ascii value of the key to each index of the enc string
				//then modulo 27, then add 65 to the resulting value to place the string within the bounds of capital letters and '['
				//lastly, change any '[' to ' ' before sending the encrypted string back to the client
				for(i = 0; i < fileLength - 1; i++){
					enc[i] -= 65;
					temp = (int)enc[i] + (int)key[i];
					temp = temp % 27;
					enc[i] = (char)(temp + 65);
				}
				for(i = 0; i < fileLength-1; i++){
					if(enc[i] == '['){
						enc[i] = ' ';
					}
				}
				d();
				//send the encrypted string back to the client, when client receives the full string, it will send a 0 back
				//after that, dealocate any memory and close the socket
				charsRead = send(establishedConnectionFD, enc, strlen(enc), 0); // Write to the server
				memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
				charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
				if (charsRead < 0) error("CLIENT: ERROR writing to socket");

				d();
//				free(enc);
//				free(key);
				close(establishedConnectionFD); // Close the existing socket which is connected to the client
				close(listenSocketFD); // Close the listening socket
				exit(0);
			}

			else{
				//parent process will check if any child processes have finished and will close them if they have
				int returnStatus;
				waitpid(pid, &returnStatus, 0);
				if(WIFEXITED(returnStatus)){
					WEXITSTATUS(returnStatus);
				}
			}
		}
	}

	//There is no way to reach the end of the daemon, but if that were to somehow occcur, this will close any sockets that have been created before exit
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	close(listenSocketFD); // Close the listening socket
	return 0;
}
