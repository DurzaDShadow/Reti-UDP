// Include the necessary headers
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>   // Added for strtol() and itoa
#include "protocol.h" // Application Protocol File Header.h

#ifdef WIN32
	#include <winsock.h>
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#define closesocket close
#endif

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}

void ErrorHandler(char *errorMessage) {
	printf(errorMessage);
}

// Function Prototype
void ClearAll(char Buffer[],char FirstNumber[],char SecondNumber[], char ResultString[], char text[], int passwordLength);
void ListToken(char Buffer[], const char separate[], char * List []);
void generatePassword(char *pw, int passwordLength, int b, int e);
void generateUnambiguousPassword(char *pw, int passwordLength, int b, int e);
void generateNumeric(char *pw, int passwordLength);
void generateAlpha(char *pw, int passwordLength);
void generateMixed(char *pw, int passwordLength);
void generateSecure(char *pw, int passwordLength);
void generateUnambiguous(char *pw, int passwordLength);


int main() {
	// Initializing winsock with error prompt
	#ifdef WIN32
		WSADATA wsa_data;
		int Warning = WSAStartup(MAKEWORD(2 ,2), &wsa_data);
		if (Warning != 0) {
			printf ("Error at WSAStartup\n");
			return EXIT_FAILURE;
		}
	#endif

	int my_socket;
	// Creating welcome socket
	if ((my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		ErrorHandler("Error: socket creation failed.\n");
	}

	// Build for Server
	struct sockaddr_in echosad;
	memset(&echosad, 0, sizeof(echosad));
	echosad.sin_port = htons(PROTOPORT);
	echosad.sin_family = AF_INET;
	echosad.sin_addr.s_addr = inet_addr("127.0.0.1");

	// This happens if server is active and it isn't shutdown
	if ((bind(my_socket, (struct sockaddr *)&echosad, sizeof(echosad))) < 0) {
		ErrorHandler("Error: binding failed.\n");
	}

	char eBuffer[ECHOMAX];
	int passwordLength;
	char choice;
	char sendString[100];

	// Separate used to space between two numbers.
	const char separate[2] = " "; // constant array variable used to hold the constant space to sweep the string
	// Array for strings
	char * List[3];
	char firstChar[2];
	char secondChar[5];
	char resultString[50];
	char pw[50];

	// Client
	struct sockaddr_in echocad;
	int cadLen;
	// Host
	struct hostent * client_host;

	// During loop, client sends the string and server carries out the request.
	while(1) {
		// ClearAll old values
		ClearAll(eBuffer, firstChar, secondChar, resultString, sendString, passwordLength);
		cadLen = sizeof(echocad);
		printf("Waiting for a request...\n");
		// Receive string from client
		recvfrom(my_socket, eBuffer, ECHOMAX, 0, (struct sockaddr*)&echocad, &cadLen);



		// Host Settings
		client_host = gethostbyaddr((char *) &(echocad.sin_addr), 4, AF_INET);


		if (client_host != NULL) {
			printf("New request from %s:%d\n", inet_ntoa(echocad.sin_addr), ntohs(echocad.sin_port));
		} else {
			printf("Client Host Name: Unknown\n");
		}

		// Function used to break the string
		ListToken(eBuffer, separate, List);

		// Conversion types
		choice = List[0][0];
		firstChar[0] = choice;
		passwordLength = strtol(List[1], NULL, 10);
		itoa(passwordLength, secondChar, 10);

		// Displayed Msg
		//printf("Client's password request '%c %d'\n", choice, passwordLength);

		if ((choice == 'n' || choice == 'a' || choice == 'm' || choice == 's' || choice == 'u') && (passwordLength > 5 && passwordLength < 33)) {
			switch (choice) {
				case 'n':
					generateNumeric(pw, passwordLength);
					break;
				case 'a':
					generateAlpha(pw, passwordLength);
					break;
				case 'm':
					generateMixed(pw, passwordLength);
					break;
				case 's':
					generateSecure(pw, passwordLength);
					break;
				case 'u':
					generateUnambiguous(pw, passwordLength);
					break;
				default:
					break;
			}

			//printf("Generated Password (): %s\n", pw);
			sprintf(sendString, "%s", pw);
		} else {
			printf("Enter a valid choice!");
			sprintf(sendString, "Enter a valid choice!");
		}

		strcat(resultString, firstChar);
		strcat(resultString, " ");
		strcat(resultString, secondChar);
		strcat(resultString, ": ");
		strcat(resultString, sendString);
		puts("");

		int CheckResultString = strlen(resultString);
		// Send result
		if (sendto(my_socket, resultString, CheckResultString, 0, (struct sockaddr *)&echocad, sizeof(echocad)) != CheckResultString) {
			ErrorHandler("Error: number of bytes sent is different than expected.\n");
		}
	}
}

// Function used to clear all variables and arrays
void ClearAll(char Buffer[], char FirstNumber[], char SecondNumber[], char ResultString[], char text[], int passwordLength) {
	for (int increase = 0; increase < 10; increase++) {
		Buffer[increase] = '\0';
	}

	for (int increase = 0; increase < strlen(FirstNumber); ++increase) {
		FirstNumber[increase] = '\0';
	}

	for (int increase = 0; increase < strlen(SecondNumber); increase++) {
		SecondNumber[increase] = '\0';
	}

	for (int increase = 0; increase < 50; ++increase) {
		ResultString[increase] = '\0';
	}

	for (int increase = 0; increase < strlen(text); ++increase) {
		text[increase] = '\0';
	}

	passwordLength = '\0';
}

// Function used to break string
void ListToken(char Buffer[], const char separate[], char * List []) {
	int increase = 0; // Used for count
	char * token; // Hold single character

	token = strtok(Buffer, separate);
	while (token != NULL) {
		List[increase] = token;
		token = strtok(NULL, separate);
		increase++;
	}
}

void generateNumeric(char *pw, int passwordLength) {
	int b = 53, e = 61;
	generatePassword(pw, passwordLength, b, e);
}

void generateAlpha(char *pw, int passwordLength) {
	int b = 27, e = 51;
	generatePassword(pw, passwordLength, b, e);
}

void generateMixed(char *pw, int passwordLength) {
	int b = 0, e = 62;
	generatePassword(pw, passwordLength, b, e);
}

void generateSecure(char *pw, int passwordLength) {
	int b = 0, e = 88;
	generatePassword(pw, passwordLength, b, e);
}

void generateUnambiguous(char *pw, int passwordLength) {
	int b = 0, e = 73;
	generateUnambiguousPassword(pw, passwordLength, b, e);
}

void generatePassword(char *pw, int passwordLength, int b, int e) {
	srand(time(NULL));
	char *charset =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+[]{}|;:,.<>?/~";
	for (int i = 0; i < passwordLength; i++) {
		pw[i] = charset[(rand() % (e - b + 1)) + b];
	}
	pw[passwordLength] = '\0';
}

void generateUnambiguousPassword(char *pw, int passwordLength, int b, int e) {
	srand(time(NULL));
	char *charset =
			"ACDEFGHJKLMNPQRTUVWXYabcdefghjkmnpqrtuvwxy34679!@#$%^&*()_+[]{}|;:,.<>?/~";
	for (int i = 0; i < passwordLength; i++) {
		pw[i] = charset[(rand() % (e - b + 1)) + b];
	}
	pw[passwordLength] = '\0';
}
