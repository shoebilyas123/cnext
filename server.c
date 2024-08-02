#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT "8080"
#define BACKLOGS 20

int main() {
    // prepare our address info to create socket file descriptor
    // Host server data
    struct addrinfo *srvinfo;
    struct addrinfo hints;

    // Remote connection data
    int remote_fd;
    struct sockaddr_storage remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(NULL, SERVER_PORT, &hints, &srvinfo) != 0) {
        fprintf(stderr, "Cannot initialize server info: ERROR %d\n", errno);
        exit(errno);
    };

    int srvfd = socket(srvinfo->ai_family, srvinfo->ai_socktype, srvinfo->ai_protocol);

    if(bind(srvfd, srvinfo->ai_addr, srvinfo->ai_addrlen) != 0) {
        fprintf(stderr, "Cannot bind to port: ERROR %d\n", errno);
        exit(errno);
    };

    printf("Server started on PORT:%s\n", SERVER_PORT);
    listen(srvfd, BACKLOGS); // Start listening for connections

    while(1) {
        // Accept will block until there is a connection in the accept queue.
        remote_fd = accept(srvfd, (struct sockaddr *)&remote_addr, &remote_addr_len);
        printf("Incomgin connection: SOCKET_FD_ID:%d\n", remote_fd);

        close(remote_fd);
    }

    return 0;
}
