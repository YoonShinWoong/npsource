#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 1024

void error_handling(char *);
int main(int argc, char *argv[]){
    
    char buf[BUFSIZE];
    int sock, i, str_len;
    struct sockaddr_in serv_addr;
    
    if(argc!=3)
        error_handling("ARGC ERROR!");
    
    //socket
    sock = socket(PF_INET, SOCK_STREAM,0);
    if(sock == -1)
        error_handling("SOCKET ERROR!");
    
    // connect
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))== -1)
        error_handling("CONNECT ERROR!");
    
    
    // 처리
    while(1){
        fputs("INPUT MESSAGE(q to quit) : ",stdout);
        fgets(buf, BUFSIZE, stdin);
        
        // 종료
        if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n")){
            break;
        }
        
        write(sock, buf, sizeof(buf));
        str_len = read(sock, buf, BUFSIZE);
        buf[str_len]=0;
        printf(" Message From Server : %s",buf);
    }
    
    close(sock);
    
}


void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
