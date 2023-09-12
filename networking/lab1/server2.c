#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

//#pragma comment(lib, "ws2_32.lib")

// Function to get command explanation
const char* getCommandExplanation(const char* command) {
    const char* explanations[] = {
        "ls", "ls : list directory contents",
        "cat", "cat : concatenate files and print on the standard output",
        "pwd", "pwd : print working directory",
        "mkdir", "mkdir : create a directory",
        "cp", "cp : copy files or directories",
        "mv", "mv : move or rename files or directories",
        "rm", "rm : remove files or directories",
        "echo", "echo : display a message",
        "chmod", "chmod : change file permissions",
        "chown", "chown : change file ownership",
        "ps", "ps : display information about running processes",
        "top", "top : display dynamic view of system processes",
        "df", "df : report file system disk space usage",
        "du", "du : estimate file space usage",
        "grep", "grep : search for patterns in files",
        "sed", "sed : stream editor for text manipulation",
        "awk", "awk : pattern scanning and text processing language",
        "ping", "ping : send ICMP echo requests to a host",
        "traceroute", "traceroute : print the route packets take to network host",
        "netstat", "netstat : display network connections, routing tables, etc.",
       
    };

    // Search for the command and return its explanation
    for (int i = 0; i < sizeof(explanations) / sizeof(explanations[0]); i += 2) {
        if (strcmp(command, explanations[i]) == 0) {
            return explanations[i + 1];
        }
    }

    return "Command not recognized";
}
DWORD WINAPI handleClient(LPVOID clientSocketPtr) {
    SOCKET clientSocket = (SOCKET)clientSocketPtr;

    char buffer[1024];
    int bytesRead;

    while (1) {
        // Receive data from client
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }

        buffer[bytesRead] = '\0';
        printf("Client socket %d sent message: %s\n", clientSocket, buffer);

        // Get command explanation
        const char* explanation = getCommandExplanation(buffer);

        // Send explanation to client
        send(clientSocket, explanation, strlen(explanation), 0);
    }

    printf("Client socket %d disconnected\n", clientSocket);

    // Close client socket
    closesocket(clientSocket);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return 1;
    }

    // Create socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        perror("socket creation failed");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("bind failed");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Listen
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        perror("listen failed");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d\n", port);

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    while (1) {
        // Accept incoming connection
        SOCKET clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            perror("accept failed");
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("Connected with client socket number %d\n", clientSocket);

        // Create a thread to handle the client
        HANDLE threadHandle;
        DWORD threadId;
        threadHandle = CreateThread(NULL, 0, handleClient, (LPVOID)clientSocket, 0, &threadId);
        if (threadHandle == NULL) {
            perror("CreateThread failed");
            closesocket(clientSocket);
        } else {
            // Close the thread handle (the thread itself will continue running)
            CloseHandle(threadHandle);
        }
    }

    // Close listening socket
    closesocket(listenSocket);

    WSACleanup();

    return 0;
}
