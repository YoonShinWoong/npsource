#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#define BUFSIZE 1024

int clnt_socks[BUFSIZE];
int clnt_cnt;
pthread_mutex_t mutex;
char buf[BUFSIZE];

void* func1(void * arg);
void func2(char *, int);
void error_handling(char *buf);
int main(int argc, char* argv[]){
    
    int sock,i,str_len, clnt_sock;
    pthread_t pid;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    
    if(argc!=2)
        error_handling("ARGC ERROR!");
    
    pthread_mutex_init(&mutex, NULL);
    
    // socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("SOCKET ERROR!");
    
    
    // bind
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("BIND ERROR!");
    
    // listen
    if(listen(sock,5)==-1)
        error_handling("LISTEN ERROR!");
    
    // 처리
    while(1){
        
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        
        // MUTEX
        pthread_mutex_lock(&mutex);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutex);
        
        // create
        pthread_create(&pid, NULL, func1, (void*)&clnt_sock);
		pthread_detach(pid);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_addr.sin_addr));
    }
    close(sock);
    pthread_mutex_destroy(&mutex);
    
}

void* func1(void * arg){

    int sock = *((int *)arg);
    int str_len;

    // 읽고 쓰기
    while(str_len = read(sock, buf, BUFSIZE))
        func2(buf, str_len);
    
    	
	pthread_mutex_lock(&mutex);
    
	for(int i=0; i<clnt_cnt; i++)   // remove disconnected client
	{
		if(sock==clnt_socks[i])
		{
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutex);
	close(sock);
	return NULL;
    
}

void func2(char * msg, int len)   // send to all
{
	int i;
	pthread_mutex_lock(&mutex);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutex);
}

void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}