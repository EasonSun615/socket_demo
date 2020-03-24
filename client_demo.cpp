//
// Created by syr on 2020/3/19.
//


#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define REMOTE_IP "10.224.11.111"
#define REMOTE_PORT 12345
#define MAXLINE 1024


int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0){
        perror("create socket error:");
        exit(-1);
    }
    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(REMOTE_PORT);
    remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP);
    if(connect(fd,(sockaddr *)&remote_addr, sizeof(remote_addr)) < 0){
        perror("connect error:");
        exit(-1);
    }
    printf("...connect\n");
    char buf[MAXLINE];
    while(true) {
        memset(buf, 0, sizeof(buf));
        std::cin>>buf;
        send(fd, buf, strlen(buf),0);
        if(strcmp(buf, "exit") == 0)
            break;
        int len = recv(fd, buf, sizeof(buf), 0);
        buf[len] = '\0';
        printf("%s\n",buf);
    }
    close(fd);
}
