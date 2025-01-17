#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"  // Application Protocol File Header.h

#ifdef WIN32
#include <winsock.h>
#else
     #include <unistd.h>
     #include <netdb.h>
     #include <sys/socket.h>
     #include <arpa/inet.h>
     #define closesocket close
#endif

// Function prototype
int Check(char text[]);
void ListToken(char *List[], char host[]);

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}
void ErrorHandler(char *errorMessage) {
	printf(errorMessage);
}


int main(int argc, char *args[]) {
// Initializing winsock with error prompt
#ifdef WIN32
	WSADATA wsa_data;
	int Warning = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (Warning != 0) {
		printf("Error at WSASturtup\n");
		return EXIT_FAILURE;
	}
#endif

	// Host settings
    struct hostent *host;
    char *host_name;

    // Resolve the host
    host = gethostbyname("passwdgen.uniba.it");
    if (host == NULL) {
        fprintf(stderr, "Error: cannot get hostname.\n");
        exit(EXIT_FAILURE);
    }
    host_name = host->h_name;

    printf("Connected to host: %s\n", host_name);

	// Creating welcome socket
	int my_socket;
	struct sockaddr_in echosad;
	int fromSize;
	struct sockaddr_in fromAddr;
	int eStringLen;
	int rStringLen;
	char echoString[ECHOMAX];
	char echoBuffer[ECHOMAX];
	while (Check(echoString) != 1) {
		// Commands for user
		printf(
				"\nInsert the type of password you want to generate:\n"
						"- h for help menu\n"
						"- n for numerical\n"
						"- a for alphabetical\n"
						"- m for mixed\n"
						"- s for secure\n"
						"- u for unambiguous\n"
						"After choosing the type of password select the length"
						"[Enter 'q' to quit the application].\n");
		printf("(password type) space (password length))\n");
		fgets(echoString, sizeof(echoString), stdin);
		echoString[strlen(echoString) - 1] = '\0';

		// Check for sentinel characters.
		if (Check(echoString) == 1) {
			printf("Closing client. . .\n");
			break;
		}else if(Check(echoString) == 2){ //inserted h
			printf("\nPassword Generator Help Menu\n"
					" Commands:\n"
					" h        : show this help menu\n"
					" n LENGTH : generate numeric password (digits only)\n"
					" a LENGTH : generate alphabetic password (lowercase letters)\n"
					" m LENGTH : generate mixed password (lowercase letters and numbers)\n"
					" s LENGTH : generate secure password (uppercase, lowercase, numbers, symbols)\n"
					" u LENGTH : generate unambiguous secure password (no similar-looking characters)\n"
					" q        : quit application\n"
					" LENGTH must be between 6 and 32 characters\n"
					" Ambiguous characters excluded in 'u' option:\n"
					"0 O o (zero and letters O)\n"
					" 1 l I i (one and letters l, I)\n"
					" 2 Z z (two and letter Z)\n"
					" 5 S s (five and letter S)\n"
					" 8 B (eight and letter B)\n");
		}else if( Check(echoString) == 0 ){
			printf("Data processing . . .\n");

			// Creating socket
			if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
				ErrorHandler("Error: socket creation failed.\n");
			}

			// Check if string length is too long
			if ((eStringLen = strlen(echoString)) > ECHOMAX) {
				ErrorHandler("Error: echo word is too long.\n");
			}

			// Build for Server
			memset(&echosad, 0, sizeof(echosad));
			echosad.sin_port = htons(PROTOPORT);
			echosad.sin_family = PF_INET;
			echosad.sin_addr.s_addr = inet_addr("127.0.0.1");

			// Check if input sent is equal input received.
			if (sendto(my_socket, echoString, eStringLen, 0, (struct sockaddr*) &echosad, sizeof(echosad))!= eStringLen) {
				ErrorHandler("Error: sent different number of bytes than expected.\n");
			}

			// Return echo string
			fromSize = sizeof(fromAddr);
			rStringLen = recvfrom(my_socket, echoBuffer, ECHOMAX, 0, (struct sockaddr*) &fromAddr, &fromSize);

			// If server address is different than before, the program prints error
			if (echosad.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
				fprintf(stderr, "Error: the packet received is from unknown source.\n");
				exit(EXIT_FAILURE);
			}
			echoBuffer[rStringLen] = '\0';

			printf("Received password from server %s, ip %s, client's requirements, server response: %s\n",
					host_name, inet_ntoa(fromAddr.sin_addr), echoBuffer);
		}

	}

	// Closing connection
	closesocket(my_socket);
	ClearWinSock();
	system("pause");
	return EXIT_SUCCESS;
}

// Function checks if string contains sentinel character
int Check(char text[]) {

	for (int increase = 0; increase < strlen(text); ++increase) {
				if (*(text + increase) == 'q') {
					return 1;
				}else if( *(text + increase) == 'h' ){
					return 2;
				}
			}

			return 0;
}

// Function is used for hostname
void ListToken(char * List[], char host[]) {
	int increase = 0;                 // Used for count
	const char separate[2] = ":";     // Separate hostname srv.di.uniba.it from port 56700
	char * token;                      // Single host's character

	token = strtok(host, separate);
	while (token != NULL) {           // Look for all token
		List[increase] = token;       // Put inside a list
		token = strtok(NULL, separate);
		increase++;                   // Increase count
	}

}

