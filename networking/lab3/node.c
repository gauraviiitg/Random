#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5555
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET clientSocket;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    if (argc != 2) {
        printf("Usage: %s <node_id>\n", argv[0]);
        return 1;
    }

    int nodeID = atoi(argv[1]);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSA Initialization failed");
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    
    struct timeval tv;
    tv.tv_sec = 5;  // 5 seconds timeout
    tv.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    printf("Node %d connected to hub\n", nodeID);

    while (1) {
        printf("\nNode %d, choose an option:\n", nodeID);
        printf("1. Send message\n");
        printf("2. See received message\n");
        printf("3. Exit\n");
        
        int option;
        scanf("%d", &option);
        getchar();

        switch (option) {
            case 1: {
                printf("Enter destination node ID (1-5): ");
                int destNode;
                scanf("%d", &destNode);
                getchar();

                printf("Enter your message: ");
                fgets(buffer, BUFFER_SIZE, stdin);
                buffer[strlen(buffer) - 1] = '\0';

                char fullMessage[BUFFER_SIZE];
                snprintf(fullMessage, sizeof(fullMessage), "%d:%s", destNode, buffer);
                send(clientSocket, fullMessage, strlen(fullMessage), 0);
                break;
            }
            case 2: {
                int recvSize = recv(clientSocket, buffer, BUFFER_SIZE, 0);
                if (recvSize == SOCKET_ERROR) {
                    if (WSAGetLastError() == WSAETIMEDOUT) {
                        printf("No message received within the timeout period.\n");
                        continue;
                    } else {
                        perror("Error receiving data");
                        break;
                    }
                }
                buffer[recvSize] = '\0';

                int senderID, destID;
                char msgContent[BUFFER_SIZE];
                sscanf(buffer, "%d:%d:%s", &senderID, &destID, msgContent);

                if (destID == nodeID) {
                    printf("Received message from Node %d: %s\n", senderID, msgContent);
                } else {
                    printf("Discarded message (intended for Node %d, from Node %d): %s\n", destID, senderID, msgContent);
                }
                break;
            }
            case 3:
                closesocket(clientSocket);
                WSACleanup();
                return 0;
            default:
                printf("Invalid option. Please choose a valid option.\n");
                break;
        }
    }
}
