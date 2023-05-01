#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7 //select minimal legacy support, needed for inet_pton, inet_ntop
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
#else
#include <sys/socket.h> //for sockaddr, socket, socket
#include <sys/types.h> //for size_t
#include <netdb.h> //for getaddrinfo
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
#include <errno.h> //for errno
#include <stdio.h> //for fprintf, perror
#include <unistd.h> //for close
#include <stdlib.h> //for exit
#include <string.h> //for memset
#endif
#define PORT 8080
int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create TCP socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return -1;
    }

    // Specify server address and port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        return -1;
    }

    // Send random operation requests to server
    char operation[3];
    int num1, num2, result;
    for (int i = 0; i < 5; i++) {
        // Generate random operation
        num1 = rand() % 100;
        num2 = rand() % 100;
        switch (rand() % 4) {
            case 0:
                sprintf(operation, "%c", '+');
                result = num1 + num2;
                break;
            case 1:
                sprintf(operation, "%c", '-');
                result = num1 - num2;
                break;
            case 2:
                sprintf(operation, "%c", '*');
                result = num1 * num2;
                break;
            case 3:
                sprintf(operation, "%c", '/');
                result = num1 / num2;
                break;
        }

        // Send request to server
        char request[20];
        sprintf(request, "%d %s %d", num1, operation, num2);
        send(sock, request, strlen(request), 0);
        printf("Request sent: %s\n", request);

        // Receive and check response from server
        valread = read(sock, buffer, 1024);
        buffer[valread] = '\0';
        printf("Server response: %s\n", buffer);
        if (result == atoi(buffer)) {
            printf("Result is correct!\n");
        } else {
            printf("Result is incorrect.\n");
        }
    }

    // Send "STOP" request to server
    send(sock, "STOP", strlen("STOP"), 0);
    printf("Request sent: STOP\n");

    // Receive "OK" response from server
    valread = read(sock, buffer, 1024);
    buffer[valread] = '\0';
    printf("Server response: %s\n", buffer);

    // Send "KTNXBYE" request to server and close socket
    send(sock, "KTNXBYE", strlen("KTNXBYE"), 0);
    printf("Request sent: KTNXBYE\n");
    close(sock);

    return 0;
}