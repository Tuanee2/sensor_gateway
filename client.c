#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define BUFF_SIZE 256
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

/* Chức năng gửi thông tin */
void send_info(int server_fd)
{
    int numb_write;
    char sendbuff[BUFF_SIZE],roomid[10];
    time_t rawtime;
    struct tm *timeinfo;
    char timebuff[80]; // Bộ đệm để chứa thời gian dưới dạng chuỗi
    // room id
    memset(roomid, '0', 10);
    printf("Please enter the room id: ");
    fgets(roomid, 10, stdin);
    roomid[strcspn(roomid, "\n")] = 0;

    while (1) {
        memset(sendbuff, '0', BUFF_SIZE);
        printf("Please enter the temperature: ");
        fgets(sendbuff, BUFF_SIZE, stdin);
        sendbuff[strcspn(sendbuff, "\n")] = 0; // Loại bỏ ký tự xuống dòng

        // Lấy thời gian hiện tại
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timebuff, sizeof(timebuff), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Gộp thời gian và nhiệt độ vào cùng một chuỗi, thời gian trước, nhiệt độ sau
        char final_message[BUFF_SIZE + 95 ];
        snprintf(final_message, BUFF_SIZE + 95, "%s - %s - %s\n", timebuff, roomid, sendbuff);

        /* Gửi thông điệp tới server bằng hàm write */
        numb_write = write(server_fd, final_message, strlen(final_message));
        if (numb_write == -1)
            handle_error("write()");
        if (strncmp("exit", final_message, 4) == 0) {
            printf("Client exit ...\n");
            break;
        }
    }
    close(server_fd); /* close */
}

int main(int argc, char *argv[])
{
    int portno;
    int server_fd;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    /* Đọc port number từ command line */
    if (argc < 3) {
        printf("command : ./client <server address> <port number>\n");
        exit(1);
    }
    portno = atoi(argv[2]);

    /* Khởi tạo địa chỉ server */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(portno);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) == -1)
        handle_error("inet_pton()");

    /* Tạo socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        handle_error("socket()");

    /* Kết nối tới server */
    if (connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("connect()");

    send_info(server_fd);

    return 0;
}