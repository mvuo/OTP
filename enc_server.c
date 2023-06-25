
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include "utils.c"

//#define SERVER_DEBUG
#define MAX_CHILDS		5


// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
  int connectionSocket;
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    fatal("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Enables "reuse" for the socket
  int on = 1;
  setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    fatal("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
	int childCounter = 0;
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }
    else {

#ifdef SERVER_DEBUG
			// Prints client connection info
			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(clientAddress.sin_addr), ip, INET_ADDRSTRLEN);
			printf("SERVER: Connected to client running at host %s port %d\n", 
                          ip,
                          ntohs(clientAddress.sin_port));
#endif

      // Fork a new process to handle the client request
      if (fork() == 0)
      {
				// Close unused listening socket
        close(listenSocket);
			
				// Checks valid signature of client
				int encode = ENCODE_SIGN;
				// Sends own signature
				if (!sendBuf(connectionSocket, &encode, sizeof encode)) {
					error("Error: Can't send the signature to the client");
				}
				else {
					int sign = 0;
					// Receives client signature
					if (!recvBuf(connectionSocket, &sign, sizeof sign)) {
						error("Error: Can't receive the signature from the client");
					}
					else {
						// Checks client signature
						if (sign != ENCODE_SIGN) {
							error("Error: wrong client type\n");
						}
						else {
							char *plain = 0;
							char *key = 0;
							// Receives the plain text from the client
							if (!recvStr(connectionSocket, &plain)){
								error("Error: Can't receive the plain text");
							}
							else {
								// Receives the key from the client
								if (!recvStr(connectionSocket, &key)) {
									error("Error: Can't receive the key");
								}
								else {
									// Applies the key to the plain text
									char *cipher = oneTimePadCrypt(plain, key, ENCODE);
									if (cipher) {
										// Sends the ciphertext to the client
										if (!sendStr(connectionSocket, cipher)) {
											error("Error: Can't send the cipher text to the client");
										}
										free(cipher);
									}
									free(key);
								}
								free(plain);
							}
							//sendStr(connectionSocket, "ALL OK");
						}
					}
				}
				// Close the connection socket for this client
				close(connectionSocket); 
				exit(0);
			}
			// Close the connection socket for this client
			close(connectionSocket); 
			// Increments number of the childs
			childCounter++;
			// Clean up the zombies
			// Limits number of the childs to MAX_CHILDS
			while(childCounter >= MAX_CHILDS) {
				int status;
				// Waits any child changes
				int ret = waitpid(-1, &status, 0);
				if (ret > 0) {
					// child exited
					if (WIFEXITED(status)) {
						// Decrements number of the childs to continue accepting connections
						childCounter--;
					}
				}
			}
			// Continue accepting connections
		}
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}

