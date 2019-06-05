#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUFSIZE 1024

void error_handling(char *);

int main(int argc, char * argv[]){
    
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buf[BUFSIZE];
    int i,str_len;
    int sock;
    
    // error 
    if(argc != 2)
        error_handling("ARGC ERROR!");
    
    // socket
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("SOCKET ERROR!");
    
    // bind
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("BIND ERROR!");
    
    // 처리
    while(1){
        clnt_addr_size = sizeof(clnt_addr);
        str_len = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        sendto(sock, buf, str_len, 0, (struct sockaddr*)&clnt_addr, clnt_addr_size);
    }
    
    close(sock);    
    return 0;
}

void error_handling(char * msg){
    fputs(msg,stderr);
    fputc('\n', stderr);
    exit(1);
}
