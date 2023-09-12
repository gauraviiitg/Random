#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 5555
#define SERVER_ADDR "127.0.0.1"
#define FRAME_SIZE 512

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server;
    char buffer[FRAME_SIZE];
    int bytes_received;
    int frame_count = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize. Error code: %d", WSAGetLastError());
        return 1;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Could not create socket. Error code: %d", WSAGetLastError());
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection failed. Error code: %d", WSAGetLastError());
        closesocket(client_socket);
        return 1;
    }

    FILE* file = fopen("rec.txt", "w");
    if (!file) {
        printf("Failed to open rec.txt for writing.\n");
        return 1;
    }

    while (1) {
        bytes_received = recv(client_socket, buffer, FRAME_SIZE, 0);
        if (bytes_received <= 0) {
            break;  // Exit the loop when there's no more data
        }

        if (frame_count == 6 || frame_count == 7) {
            printf("Simulating lost ACK for frame %d.\n", frame_count);
            Sleep(6000);
            continue;  // Skip sending ACK for lost frames
        }

        fwrite(buffer, 1, bytes_received, file);
        printf("Received frame.\n");
        frame_count++;

        send(client_socket, "ACK", 3, 0);
        printf("Sent ACK.\n");
    }

    fclose(file);
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
