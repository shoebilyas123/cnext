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
#define MAX_REQ_SIZE 0x100

struct req_line {
    char* method;
    char* url;
};


int get_req_line(struct req_line* req_line_res, char *buffer) {
    req_line_res->method = strtok(buffer, " ");
    req_line_res->url = strtok(NULL, " ");
    return 0;
}

int getfile_content(char* path, char* fname, char* f_buff) {
    // F_OK: mode parameter to check the existence of a path
    char* f_path = (char *)malloc(1+strlen(path)+strlen(fname)+strlen(".html"));
    strcpy(f_path, path);
    strcat(f_path, fname);
    strcat(f_path, ".html");

    if(access(f_path, F_OK) != 0) {
        return errno;
    }
    FILE *fptr;
    char *c_ptr;
    char *c_itr = c_ptr;

    fptr = fopen(f_path, "r");
    char  c_reader;
    while((c_reader = fgetc(fptr)) != EOF) {
        *(c_itr++) = c_reader;
    }

    strcpy(f_buff, c_ptr);
    printf("%s\n", c_ptr);

    // if(fptr != NULL) {
        fclose(fptr);
    // }
    printf("%s", f_buff);
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
        int r_fd = accept(srvfd, (struct sockaddr *)&remote_addr, &remote_addr_len);
        printf("Incoming connection: SOCKET_FD_ID:%d\n", r_fd);

        char buffer[MAX_REQ_SIZE];
        recv(r_fd, &buffer, MAX_REQ_SIZE, 0);

        char *_method = strtok(buffer, " ");
        char *_url = strtok(NULL, " ");

        char* f_path = (char *)malloc(1+strlen(".html")+strlen("./html")+strlen(_url));
        strcpy(f_path, "./html");
        strcat(f_path, _url);
        strcat(f_path, ".html");

        if(access(f_path, F_OK) != 0) {
            char *res_404 = "HTTP/1.1 404 Not Found\r\n\r\n";
            size_t bytes_to_sent = strlen(res_404);
            int bytes_sent = send(r_fd, res_404, bytes_to_sent, 0);
        }else {
            FILE *fp;
            fp = fopen(f_path, "r");
            long fsize = ftell(fp);
            // fclose(fp);

            char *f_contents = (char *)malloc(fsize+1);
            getfile_content("./html", _url, f_contents);
            printf("%s", f_contents);

            // char res_200[BUFSIZ];
            // snprintf(res_200, sizeof(res_200), "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n", c_len, f_contents);
            // send(r_fd, res_line, strlen(res_line), 0);
        }

        close(r_fd);
    }

    return 0;
}
