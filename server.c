//#!/usr/bin/c -Wall -Wextra -std=gnu99 -pedantic --
// This server can do 20k requests/sec on my laptop...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <err.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>

#define PORT 2020
#define _200 "HTTP/1.0 200 OK\r\n"
#define _404 "HTTP/1.0 404 Not Found\r\n\r\nnot found\n"
#define _400 "HTTP/1.0 400 Bad Request\r\n\r\nbad request\n"

struct ref_t {const char *path, *ctyp;};
void switch_paths(char * req, struct ref_t * ref){

  if(!strcmp(req,"")){
      ref->path = "index.html";
      ref->ctyp = "text/html";
      }
  else if(strstr(req,".ico")){
      ref->path = req;
      ref->ctyp = "image/x-icon";
  }
  else if(strstr(req,".png")){
      ref->path = req;
      ref->ctyp = "image/png";
  }
  else if(strstr(req,".css")){
      ref->path = req;
      ref->ctyp = "text/css";
  }
  else if(strstr(req,".gif")){
      ref->path = req;
      ref->ctyp = "image/gif";
  }

  else if (!strcmp(req,"Lectures")){
        printf("Lectures!\n");
        ref->path = "static/Lectures/Lectures.html";
        ref->ctyp = "text/html";
      }
  else if (!strcmp(req,"EscolaVirtual/")){
        printf("EscolaVirtual!\n");
        ref->path = "static/projects/EscolaVirtual.html";
        ref->ctyp = "text/html";
      }
  else if (!strcmp(req,"CataOnibus/")){
        printf("CataÔnibus!\n");
        ref->path = "static/projects/CataOnibus.html";
        ref->ctyp = "text/html";
      }
  else if (!strcmp(req,"Darwin/")){
        printf("Darwin!\n");
        ref->path = "static/projects/Darwin.html";
        ref->ctyp = "text/html";
      }
  else if (!strcmp(req,"CifraVirtual/")){
        printf("CifraVirtual!\n");
        ref->path = "static/projects/CifraVirtual.html";
        ref->ctyp = "text/html";
      }
  else if (!strcmp(req,"CariocaScript/")){
        printf("CariocaScript!\n");
        ref->path = "static/projects/CariocaScript.html";
        ref->ctyp = "text/html";
      }
  else{
      ref->path = req;
      ref->ctyp = "image/png";

  }

}

// Extract path from a request.
int xpath(char *req, struct ref_t *ref) {
    // Determine path length. Truncate string.
    char *p = strchr(req,  ' ');
    int len = p - req;
    req[len] = '\0';

    // Remove query strings.
    p = strchr(req, '?');
    if (p) *p = '\0';

    printf("File path:%s\n",req);
    
    switch_paths(req, ref);
    
    printf("File Type: %s\n",ref->ctyp);
    
    return (!!strchr(req, '/'));
}

// Given a HTTP request, write to directly to the output fd.
int respond(char *req, int ofd) {
    printf("\n\n----Init Request------\n");
    printf("Original request:%s\n",req);
    
    
    struct ref_t ref; // Drop non-GETs / netcat garbage / path traversals
    if (strncmp("GET ", req, 4) || !strchr(req+5, ' ') )
        return write(ofd, _400, strlen(_400));
    xpath(req+5, &ref);

    // Attempt to open file, else 404
    int ifd = open(ref.path, O_RDONLY);
    if (ifd == -1) {
        return write(ofd, _404, strlen(_404));
    }

    // Read and transmit file
    int ret = 0;
    struct stat st;
    stat(ref.path, &st);
    sprintf(req, _200 "Content-Type: %s\r\nContent-Length: %ld\r\n\r\n", ref.ctyp, st.st_size);
    
    if(!strcmp(ref.ctyp,"image/png")){
        printf("Image!\n");
        
        FILE * file = fdopen(ofd,"r");
        ret = fwrite(req, strlen(req), 1, file);
     }
     else{
    ret = write(ofd, req, strlen(req));
    }
    if (ret == -1) goto cleanup;
    ret = sendfile(ofd, ifd, 0, st.st_size);

cleanup:
    close(ifd);
    return ret;
}

int main(void) {
    // Ignore remotely terminated sockets
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) err(1, "signal");

    // Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) err(1, "sock");
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) err(1, "setsockopt");

    // Bind/Listen
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr))) err(1, "bind");
    if (listen(sock, 50)) err(1, "listen"); // Backlog=16

    while (1) {
        // Accept
        socklen_t alen = sizeof(addr);
        int fd = accept(sock, (struct sockaddr *) &addr, &alen);
        if (fd == -1) goto err;

        // Receive
        char buf[1024]; 
        int len = read(fd, buf, sizeof(buf));
        if (len < 1) goto err;

        // Transmit
        buf[len-1] = '\0';
        if (respond(buf, fd) == -1) goto err;
        printf("Fd: %d, len: %d\n",fd, len);

        // Close socket
        shutdown(fd, SHUT_RDWR);
        err: close(fd);
    }
}
