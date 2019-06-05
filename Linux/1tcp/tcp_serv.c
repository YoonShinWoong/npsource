#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 1024

void error_handling(char *);

int main(int argc, char * argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buf[BUFSIZE];
    
    if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
    
    // socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("SOCKET ERROR!");
    
    // bind
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("BIND ERROR!");
    
    //listen
    if(listen(serv_sock, 5) == -1)
        error_handling("LISTEN ERROR!");
    
    
    // 처리
    clnt_addr_size = sizeof(clnt_addr);
    int i, str_len;
    for(i=0; i<5; i++){
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        
		if(clnt_sock == -1)
			error_handling("accept() error");
        
        // 까먹은 처리 부분!!!
		else
			printf("Connected client %d \n", i+1);
        
        // 틀린 부분
		while((str_len = read(clnt_sock, buf, BUFSIZE))!= 0)
			write(clnt_sock, buf, str_len);
        close(clnt_sock);
    }
    
    close(serv_sock);
    return 0;
}

void error_handling(char * msg){
    fputs(msg,stderr);
    fputc('\n', stderr);
    exit(1);
}














