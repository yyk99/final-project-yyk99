/*
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <getopt.h>
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "server.h"

int done = 0;

static void on_signal(int)
{
    syslog(LOG_INFO, "Caught signal, exiting");
    done = 1;
    exit(EXIT_FAILURE);
}

static void usage(const char *me)
{
    fprintf(stderr, "Usage: %s [-h] [-d] [-p port-number]\n", me);
}

int main(int argc, char **argv)
{
    const char *me = argv[0];
    int ch;
    int daemon_flag = 0;

    openlog("udpserver", LOG_PID|LOG_PERROR, LOG_USER);

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    while((ch = getopt(argc, argv, "hdp:")) != -1) {
        switch(ch) {
        case 'd':
            daemon_flag = 1;
            break;
        case 'h':
            usage(me);
            return 0;
        case 'p':
            /* ignored */
            break;
        default:
            usage(me);
            exit(EXIT_FAILURE);
        }
    }

    argc -= optind;
    argv += optind;

    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    ssize_t recv_len;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        syslog(LOG_ERR, "Socket creation failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        syslog(LOG_ERR, "Bind failed: %s", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "UDP Echo Server listening on port %d...\n", PORT);

    // Echo loop
    while (!done) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive data from client
        recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                            (struct sockaddr *)&client_addr, &client_len);

        if (recv_len < 0) {
            syslog(LOG_ERR, "recvfrom failed: %s", strerror(errno));
            continue;
        }

        buffer[recv_len] = '\0';
        syslog(LOG_INFO, "Received from %s:%d: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               buffer);

        // Echo back to client
        if (sendto(sockfd, buffer, recv_len, 0,
                   (struct sockaddr *)&client_addr, client_len) < 0) {
            syslog(LOG_ERR, "sendto failed: %s", strerror(errno));
        }
    }

    close(sockfd);
    closelog();
    return 0;

}
