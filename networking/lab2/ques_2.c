#include <stdio.h>
#include <string.h>

#define HEADER_SIZE 64
#define MAX_MESSAGE_SIZE 512


void physical_layer(char* message);
void data_link_layer(char* message);
void network_layer(char* message);
void transport_layer(char* message);
void session_layer(char* message);
void presentation_layer(char* message);
void application_layer(char* message);

void append_header_and_print(char* message, const char* header) {
    char newMessage[MAX_MESSAGE_SIZE];
    snprintf(newMessage, sizeof(newMessage), "%s%s", header, message);
    printf("%s\n", newMessage);
    strcpy(message, newMessage);
}

void physical_layer(char* message) {
    append_header_and_print(message, "physical_header: ");
}

void data_link_layer(char* message) {
    append_header_and_print(message, "data_link_header: ");
    physical_layer(message);
}

void network_layer(char* message) {
    append_header_and_print(message, "network_header: ");
    data_link_layer(message);
}

void transport_layer(char* message) {
    append_header_and_print(message, "transport_header: ");
    network_layer(message);
}

void session_layer(char* message) {
    append_header_and_print(message, "session_header: ");
    transport_layer(message);
}

void presentation_layer(char* message) {
    append_header_and_print(message, "presentation_header: ");
    session_layer(message);
}

void application_layer(char* message) {
    append_header_and_print(message, "app_header: ");
    presentation_layer(message);
}

int main() {
    char message[MAX_MESSAGE_SIZE];
    printf("Enter the application message: ");
    fgets(message, sizeof(message), stdin);
    // Removing newline character
    message[strcspn(message, "\n")] = 0;
    
    application_layer(message);
    return 0;
}
