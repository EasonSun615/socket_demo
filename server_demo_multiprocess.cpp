//
// Created by syr on 2020/3/18.
//

#include <stdio.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 1024    /*max text line length*/
#define SERV_PORT 12345


int main(int argc, char **argv) {
    int listenfd, connfd, len;
    socklen_t clilen;
    struct sockaddr_in servaddr, cliaddr;
    char buf[MAXLINE];

    // create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // preparation of socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind address
    if (bind(listenfd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        perror("bind error");
        exit(-1);
    }

    //listen and set connection queue size 3
    if (listen(listenfd, 3) != 0) {
        perror("listen error");
        exit(-1);
    }

    printf("Waiting client...\n");

    while (true) {
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (sockaddr *) &cliaddr, &clilen)) < 0) {
            perror("accept error");
            continue;
        }
        if (!fork()) {   // child process
            close(listenfd);
            char *client_ip = inet_ntoa(cliaddr.sin_addr);
            printf("connect to client ip:%s, port: %d\n", client_ip, ntohs(cliaddr.sin_port));
            pid_t pid = getpid();
            while (true) {
                memset(buf, 0, sizeof(buf));
                int len = recv(connfd, buf, sizeof(buf), 0);
                buf[len] = '\0';
                if(strcmp(buf,"exit") == 0){
                    close(connfd);
                    printf("disconnect to client ip:%s, port: %d\n", client_ip, ntohs(cliaddr.sin_port));
                    exit(0);
                }
                printf("sub process:%d, get message from client %s %d, message:%s\n", pid, client_ip,ntohs(cliaddr.sin_port), buf);
                printf("input message send to client(%s %d) : ", client_ip,ntohs(cliaddr.sin_port));
                std::cin>>buf;
                send(connfd, buf, strlen(buf),0);
            }
        }
        close(connfd);
    }
    close(listenfd);
}