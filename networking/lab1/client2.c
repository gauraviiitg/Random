#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

//#pragma comment(lib, "ws2_32.lib")

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server IP> <port>\n", argv[0]);
        return 1;
    }

    char *serverIP = argv[1];
    int port = atoi(argv[2]);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return 1;
    }

    // Create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        perror("socket creation failed");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("connection failed");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server\n");

    char buffer[1024];
    int bytesRead;

    while (1) {
        printf("Please enter the message to the server: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Remove newline character
        buffer[strcspn(buffer, "\n")] = '\0';

        // Send input to server
        send(clientSocket, buffer, strlen(buffer), 0);

        // Receive and display server's reply
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            printf("Server reply: %s\n", buffer);
        }
    }

    // Close socket
    closesocket(clientSocket);

    WSACleanup();

    return 0;
}
