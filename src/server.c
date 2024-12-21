#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
//#include <sys/epoll.h>
#include <sys/socket.h>
#include "sighand.h"
#include "sensor.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>

#define default_port  2000
#define LISTEN_BACKLOG 50
#define BUFF_SIZE 256

int port,len;
int server_fd, newsocket_fd;
struct sockaddr_in serv_addr;

pid_t child_pids[2];

// Signal handler cho SIGINT
void sigint_handler(int signum) {
    printf("Parent received SIGINT. Sending SIGINT to children...\n");
    signal_intallchild(child_pids, 2); // Gửi SIGINT đến các tiến trình con
}

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    pid_t child_pid, pid;
    pid = getpid();

    if(argc > 1){
        port = atoi(argv[1]);
    }else{
        port = default_port;
    }

    child_pid = fork();
    if(child_pid == 0){
        signal(SIGINT, signal_handler00);
        while(1);
    }else{
        pid_t child01_pid = fork();
        if(child01_pid == 0){
            signal(SIGINT, signal_handler00);
            while(1);
        }else{
            printf("i'm %d\n", getpid());
            sleep(5);
            child_pids[0] = child_pid;
            child_pids[1] = child01_pid;
            signal(SIGINT,sigint_handler); // Thiết lập hàm xử lý tín hiệu SIGINT

            pthread_t tid0,tid1;
            

            server_fd = socket(AF_INET, SOCK_STREAM, 0);
            if(server_fd == -1){
                handle_error("socket()");
            }

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);
            serv_addr.sin_addr.s_addr =  INADDR_ANY;
            if(bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1){
                handle_error("bind()");
            }

            if (listen(server_fd, LISTEN_BACKLOG) == -1){
                   handle_error("listen()");
            }

            while(1){
                newsocket_fd = accept(server_fd, (struct sockaddr *)&serv_addr, (socklen_t*)&len);
                if(newsocket_fd == -1){
                    handle_error("accept()");
                }
            }
        
            
        }

        
    }
    
    
    return 0;
}