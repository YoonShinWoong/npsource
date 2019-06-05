#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char *buf);

int main(int argc, char* argv[]){

    char buf[BUF_SIZE];
    int sock, max_fd, cnt;
    int clnt_sock, clnt_addr_size;
    fd_set reads, cpy_reads;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval timeout;
    
    // socket
    sock = socket(PF_INET, SOCK_STREAM,0);
    if(sock == -1)
        error_handling("SOCKET ERROR!");
    
    // bind
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("BIND ERROR!");
    
    // listen
    if(listen(sock, 5) == -1)
        error_handling("LISTEN ERROR!");
    
    // FD 초기화
    FD_ZERO(&reads);
    FD_SET(sock, &reads);
    max_fd = sock;
    
    while(1){
        
        // time
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        
        cpy_reads = reads;
        
        // select
        if((cnt=select(max_fd+1,&cpy_reads, 0,0,&timeout))==-1)
            break;

        if(cnt==0)
            continue;
            
        // check
        for(int i=0;i<=max_fd;i++){
            if(FD_ISSET(i,&cpy_reads)){
                // server일 경우
                if(i==sock){
                    clnt_addr_size = sizeof(clnt_addr);
                    clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
                    
                    // check
                    if(clnt_sock>max_fd)
                        max_fd = clnt_sock;
                    
                    FD_SET(clnt_sock, &reads);
                    printf("Connected Client :%d \n",clnt_sock);
                }
                
                // rw 처리
                else{
                    int str_len;
                    
                    str_len = read(i, buf, BUF_SIZE);
                    
                    // 에러
                    if(str_len == -1)
                        error_handling("READ ERROR!");
                    
                    // 0이면 종료
                    else if(str_len == 0){
                        FD_CLR(i, &reads);
                        close(i); // 닫아주기
                        printf("Disconnected Client :%d \n",i);
                    }
                    
                    else{
                        write(i,buf,str_len);
                    }
                }
            }
        }
        
    }
    close(sock);
    return 0;
    
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}