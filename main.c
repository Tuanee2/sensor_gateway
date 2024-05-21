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
char in_buff[MSG_SIZE];
int num_read = 0;

sem_t *sem01,*sem02;

// create the data's structure
LinkedList data ={NULL,NULL};

void *handle_client(void *arg) {
    int sock = *(int*)arg;
    free(arg); // Free the allocated memory for the socket descriptor

    char buffer[1024];
    int sensor_id;
    double temperature;
    time_t timestamp;

    while (recv(sock, buffer, sizeof(buffer), 0) > 0) {
        //sscanf(buffer, "%d %lf %ld", &sensor_id, &temperature, &timestamp);
        //pthread_mutex_lock(&mutex_lock01);
        //Node* newNode = (Node*)malloc(sizeof(Node));
        //newNode->sensor_id = sensor_id;
        //newNode->temperature = temperature;
        //strcpy(newNode->timestamp, timestamp);
        //newNode->next = data.head;
        //data.head = newNode;
        //pthread_mutex_unlock(&mutex_lock01);
        write(fds[1], buffer, MSG_SIZE);
        printf("done putting data into pipe\n");

        sem_post(sem01);

        // Log the received data
        //char log_message[256];
        //sprintf(log_message, "Sensor %d: Temp=%.2f at %s", sensor_id, temperature, ctime(&timestamp));
        //write(fds[1], log_message, strlen(log_message));
    }

    close(sock);
    return NULL;
}

static void* thread_handler(void *args){
    pthread_detach(pthread_self());
    // get thread id 
    LinkedList* data = (LinkedList*)args;
    pthread_t tid = pthread_self();
    if(pthread_equal(tid1,tid)){
        //create server socket
        int server_fd,*client_sock;
        struct sockaddr_in server, client;
        socklen_t c = sizeof(struct sockaddr_in);

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
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, MAX_EVENTS) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        printf("Maximum 20 client.\n");
        printf("Hearing any clinet .....\n");

        for(int i = 0;i<20;i++){
            int index = appendEmptyNode(data);
            printf("index : %d\n",index);
            client_sock = malloc(sizeof(int));
            *client_sock = accept(server_fd, (struct sockaddr *)&client, &c);
            printf("client number %d connect to server.\n",i);
            if (*client_sock < 0) {
                free(client_sock);
                continue;
            }

            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, handle_client, client_sock) < 0) {
                perror("Could not create thread");
                close(*client_sock);
                free(client_sock);
            }

            pthread_detach(thread_id);
            sleep(60);
        }


    }else if(pthread_equal(tid2,tid)){
        while(1){
            sem_wait(sem02);
            printf("manager\n");
            Node* current = data->head;
            while (current != NULL)
            {
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

    append(&data,"15:33",30,1,1);
    Node* first = getFirst(&data);
    printf("%s - %.1f - %d\n",first->timestamp,first->temperature,first->sensor_id);

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
            
            num_read = read(fds[0], in_buff, MSG_SIZE);
            if (num_read == -1) {
                printf("read() failed\n");
                exit(0);
            } else if (num_read == 0) {
                printf("pipe end-of-pipe\n");
                    
            } else {
                printf("msg: %s\n", in_buff);
            }
        // write data to logfile 
            char final_mes[MSG_SIZE + 14];
            snprintf(final_mes,MSG_SIZE + 14,"%s - connection\n",in_buff);
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