#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUFSIZE 1024

void error_handling(char *);

int main(int argc, char * argv[]){
    
    struct sockaddr_in serv_addr, from_addr;
    socklen_t from_addr_size;
    char buf[BUFSIZE];
    int i,str_len;
    int sock;
    
    // error 
    if(argc != 3)
        error_handling("ARGC ERROR!");
    
    // socket
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("SOCKET ERROR!");
    
    // 주소처리
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    
    // 처리
    while(1){
        fputs("Insert message(Q to quit): ", stdout);
		fgets(buf, BUFSIZE, stdin);
		if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
			break;
        sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        memset(buf, 0, BUFSIZE);
        from_addr_size = sizeof(from_addr);
        str_len = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&from_addr, &from_addr_size);
        buf[str_len]=0;
        printf("Message From Server : %s",buf);
    }
    
    close(sock);    
    return 0;
}

void error_handling(char * msg){
    fputs(msg,stderr);
    fputc('\n', stderr);
    exit(1);
}
