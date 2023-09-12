#include <stdio.h>
#include <winsock2.h>

#define WINDOW_SIZE 10
#define MAX_DATA_SIZE 1024
#define PORT 5050
#define SERVER_IP "127.0.0.1"

typedef struct Frame {
    int sequence_num;
    int ack_num;
    int is_ack;
    char data[MAX_DATA_SIZE];
} Frame;

SOCKET client_socket;
int expected_sequence = 0;
int dropped_6 = 0, dropped_7 = 0, dropped_8 = 0;

void send_data_with_acknowledgment(int sequence_num) {
    printf("Sending acknowledgment for Frame %d\n", sequence_num);
    Frame ack_frame;
    ack_frame.is_ack = 1;
    ack_frame.ack_num = sequence_num;
    strcpy(ack_frame.data, "Piggybacked Data from Client");  // Static message in rela life implementation it contains info
    send(client_socket, (char*)&ack_frame, sizeof(ack_frame), 0);
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Could not create socket.\n");
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection failed.\n");
        return 1;
    }
    printf("Connected to server.\n");

    FILE *file = fopen("received_alice.txt", "w");
    if (!file) {
        printf("Failed to open the file.\n");
        return 1;
    }

    while (1) {
        Frame received_frame;
        int bytes_received = recv(client_socket, (char*)&received_frame, sizeof(received_frame), 0);

        if (bytes_received > 0) {
            printf("Received Frame %d\n", received_frame.sequence_num); //printf("Received Frame %d with data: %s\n", received_frame.sequence_num, received_frame.data);

            if (received_frame.sequence_num == 6 && !dropped_6) {
                printf("Simulating noise: Dropping Frame 6\n");
                dropped_6 = 1;
                continue;
            } else if (received_frame.sequence_num == 7 && !dropped_7) {
                printf("Simulating noise: Dropping Frame 7\n");
                dropped_7 = 1;
                continue;
            } else if (received_frame.sequence_num == 8 && !dropped_8) {
                printf("Simulating noise: Dropping Frame 8\n");
                dropped_8 = 1;
                continue;
            }

            if (received_frame.sequence_num == expected_sequence) {
                fprintf(file, "%s", received_frame.data); 
                send_data_with_acknowledgment(expected_sequence);
                expected_sequence = (expected_sequence + 1) % WINDOW_SIZE;
            } else {
                printf("Discarding Frame %d. Expected %d\n", received_frame.sequence_num, expected_sequence);
            }
        } else if (bytes_received == 0) {
            printf("Server closed connection.\n");
            break;
        } else {
            printf("Error receiving frame.\n");
            break;
        }
    }

    fclose(file);
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
