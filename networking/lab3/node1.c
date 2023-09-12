#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5555
#define BUFFER_SIZE 1024
#define MAX_STORED_MESSAGES 100

char storedMessages[MAX_STORED_MESSAGES][BUFFER_SIZE];
int storedMessageCount = 0;

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

    printf("Node %d connected to hub\n", nodeID);

    while (1) {
        printf("\nNode %d, choose an option:\n", nodeID);
        printf("1. Send message\n");
        printf("2. See received message\n");
        printf("3. View discarded messages\n");
        printf("4. Exit\n");
        
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
                if (recvSize <= 0) {
                    printf("Failed to receive or connection closed.\n");
                    continue;
                }
                buffer[recvSize] = '\0';
                printf("Debug: Full received message: %s\n", buffer);

                int senderID, destID;
                char msgContent[BUFFER_SIZE];

                sscanf(buffer, "%d:%d:%s", &senderID, &destID, msgContent);
                printf("Debug: Parsed IDs - Sender: %d, Dest: %d, Content: %s\n", senderID, destID, msgContent);

                if (destID == nodeID) {
                    printf("Processed message from Node %d: %s\n", senderID, msgContent);
                } else {
                    printf("Discarded message (intended for Node %d, from Node %d)\n", destID, senderID);
                    if(storedMessageCount < MAX_STORED_MESSAGES) {
                        strncpy(storedMessages[storedMessageCount], buffer, BUFFER_SIZE);
                        storedMessageCount++;
                        printf("Debug: Storing discarded message.\n");
                    } else {
                        printf("Warning: Message storage is full. \n");
                    }
                }
                break;
            }
            case 3: {
                printf("Stored (Discarded) Messages:\n");
                printf("Debug: Current discarded messages list:\n");
for (int i = 0; i < storedMessageCount; i++) {
    printf("%s\n", storedMessages[i]);
}

                for (int i = 0; i < storedMessageCount; i++) {
                    printf("%s\n", storedMessages[i]);
                }
                break;
            }
            case 4:
                closesocket(clientSocket);
                WSACleanup();
                return 0;
            default:
                printf("Invalid option. Please choose a valid option.\n");
                break;
        }
    }
}
