#include <stdio.h>
#include <winsock2.h>
#include <time.h>

#define WINDOW_SIZE 9
#define MAX_DATA_SIZE 1024
#define PORT 5050
#define TIMEOUT 3 

typedef struct Frame {
    int sequence_num;
    char data[MAX_DATA_SIZE];
} Frame;

SOCKET server_socket, client_socket;
int sequence = 0; 

Frame create_frame(char* data) {
    Frame frame;
    frame.sequence_num = sequence % WINDOW_SIZE;
    strcpy(frame.data, data);
    return frame;
}

void send_frame(Frame frame) {
    printf("Sending Frame %d\n", frame.sequence_num);
    send(client_socket, (char*)&frame, sizeof(frame), 0);
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

    struct sockaddr_in client;
    int client_size = sizeof(struct sockaddr_in);
    client_socket = accept(server_socket, (struct sockaddr*)&client, &client_size);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Connection accepted.\n");

    char buffer[MAX_DATA_SIZE];
    FILE *file = fopen("alice.txt", "r");
    if (!file) {
        printf("Failed to open the file.\n");
        return 1;
    }

    while (fgets(buffer, MAX_DATA_SIZE, file) != NULL) {
        Frame frame = create_frame(buffer);
        send_frame(frame);
        sequence++;

        char ack_buffer[10];
int bytes_received;
time_t start_time = time(NULL);

while (1) {
    bytes_received = recv(client_socket, ack_buffer, sizeof(ack_buffer), 0);
    ack_buffer[bytes_received] = '\0'; 
    
    if (strcmp(ack_buffer, "DROP") == 0) {
        printf("Frame %d was dropped by the client. Resending...\n", frame.sequence_num);
        send_frame(frame); 
        continue;
    }

    int ack = atoi(ack_buffer);
    if (ack == frame.sequence_num) {
        printf("Acknowledgment received for Frame %s\n", ack_buffer);
        break;
    }

    if (time(NULL) - start_time >= TIMEOUT) {
        printf("Timeout for Frame %d. Going back %d frames and resending...\n", frame.sequence_num, WINDOW_SIZE);
        fseek(file, -WINDOW_SIZE * MAX_DATA_SIZE, SEEK_CUR); 
        sequence -= WINDOW_SIZE; 
        break; 
    }
}
    }

    fclose(file);
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
