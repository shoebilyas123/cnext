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

#define SERVER_PORT "8081"
#define BACKLOGS 20

#define MAX_URL_LEN 2048
#define MAX_REQ_SIZE 4096

struct req_line {
    char* method;
    char* url;
};


int get_req_line(struct req_line* req_line_res, char *buffer) {
    req_line_res->method = strtok(buffer, " ");
    req_line_res->url = strtok(NULL, " ");
    return 0;
}

int getfile_content(char* path, char* fname, char* buffer) {
    // F_OK: mode parameter to check the existence of a path
    char* f_path = (char *)malloc(1+strlen(path)+strlen(fname));
    strcpy(f_path, path);
    strcat(f_path, fname);
    strcat(f_path, ".html");

    if(access(f_path, F_OK) != 0) {
        return errno;
    }



    return 0;
}

int main() {
    // prepare our address info to create socket file descriptor
    // Host server data
    struct addrinfo *srvinfo;
    struct addrinfo hints;

    // Remote connection data
    int remote_fd;
    struct sockaddr_storage remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);
    struct req_line request_line;

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
        printf("Incoming connection: SOCKET_FD_ID:%d\n", remote_fd);

        char buffer[MAX_REQ_SIZE];
        recv(remote_fd, buffer, MAX_REQ_SIZE, 0);

        get_req_line(&request_line, buffer);

        printf("METHOD: %s\n", request_line.method);
        printf("URL: %s\n", request_line.url);

        char pwd[4096];
        getcwd(pwd, sizeof(pwd));
        strcat(pwd, "/html");

        char* f_contents;

        if(getfile_content(pwd, request_line.url, f_contents) != 0) {
            fprintf(stderr,"File does not exist: ERROR %d\n", errno);
            return errno;
        }

        close(remote_fd);
    }

    return 0;
}
