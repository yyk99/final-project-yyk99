/*

 */

#ifndef UDPSERVER_H
#define UDPSERVER_H

#define LISTEN_BACKLOG 10
#define PORT 9001
#define BUFFER_SIZE 1024

#define DHT11_DEVICE "/dev/dht11"

typedef struct {
    int port;
} server_t;

typedef struct {
    char *output;
    size_t output_size;
} server_reply_t;

/*
  returns status, 0 == OK, otherwise is FAILURE
 */
int server_process_request(server_t *self, const char *cmd, server_reply_t *out);

#endif /* UDPSERVER_H */
