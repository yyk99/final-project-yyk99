/*

 */

#ifndef UDPSERVER_H
#define UDPSERVER_H

#define LISTEN_BACKLOG 10
#define PORT 9001
#define BUFFER_SIZE 1024

typedef struct {
    int port;
} server_t;

#endif /* UDPSERVER_H */
