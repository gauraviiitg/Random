#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5555
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

SOCKET clientSockets[MAX_CLIENTS] = {0};

DWORD WINAPI HandleClient(LPVOID clientSocketParam);

int main() {
    WSADATA wsa;
    SOCKET masterSocket;
    struct sockaddr_in server, client;
    int clientAddrSize = sizeof(client);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSA Initialization failed");
        return 1;
    }

    masterSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (masterSocket == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(masterSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(masterSocket);
        WSACleanup();
        return 1;
    }

    listen(masterSocket, MAX_CLIENTS);
    printf("Hub started on port %d\n", PORT);

    while (1) {
        SOCKET newSocket = accept(masterSocket, (struct sockaddr *)&client, &clientAddrSize);
        if (newSocket == INVALID_SOCKET) {
            perror("Accept failed");
            continue;
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] == 0) {
                clientSockets[i] = newSocket;
                CreateThread(NULL, 0, HandleClient, (LPVOID)newSocket, 0, NULL);
                printf("Node %d connected\n", i + 1);
                break;
            }
        }
    }
    closesocket(masterSocket);
    WSACleanup();
    return 0;
}

DWORD WINAPI HandleClient(LPVOID clientSocketParam) {
    SOCKET currentSocket = (SOCKET)clientSocketParam;
    char buffer[BUFFER_SIZE];

    while (1) {
        int recvSize = recv(currentSocket, buffer, BUFFER_SIZE, 0);
        if (recvSize <= 0) {
            printf("Node disconnected or error occurred\n");
            closesocket(currentSocket);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clientSockets[i] == currentSocket) {
                    clientSockets[i] = 0;
                    break;
                }
            }
            return 1;   
        }
        buffer[recvSize] = '\0';

        int destNode;
        char message[BUFFER_SIZE];
        sscanf(buffer, "%d:%s", &destNode, message);

        int senderID = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] == currentSocket) {
                senderID = i + 1;
                break;
            }
        }

        char modifiedMessage[BUFFER_SIZE];
        snprintf(modifiedMessage, sizeof(modifiedMessage), "%d:%d:%s", senderID, destNode, message);

        for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clientSockets[i] != 0) {
        send(clientSockets[i], modifiedMessage, strlen(modifiedMessage), 0);
    }
        }

    }
    closesocket(currentSocket);
    return 0;
}
