#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define CHUNK_SIZE 5000

int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c;
    FILE *fp;
    char buffer[CHUNK_SIZE + 1];
    char *ack = "ACK";

    WSAStartup(MAKEWORD(2,2),&wsa);

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 3);

    puts("Server ready to receive file...");

    c = sizeof(struct sockaddr_in);
    new_socket = accept(s, (struct sockaddr *)&client, &c);
    puts("Connection accepted. Waiting for file from client...");

    fp = fopen("received_alice.txt", "wb");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    int bytes_received;
    while ((bytes_received = recv(new_socket, buffer, CHUNK_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
        send(new_socket, ack, strlen(ack), 0);
        puts("Sent ACK to client...");
    }

    fclose(fp);
    closesocket(new_socket);
    closesocket(s);
    WSACleanup();

    printf("File received and saved as received_alice.txt\n");
    return 0;
}
