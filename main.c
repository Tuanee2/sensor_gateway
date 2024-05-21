#include <stdio.h>
#include "shared_data.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>  
#include <time.h> 
#include <sys/mman.h>      
#include <sys/epoll.h>

#define MSG_SIZE 200
#define MAX_EVENTS 20
#define BUFFER_SIZE 512


pid_t main_pid,log_pid;
pthread_t tid1,tid2,tid3;
pthread_mutex_t mutex_lock01;
int port = 2000;

// create pipe
int fds[2];
char in_buff[BUFFER_SIZE];
int num_read = 0;

sem_t *sem01,*sem02;

int numOfClient = 0;
int orderOfClient[25];

// create the data's structure
LinkedList data ={NULL,NULL};

void parseAndUpdateNode(LinkedList* list, int nodeIndex, const char* message) {
    char* copy = strdup(message);
    if (!copy) {
        perror("Failed to duplicate message string");
        exit(EXIT_FAILURE);
    }

    char* token;
    char timestamp[50] = {0};
    int room_id = 0;
    float temperature = 0.0;

    // Lấy timestamp
    token = strtok(copy, " -"); // Sử dụng " -" để ngăn cắt số phút và giây
    if (token != NULL) {
        strncpy(timestamp, token, sizeof(timestamp) - 1);
        timestamp[sizeof(timestamp) - 1] = '\0';
    }

    // Lấy room ID
    token = strtok(NULL, " -");
    if (token != NULL) {
        room_id = atoi(token);
    }

    // Lấy temperature
    token = strtok(NULL, " -");
    if (token != NULL) {
        temperature = atof(token);
    }

    Node* node = getNodeAt(list, nodeIndex);
    if (node != NULL) {
        strncpy(node->timestamp, timestamp, sizeof(node->timestamp) - 1);
        node->timestamp[sizeof(node->timestamp) - 1] = '\0';
        node->sensor_id = room_id;
        node->temperature = temperature;
    }

    free(copy);
}



static void* thread_handler(void *args){
    pthread_detach(pthread_self());
    // get thread id 
    LinkedList* data = (LinkedList*)args;
    pthread_t tid = pthread_self();
    if(pthread_equal(tid1,tid)){
        //create server socket
        int server_fd,*client_sock,epoll_fd,new_sock;
        struct sockaddr_in server, client;
        socklen_t c = sizeof(struct sockaddr_in);
         struct epoll_event event, events[MAX_EVENTS];

        // Set up the socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            perror("Could not create socket");
            exit(EXIT_FAILURE);
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
            kill(main_pid,9);
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, MAX_EVENTS) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        // Set the server socket to non-blocking
        set_socket_non_block(server_fd);

        printf("Maximum 20 client.\n");
        printf("Hearing any clinet .....\n");

        // Create an epoll instance
        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }

        event.data.fd = server_fd;
        event.events = EPOLLIN;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }

        while (1)
        {
            int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            if(n == -1){
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }

            for(int i=0;i<n;i++){
                if(events[i].data.fd == server_fd){
                    new_sock = accept(server_fd, (struct sockaddr *)&client, &c);
                    if(new_sock == -1){
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    if(new_sock > 0 ){
                        numOfClient ++;
                        printf("%d sensor connect to server\n",numOfClient);
                        orderOfClient[numOfClient] = new_sock;
                        appendEmptyNode(data);
                        data->head->sensor_id = numOfClient;
                    }
                    set_socket_non_block(new_sock);
                    event.data.fd = new_sock;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_sock,&event) == -1) {
                        perror("epoll_ctl: conn_sock");
                        exit(EXIT_FAILURE);
                    }
                }else{
                    int done = 0;
                    while (1) {
                        ssize_t count;
                        char buf[BUFFER_SIZE];

                        count = read(events[i].data.fd, buf, sizeof buf);
                        if (count == -1) {
                            // If errno == EAGAIN, that means we have read all data
                            if (errno != EAGAIN) {
                                perror("read");
                                done = 1;
                            }
                            break;
                        } else if (count == 0) {
                            // End of file
                            done = 1;
                            break;
                        }
                        parseAndUpdateNode(data,i,buf);
                        printf("%s\n",buf);
                        write(fds[1], buf, BUFFER_SIZE);
                        printf("done putting data into pipe\n");
                        sem_post(sem01);
                    }
                    if (done) {
                        printf("Closed connection on descriptor %d\n", events[i].data.fd);
                        close(events[i].data.fd);
                        numOfClient--;
                        printf("1 sensor disconnect to server\n");
                    }
                }
            }
        }
    }else if(pthread_equal(tid2,tid)){
        while(1){
            sem_wait(sem02);
            Node* current = data->head;
            int count = 1;
            
            printList(data);
            while (current != NULL)
            {
                printf("Temperature %d \n",count);
                count++;
                printf("%f\n",current->temperature);
                if(current->temperature > 30) printf("too hot\n");
                if((current->temperature <= 30) && (current->temperature > 25)) printf("warn\n");
                if((current->temperature <= 25) && (current->temperature >= 20)) printf("cool\n");
                if(current->temperature <20 ) printf("cold\n");
                current = current->next;
            }
        }
    }

}

int set_socket_non_block(int socket_fd){
    int flags;
    if ((flags = fcntl(socket_fd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char* argv[]){
    //get main pid;
    main_pid = getpid();

    //convert port
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    if(pipe(fds)<0){
        printf("pipe() unsuccessfully\n");
        exit(1);
    }

    // create semaphore
    sem01 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem01 == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    sem02 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem02 == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    // set sem01 = 0,sem02 = 0;
    sem_init(sem01,1,0);
    sem_init(sem02,1,0);

    // create the log_process
    log_pid = fork();
    if(log_pid == 0){

        // close writting mode of pipe
        if(close(fds[1]) == -1){
            printf("close(fds[1]) failed\n");
        }

        // create log file
        char* logfilename = "logfile.txt";

        int fd = open(logfilename,O_CREAT | O_RDWR | O_APPEND, 0660 );
        if (fd == -1) {
            perror("Can't open the file :((\n");
            exit(EXIT_FAILURE);
        }
        while(1){
            printf("wait to write the log file\n");
            sem_wait(sem01); // sem = 0 (block)
            
            num_read = read(fds[0], in_buff, BUFFER_SIZE);
            if (num_read == -1) {
                printf("read() failed\n");
                exit(0);
            } else if (num_read == 0) {
                printf("pipe end-of-pipe\n");
                    
            } else {
                printf("msg: %s\n", in_buff);
            }
        // write data to logfile 
            char final_mes[BUFFER_SIZE + 14];
            snprintf(final_mes,BUFFER_SIZE + 14,"%s - connection\n",in_buff);
            char* text_log = final_mes;
            ssize_t bytes_written = write(fd,text_log,strlen(text_log));
            if(bytes_written == -1)
            {
                perror("error to write data :((");
                exit(EXIT_FAILURE);
            }
            printf("Writting data is completed.\n");
            sem_post(sem02);
        }

    }else{
        
        //creat the share data
        LinkedList share_data = {NULL,NULL};

        // close reading mode of pipe
        if (close(fds[0]) == -1){
            printf("close(fds[0]) failed\n");
        }
        
        
        int ret;

        // creat the connection thread
        if(ret = pthread_create(&tid1,NULL,&thread_handler,&data)){
            printf("Error to create the thread 1.\n");
            return -1;
        }
        // create the data manager thread
        if(ret = pthread_create(&tid2,NULL,&thread_handler,&data)){
            printf("Error to create the thread 2.\n");
            return -1;
        }

        while(1){

        }


    }

    return 0;
}