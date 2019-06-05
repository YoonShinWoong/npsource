#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUFSIZE 100

void error_handling(char *buf);
int main(int argc, char* argv[]){


    int sock;
    char buf[BUFSIZE];
    int str_len;
    struct sockaddr_in serv_addr;
    
    //socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("SOCKET EEROR!");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("CONNECT ERROR!");
    
    while(1){
        
        fputs("INPUT MESSEGE : ", stdout);
        fgets(buf, BUFSIZE, stdin);
        
        if(!strcmp("q\n", buf) || !strcmp("Q\n", buf) )
            break;
        
        write(sock, buf, sizeof(buf));
        str_len = read(sock, buf, BUFSIZE);
        buf[str_len]=0;
        printf("Message from SERVER :%s",buf);
    }
    close(sock);
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}