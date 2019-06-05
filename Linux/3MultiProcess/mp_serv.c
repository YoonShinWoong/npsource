#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#define BUFSIZE 1024

void error_handling(char *);
void read_childproc(int sig);
int main(int argc, char* argv[]){
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
	struct sigaction act;
	char buf[BUFSIZE];
    int sock, i, str_len, clnt_sock;
    pid_t pid;
    
    // signal
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = read_childproc;
    sigaction(SIGCHLD, &act, 0);
    
    // socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("SOCKET ERROR!");
    
    // bind
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))== -1)
        error_handling("BIND ERROR!");
    
    // listen
    if(listen(sock, 5) == -1)
        error_handling("LISTEN ERROR!");
    
    // 처리
    while(1){
        clnt_addr_size = sizeof(clnt_addr);        
        clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        
        // 못열었으면
        if(clnt_sock == -1)
            continue;
        else
            printf("New Client : %d \n",clnt_sock);
        
        pid = fork();
        // fork 실패시
        if (pid == -1){
            close(clnt_sock);
            continue;
        }
    
        // 자식 처리
        if (pid == 0){
            close(sock);
            
            while((str_len = read(clnt_sock, buf, BUFSIZE)) != 0)
                write(clnt_sock, buf, str_len);
            
            puts("client disconnected...");
            close(clnt_sock);
            return 0; // 종료하기!!
        }
        else
            close(clnt_sock);
    }
    close(sock);
    return 0;    
}

void error_handling(char * msg){
    fputs(msg,stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid=waitpid(-1, &status, WNOHANG);
	printf("removed proc id: %d \n", pid);
}
