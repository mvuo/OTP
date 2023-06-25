
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h> // send(),recv()


// Error functions used for reporting issues
// error - only prints message
void error(const char *msg) {
  if (errno) perror(msg);
  else fprintf(stderr, "%s\n", msg);
} 

// fatal - prints message and exits(exit code 1)
void fatal(const char *msg) {
  error(msg);
  exit(1);
} 

// net_fatal - prints message apply to connection issues and exits(exit code 2)
void net_fatal(const char *msg, int port) {
	fprintf(stderr, "%s%d\n", msg, port);
  exit(2);
} 

// readFile - Reads a string from the given text file
int readFile(const char *fn, char **s)
{
	// Opens file
	FILE *fp = fopen(fn, "r");
	if (!fp) {
		fatal(fn);
	}

  // Determines size of file
  fseek(fp, 0, SEEK_END);
  long int fsize = ftell(fp);
  if (fsize <= 0) {
		fatal("File size equals 0");
  }
  // Allocates the string
  *s = malloc((size_t)fsize+1);

	// Reads file, trunc to buffer - 1 chars, leaving \0
  fseek(fp, 0, SEEK_SET);
  fgets(*s, fsize, fp);
  // Removes the trailing \n that fgets adds
  (*s)[strcspn(*s, "\n")] = '\0'; 

  // Closes the file
	fclose(fp);
  return 1;
}

 // chr2int converts char to int A->0 B->1 ...
int chr2int(char c)
{
	if (c == ' ') return SPACE_CHAR;
	if ((c >= 'A') && (c <= 'Z')) return c - 'A';
	return -1;
}

// int2chr converts int to char
char int2chr(int i)
{
	if ((i < 0) || (i >= CHARSET)) return 0;
	if (i == SPACE_CHAR) return ' ';
	return 'A' + i;
}

// isValidLetter checks if c in range A-Z,space
int isValidLetter(char c)
{
	if (c == ' ') return 1;
	if ((c >= 'A') && (c <= 'Z')) return 1;
	return 0;
}

// isValidStr checks if symbols of s in range A-Z,space
int isValidStr(const char *s)
{
	while(*s) {
		if (!isValidLetter(*s)) return 0;
		s++;
	}
	return 1;
}

// getRandLetter generates random symbol in range A-Z,space
char getRandLetter()
{
	int c = rand() % CHARSET;
	if (c == SPACE_CHAR) return ' ';
	return 'A' + c;
}

// sendStr sends the string to the socket
int sendStr(int sock, const char *s)
{
	int len = strlen(s);
	if (!sendBuf(sock, &len, sizeof len)) return 0;
	if (!sendBuf(sock, s, len)) return 0;
	return 1;
}

// recvStr receives the string from the socket
int recvStr(int sock, char **s)
{
	int len = 0;
	if (!recvBuf(sock, &len, sizeof len)) return 0;
	//fprintf(stderr, "recv len: %d\n", len);
	*s = malloc(len+1);
	if (!recvBuf(sock, *s, len)) return 0;
	(*s)[len] = 0;
	return 1;
}

// sendBuf sends the block of data to the socket
int sendBuf(int sock, const void *s, int size)
{
	char *p = (char *)s;
	ssize_t to_send = size;
	while(to_send > 0) {
		// Send data to the socket
		errno = 0;
		ssize_t sent = send(sock, &p[size - to_send], to_send, 0);
		if (sent < 0){
			if (errno == EINTR) {
				errno = 0;
				continue;
			}
			error("CLIENT: ERROR writing to the socket");
			return 0;
		}
		to_send -= sent;
  }
	return 1;
}

// recvBuf receives the block of data from the socket
int recvBuf(int sock, void *s, int size)
{
	char *p = (char *)s;
	ssize_t to_read = size;
	while(to_read > 0) {
		// Read data from the socket
		errno = 0;
		ssize_t rdd = recv(sock, &p[size - to_read], to_read, 0);
		if (!rdd) return 0;
		if (rdd < 0){
			if (errno == EINTR) {
				errno = 0;
				continue;
			}
			error("CLIENT: ERROR reading from the socket");
			return 0;
		}
		to_read -= rdd;
  }
	return 1;
}

// oneTimePadCrypt - Encodes/decodes plain using 'key' parameters as a key
//   action must be ENCODE or DECODE constants
char *oneTimePadCrypt(const char *plain, const char *key, int action)
{
	int len = strlen(plain);
	int keylen = strlen(key);
	if (keylen < len) {
		error("Error: key is too short");
		return 0;
	}
	char *cipher = malloc(len+1);
	char *p = cipher;
	for(int i=0;i < len;i++) {
		if (!isValidLetter(*plain)) {
			error("wrong letter in the plain text");
			free(cipher);
			return 0;
		}
		if (!isValidLetter(*key)) {
			error("wrong letter in the key");
			free(cipher);
			return 0;
		}
		*p = int2chr((chr2int(*plain) + chr2int(*key)*action + CHARSET) % CHARSET);
		plain++;
		key++;
		p++;
	}
	
	*p = 0;
	return cipher;
}


