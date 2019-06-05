#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>

#define BUFSIZE 4
#define EPSIZE 64
void error_handling(char *);

int main(int argc, char * argv[]){
    char buf[BUFSIZE];
    struct epoll_event * events, event;
    int sock,clnt_sock, i, str_len, epfd;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    
    if(argc!=2)
        error_handling("ARGC ERROR!");
    
    events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPSIZE);
    
    //socket
    sock = socket(PF_INET, SOCK_STREAM,0);
    if(sock == -1)
        error_handling("SOCKET ERROR!");
    
    // bind
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("BIND ERROR!");
    
    // listen
    if(listen(sock, 5)== -1)
        error_handling("LISTEN ERROR!");
    
    
    // epoll
    epfd = epoll_create(EPSIZE);
    event.events = EPOLLIN;
    event.data.fd = sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);
    
    // 처리
    while(1){
        int epcnt;
        epcnt = epoll_wait(epfd, events, EPSIZE, -1); // wait
        if(epcnt==-1)
		{
			puts("epoll_wait() error");
			break;
		}
		puts("return epoll_wait");
        for(i=0;i<epcnt;i++){
            //server
            if(events[i].data.fd == sock){
                clnt_addr_size = sizeof(clnt_addr);
                clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
                
                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d \n", clnt_sock);
            }
            // client
            else{
                str_len = read(events[i].data.fd, buf, BUFSIZE);
                
                // 종료
                if(str_len == 0){
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                    printf("closed client: %d \n", events[i].data.fd);
                }
                
                else{
                    write(events[i].data.fd, buf, str_len);
                }
            }
        }   
    }
    close(sock);
    close(epfd);
    return 0;
}


void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}