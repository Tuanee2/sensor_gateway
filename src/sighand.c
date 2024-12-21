#include "sighand.h"

void signal_handler00(int signum) {
    // In thông báo tùy thuộc vào signal nhận được
    printf("Tiến trình con đã kết thúc\n");
    exit(0);
}

void signal_intallchild(pid_t arr[],int arrSize){
    
    for (int i = 0; i < arrSize; i++)
    {
        kill(arr[i],SIGINT);
    }
    printf("completed to kill all process\n");

}
