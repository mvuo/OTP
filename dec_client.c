
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

#include "utils.c"

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD;
  struct sockaddr_in serverAddress;
  // Check usage & args
  if (argc < 4) { 
    fprintf(stderr,"USAGE: %s ciphertext key port\n", argv[0]); 
    exit(0); 
  } 

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    fatal("CLIENT: ERROR opening socket");
  }

  // Set up the server address struct
	int port = atoi(argv[3]);
  setupAddressStruct(&serverAddress, port, "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    net_fatal("Error: could not contact dec_server on port ", port);
  }

  // Checks valid signature of the server
  int decode = DECODE_SIGN;
	// Sends own signature
  if (!sendBuf(socketFD, &decode, sizeof decode)) {
    net_fatal("Error: Can't send the signature to the decode server, port ", port);
	}

  int sign = 0;
	// Receives server signature
  if (!recvBuf(socketFD, &sign, sizeof sign)) {
    net_fatal("Error: Can't receive the signature from the decode server, port ", port);
	}
	// Checks server signature
  if (sign != DECODE_SIGN) {
    net_fatal("Error: wrong server type, port ", port);
  }

  char *ciphertext = 0;
	// Reads the ciphertext from given file
	if (!readFile(argv[1], &ciphertext)) {
    fatal("Error: Can't read the ciphertext");
	}
  // Checks the ciphertext symbols
  if (!isValidStr(ciphertext)) {
		fprintf(stderr,"%s error: input contains bad characters\n", argv[0]); 
		exit(1);
	}
    
  char *key = 0;
	// Reads the key from given file
	if (!readFile(argv[2], &key)) {
    fatal("Error: Can't read the key");
	}
  // Checks the key length
  if (strlen(key) < strlen(ciphertext)) {
		fprintf(stderr,"Error: key '%s' is too short\n", argv[2]); 
		exit(1);
	}
  // Checks the key symbols
  if (!isValidStr(key)) {
		fprintf(stderr,"%s error: key contains bad characters\n", argv[0]); 
		exit(1);
	}
	// Sends the cipher text to the server
  if (!sendStr(socketFD, ciphertext)){
    net_fatal("Error: Can't send the cipher text, port ", port);
	}
	// Sends the key to the server
	if (!sendStr(socketFD, key)) {
    net_fatal("Error: Can't send the key, port ", port);
	}
	free(key);
	free(ciphertext);
	
  char *plain = 0;
	// Receives the plain text from the server
  if (!recvStr(socketFD, &plain)) {
    net_fatal("Error: No answer from the decode server, port ", port);
	}
	// Prints plain text to STDOUT
  printf("%s\n", plain);
	free(plain);

  // Close the socket
  close(socketFD); 
  return 0;
}
