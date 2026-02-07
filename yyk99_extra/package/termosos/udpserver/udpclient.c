/*
  udp_echo_client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "server.h"

int main(int argc, char **argv)
{
    int sockfd;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    const char *server_name = "127.0.0.1";

    if (argc == 2) {
        server_name = argv[1];
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_name);

    printf("UDP Echo Client connected to (%s)\n", server_name);
    printf("Type messages to send (Ctrl+C to exit):\n");

    while (1) {
        printf("> ");
        if (!fgets(message, BUFFER_SIZE, stdin))
            break;
        message[strcspn(message, "\n")] = 0; // Remove newline

        // Send message to server
        sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Receive echo from server
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                 (struct sockaddr *)&server_addr, &server_len);

        printf("%s\n", buffer);
    }

    close(sockfd);
    return 0;
}
