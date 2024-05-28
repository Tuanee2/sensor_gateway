#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>
#include <pthread.h>

pthread_t tid1,tid2,tid3;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;


// socket params init
int new_socket_fd;
#define LISTEN_BACKLOG 10 // listen 10 sensor
#define BUFF_SIZE 256
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


// pipes params init


// linked list
typedef struct Node {
    int socket_fd;
    float temperature;
    time_t timestamp; 
    struct Node* next;
} Node;

Node* add_to_front(Node* head, float temp, time_t time) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Lỗi cấp phát bộ nhớ\n");
        return head;
    }
    new_node->temperature = temp;
    new_node->timestamp = time;
    new_node->next = head;
    return new_node;
}

void print_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        char buffer[30];
        strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", localtime(&current->timestamp));
        printf("Temperature: %.2f at %s\n", current->temperature, buffer);
        current = current->next;
    }
}



Node* get_node(Node* head, int index) {
    Node* current = head;
    int count = 0;  // Node hiện tại bắt đầu từ 0
    while (current != NULL) {
        if (count == index) {
            return current;  // Trả về node tại vị trí index
        }
        count++;
        current = current->next;
    }
    return NULL;  // Trả về NULL nếu index không hợp lệ
}

void recvice_data(int new_socket_fd){
    int numb_read, numb_write;
    char sendbuff[BUFF_SIZE];
    char recvbuff[BUFF_SIZE];
    memset(sendbuff, '0', BUFF_SIZE);
    memset(recvbuff, '0', BUFF_SIZE);

    /* Đọc dữ liệu từ socket */
    /* Hàm read sẽ block cho đến khi đọc được dữ liệu */
    while(1){
        numb_read = read(new_socket_fd, recvbuff, BUFF_SIZE);
        if(numb_read == -1){
            handle_error("read()");
        }
        if (strncmp("exit", recvbuff, 4) == 0) {
            system("clear");
        }
        printf("\nMessage from Client: %s\n", recvbuff);
        int data = atoi(recvbuff);
        printf("data : %d\n",data);
    }

    close(new_socket_fd);

}


// thread handler
static void*thread_handle(void *args){
    pthread_detach(pthread_self());
    pthread_t tid = pthread_self();
    Node* myNode = (Node*)args;
    if(pthread_equal(tid1,tid)){
        pthread_mutex_lock(&lock1);
        printf("%d\n",new_socket_fd);
        //chat_func(new_socket_fd);
        
        recvice_data(new_socket_fd);

    
        //printf("Messages from client : %d",data);
        pthread_mutex_unlock(&lock1);
    }else if(pthread_equal(tid2,tid)){

    }else{

    }
}



int main(int argc,char* argv[]){
    // creat pipes
    int fds[2];

    if (pipe(fds) < 0) {
        printf("pipe() unsuccessfully\n");
        exit(1);
    }

    // create child_process
    pid_t log_pid = fork();
    if(log_pid == 0){
        printf("im child process!\n");
        

    }else{
        // close read 
        if (close(fds[0]) == -1){
            printf("close(fds[0]) failed\n");
        }

        int port_no, len, opt;
        int server_fd;
        struct sockaddr_in serv_addr, client_addr;

        if (argc < 2) {
            printf("No port provided\ncommand: ./server <port number>\n");
            exit(EXIT_FAILURE);
        } else {
            port_no = atoi(argv[1]);
        }

        //create socket 
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1){
            handle_error("socket()");
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            handle_error("setsockopt()");  
        }

        // init addr
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port_no);
        serv_addr.sin_addr.s_addr =  INADDR_ANY;

        if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
            handle_error("bind()");
        }

        if (listen(server_fd, LISTEN_BACKLOG) == -1){
            handle_error("listen()");
        }

        len = sizeof(client_addr);

        printf("No client connect to server\n");

        new_socket_fd  = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t *)&len); 
        printf("Server : got connection \n");
        //chat_func(new_socket_fd);
        printf("%d\n",new_socket_fd);

        int ret;
        Node* head = NULL;

        if(ret = pthread_create(&tid1,NULL,&thread_handle,&head))
        {
            printf("Error to create the thread 1.\n");
            return -1;
        }

        if(ret = pthread_create(&tid2,NULL,&thread_handle,&head))
        {
            printf("Error to create the thread 2.\n");
            return -1;
        }

        if(ret = pthread_create(&tid3,NULL,&thread_handle,&head))
        {
            printf("Error to create the thread 3.\n");
            return -1;
        }

        while(1){
            

        }
        printf("done");

        //free_list(head);

    }

    return 0;
}