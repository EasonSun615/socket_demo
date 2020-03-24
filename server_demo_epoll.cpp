//
// Created by syr on 2020/3/24.
//
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <cctype>

#define SERVER_PORT 12345
#define BUF_SIZE 1024
#define MAX_EVENTS 1000
int main(){
    int listenfd, epfd, connfd;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0))< 0){
        perror("create socket error");
        exit(-1);
    }
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_family = AF_INET;
    if(bind(listenfd,(sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
        perror("bind error");
        exit(-1);
    }
    if(listen(listenfd, 5)!=0){
        perror("listen error");
        exit(-1);
    }
    char buf[BUF_SIZE];
    // 创建一个epoll
    epfd = epoll_create(1000);
    struct epoll_event ev, events[MAX_EVENTS];
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;
    // 把监听socket 先添加到epfd中
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    while(true){
        // 返回已就绪的epoll_event,-1表示阻塞,没有就绪的epoll_event,将一直等待
        int ev_cnt = epoll_wait(epfd, events, MAX_EVENTS ,-1);
        for(int i=0;i<ev_cnt; i++){
            // 如果是新的连接,需要把新的socket添加到epfd中
            if(events[i].data.fd == listenfd){
                connfd = accept(listenfd, (sockaddr *)&cli_addr, &clilen);
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
                printf("connect client:%s, %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            }else{   // 否则说明是connfd可读
                connfd = events[i].data.fd;
                memset(buf, 0, sizeof(buf));
                int len = recv(connfd, buf, BUF_SIZE, 0);
                if(len==0){   //客户端关闭连接
                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
                    close(connfd);
                    printf("close client\n");
                }else{
                    buf[len] = '\0';
                    printf("%s\n", buf);
                    for(int j = 0; j<len; j++){
                        buf[j] = toupper(buf[j]);
                    }
                    write(connfd, buf, len);
                }
            }
        }
    }
    close(listenfd);
    close(epfd);

}

