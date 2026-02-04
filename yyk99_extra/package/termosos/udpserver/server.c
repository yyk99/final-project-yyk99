/*


 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

int server_process_request(server_t *self, const char *cmd, server_reply_t *out)
{
    int fd, s;
    char buff[80];

    fd = open(DHT11_DEVICE, O_RDONLY);
    if (fd == -1)
        return -1; /* see errno */

    /* TODO: parse the cmd */
    s = read(fd, buff, sizeof(buff));
    if (s < 0) {
        close(fd);
        return -1;
    }

    out->output_size = s;
    out->output = malloc(s);
    memcpy(out->output, buff, out->output_size);

    return 0;
}
