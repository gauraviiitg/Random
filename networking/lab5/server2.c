#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <windows.h>

#define WINDOW_SIZE 10
#define MAX_DATA_SIZE 1024
#define PORT 5050
#define TIMEOUT 3 // seconds

typedef struct Frame {
    int sequence_num;
    int ack_num;
    int is_ack;
    char data[MAX_DATA_SIZE];
} Frame;

SOCKET server_socket;

Frame create_frame(char* data, int is_ack, int sequence) {
    Frame frame;
    frame.sequence_num = sequence % WINDOW_SIZE;
    frame.is_ack = is_ack;
    strcpy(frame.data, data);
    return frame;
}

void send_frame(SOCKET client_sock, Frame frame) {
    printf("Sending Frame %d\n", frame.sequence_num); //  printf("Sending Frame %d with data: %s\n", frame.sequence_num, frame.data);
    send(client_sock, (char*)&frame, sizeof(frame), 0);
}

DWORD WINAPI handle_client(LPVOID client_socket) {
    SOCKET client_sock = (SOCKET)client_socket;
    int sequence = 0;
    unsigned long mode = 1;
    ioctlsocket(client_sock, FIONBIO, &mode); 

    char buffer[MAX_DATA_SIZE];
    FILE *file = fopen("alice.txt", "r");
    if (!file) {
        printf("Failed to open the file.\n");
        fflush(stdout);
        closesocket(client_sock);
        return 1;
    }

    while (fgets(buffer, MAX_DATA_SIZE, file) != NULL) {
        Frame frame = create_frame(buffer, 0, sequence);
        send_frame(client_sock, frame);
        
        Frame ack_frame;
        int bytes_received;
        time_t start_time = time(NULL);

        while (1) {
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(client_sock, &readSet);

            struct timeval timeout;
            timeout.tv_sec = TIMEOUT;
            timeout.tv_usec = 0;

            int result = select(0, &readSet, NULL, NULL, &timeout);
            if (result > 0) {
                bytes_received = recv(client_sock, (char*)&ack_frame, sizeof(ack_frame), 0);
                if (bytes_received == SOCKET_ERROR) {
                    printf("Error receiving acknowledgment: %d\n", WSAGetLastError());
                    fflush(stdout);
                } else if (ack_frame.is_ack) {
                    printf("Acknowledgment received for Frame %d with data: %s\n", ack_frame.ack_num, ack_frame.data);
                    fflush(stdout);
                    sequence++;
                    break;
                }
            } else if (result == 0) {
                printf("Timeout for Frame %d. Resending...\n", frame.sequence_num);
                fflush(stdout);
                send_frame(client_sock, frame);
                continue;
            } else {
                printf("Error in select: %d\n", WSAGetLastError());
                fflush(stdout);
            }
        }
    }

    fclose(file);
    closesocket(client_sock);
    return 0;
}


int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Could not create socket.\n");
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        return 1;
    }

    listen(server_socket, 3);
    printf("Server listening...\n");

    while (1) {
        struct sockaddr_in client;
        int client_size = sizeof(struct sockaddr_in);
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client, &client_size);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed with error code: %d\n", WSAGetLastError());
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        CreateThread(NULL, 0, handle_client, (LPVOID)client_socket, 0, NULL);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
