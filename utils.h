
#ifndef UTILS_H
#define UTILS_H

#define CHARSET			(26+1)
#define SPACE_CHAR	(CHARSET - 1)

#define ENCODE			1
#define DECODE			-1

// signatures for recognize encode or decode process
#define ENCODE_SIGN	0xE4C0DE
#define DECODE_SIGN	0xDEC0DE




// Error functions used for reporting issues
// fatal - prints message and exits(exit code 1)
void fatal(const char *msg);
// net_fatal - prints message apply to connection issues and exits(exit code 2)
void net_fatal(const char *msg, int port);
// error - only prints message
void error(const char *msg);

// readFile - Reads a string from the given text file
int readFile(const char *fn, char **s);

 // chr2int converts char to int A->0 B->1 ...
int chr2int(char c);
// int2chr converts int to char
char int2chr(int i);
// isValidLetter checks if c in range A-Z,space
int isValidLetter(char c);
// isValidStr checks if symbols of s in range A-Z,space
int isValidStr(const char *s);
// getRandLetter generates random symbol in range A-Z,space
char getRandLetter();

// sendStr sends the string to the socket
int sendStr(int sock, const char *s);
// recvStr receives the string from the socket
int recvStr(int sock, char **s);
// sendBuf sends the block of data to the socket
int sendBuf(int sock, const void *s, int size);
// recvBuf receives the block of data from the socket
int recvBuf(int sock, void *s, int size);

// oneTimePadCrypt - Encodes/decodes plain using 'key' parameters as a key
//   action must be ENCODE or DECODE constants
char *oneTimePadCrypt(const char *plain, const char *key, int action);

#endif
