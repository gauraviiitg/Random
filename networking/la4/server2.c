#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5555
#define FRAME_SIZE 512

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    char buffer[FRAME_SIZE], ack_buffer[4];
    int c, bytes_read;

    fd_set readfds;
    struct timeval tv;
    int retval;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize. Error code: %d", WSAGetLastError());
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
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

    FILE *file = fopen("alc.txt", "r");
    if (!file) {
        printf("Failed to open alc.txt for reading.\n");
        return 1;
    }

    while ((bytes_read = fread(buffer, 1, FRAME_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
        printf("Sent frame.\n");

        FD_ZERO(&readfds);
        FD_SET(client_socket, &readfds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(client_socket + 1, &readfds, NULL, NULL, &tv);

        if (retval == -1) {
            perror("select()");
            break;
        } else if (retval) {
            recv(client_socket, ack_buffer, sizeof(ack_buffer), 0);
            printf("Received ACK.\n");
        } else {
            printf("ACK not received. Resending frame...\n");
            continue;
        }
    }

    fclose(file);
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
