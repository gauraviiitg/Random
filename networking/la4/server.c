#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5555
#define FRAME_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket; 
    struct sockaddr_in server, client;
    char buffer[FRAME_SIZE], ack_buffer[4];
    int c, bytes_read;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize. Error code: %d", WSAGetLastError());
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error code: %d", WSAGetLastError());
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed. Error code: %d", WSAGetLastError());
        closesocket(server_socket);
        return 1;
    }

    listen(server_socket, 5);
    printf("Waiting for client connections...\n");

    c = sizeof(struct sockaddr_in);
    client_socket = accept(server_socket, (struct sockaddr*)&client, &c);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed. Error code: %d", WSAGetLastError());
        closesocket(server_socket);
        return 1;
    }

    FILE *file = fopen("alice.txt", "r");
    if (!file) {
        printf("Failed to open alice.txt for reading.\n");
        closesocket(client_socket);
        closesocket(server_socket);
        return 1;
    }

    while (1) {
        memset(buffer, 0, FRAME_SIZE);
        bytes_read = fread(buffer, 1, FRAME_SIZE, file);
        
        if (bytes_read > 0) {
            send(client_socket, buffer, bytes_read, 0);
            printf("Sent frame of %d bytes.\n", bytes_read);
            recv(client_socket, ack_buffer, sizeof(ack_buffer), 0);
            printf("Received ACK.\n");
        }

        if (bytes_read < FRAME_SIZE) {
            break;
        }
    }

    fclose(file);
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
