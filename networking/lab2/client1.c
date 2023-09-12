#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define CHUNK_SIZE 5000

int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    FILE *fp;
    char buffer[CHUNK_SIZE + 1];
    char ack_msg[4];

    WSAStartup(MAKEWORD(2,2),&wsa);

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

    connect(s, (struct sockaddr *)&server, sizeof(server));
    puts("Connected to server");

    fp = fopen("alice.txt", "rb");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    int bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, fp)) > 0) {
        send(s, buffer, bytes_read, 0);
        puts("Data sent, waiting for ACK...");

        if (recv(s, ack_msg, 3, 0) <= 0) {
            puts("Error: ACK not received or connection closed prematurely");
            break;
        }
        
        ack_msg[3] = '\0';  
        printf("Received: %s\n", ack_msg);

        if (strcmp(ack_msg, "ACK") != 0) {
            puts("Error: Incorrect ACK received");
            break;
        }
    }

    fclose(fp);
    closesocket(s);
    WSACleanup();

    printf("File sent.\n");
    return 0;
}
