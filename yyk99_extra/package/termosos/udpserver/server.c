/*
   server.c -- UDP server command processing

   Supported commands (single character):
     'v' - version
     '?' - help
     't' - temperature
     'h' - humidity
     other - help
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "server.h"

#define VERSION "1.0"

#define HELP_TEXT \
    "Available commands:\n" \
    "  v - Show server version\n" \
    "  ? - Show this help message\n" \
    "  t - Read temperature from DHT11 sensor\n" \
    "  h - Read humidity from DHT11 sensor\n"

static int reply_string(server_reply_t *out, const char *str)
{
    size_t len = strlen(str);
    out->output = malloc(len);
    if (!out->output)
        return -1;
    memcpy(out->output, str, len);
    out->output_size = len;
    return 0;
}

static int read_dht11(char *buff, size_t buff_size)
{
    int fd, s;

    fd = open(DHT11_DEVICE, O_RDONLY);
    if (fd == -1)
        return -1;

    s = read(fd, buff, buff_size - 1);
    close(fd);

    if (s < 0)
        return -1;

    buff[s] = '\0';
    return s;
}

int server_process_request(server_t *self, const char *cmd, server_reply_t *out)
{
    char buff[80];
    char result[80];
    const char *p;

    /* skip leading whitespace */
    p = cmd;
    while (*p && isspace((unsigned char)*p))
        p++;

    switch (*p) {
    case 'v':
        return reply_string(out, "udpserver version " VERSION "\n");

    case '?':
        return reply_string(out, HELP_TEXT);

    case 't':
        if (read_dht11(buff, sizeof(buff)) < 0)
            return -1;

        /* extract temperature from "T: XX.XXC H: XX.XX%\n" */
        {
            char *t = strstr(buff, "T:");
            char *h;
            if (!t)
                return -1;
            h = strstr(t, " H:");
            if (h)
                *h = '\0';
            snprintf(result, sizeof(result), "%s\n", t);
        }
        return reply_string(out, result);

    case 'h':
        if (read_dht11(buff, sizeof(buff)) < 0)
            return -1;

        /* extract humidity from "T: XX.XXC H: XX.XX%\n" */
        {
            char *h = strstr(buff, "H:");
            if (!h)
                return -1;
            /* h already ends with \n from driver */
            snprintf(result, sizeof(result), "%s", h);
        }
        return reply_string(out, result);

    default:
        return reply_string(out, "Unknown command\n" HELP_TEXT);
    }
}
