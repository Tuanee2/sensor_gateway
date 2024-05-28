// linked_list.c

#include "shared_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node* createEmptyNode() {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    strcpy(newNode->ip, "");  // Giá trị mặc định
    newNode->sock = 0;
    newNode->connection = 0;
    newNode->sensor_id = 0;    // Giá trị mặc định
    strcpy(newNode->timestamp, ""); // Chuỗi rỗng
    newNode->temperature = 0.0; // Nhiệt độ mặc định
    newNode->next = NULL;
    return newNode;
}

int appendEmptyNode(LinkedList* list) {
    Node* newNode = createEmptyNode();
    int index = 0;  // Chỉ số bắt đầu từ 0
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
        // Nếu danh sách trước đó rỗng, nút mới là nút đầu tiên
        index = 0;
    } else {
        Node* current = list->head;
        // Duyệt qua danh sách để tìm nút cuối và đếm chỉ số
        while (current->next != NULL) {
            current = current->next;
            index++;
        }
        current->next = newNode;
        list->tail = newNode;
        index++;  // Tăng chỉ số lên để phản ánh vị trí của nút mới
    }
    return index;
}

Node* createNode(const char* timestamp, float temperature,int sensor_id,int sock,char* ip) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    strcpy(newNode->ip, ip);
    newNode->sock = sock;
    newNode->connection = 0;
    newNode->sensor_id = sensor_id;
    strcpy(newNode->timestamp, timestamp);
    newNode->temperature = temperature;
    newNode->next = NULL;
    return newNode;
}

void append(LinkedList* list, const char* timestamp, float temperature,int sensor_id,int sock,char* ip) {
    Node* newNode = createNode(timestamp, temperature,sensor_id,sock,ip);
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

Node* getNodeAt(LinkedList* list, int n) {
    Node* current = list->head;
    int count = 0;
    while (current != NULL && count < n) {
        current = current->next;
        count++;
    }
    return current;
}

Node* getNodeWithIp(LinkedList* list, char* ip) {
    Node* current = list->head;
    int count = 0;
    while ((current != NULL) && (strcmp(current->ip, ip) != 0)) {
        current = current->next;
        count++;
    }
    return current;
}

Node* getNodeWithSock(LinkedList* list, int sock) {
    Node* current = list->head;
    int count = 0;
    while ((current != NULL) && (current->sock != sock)) {
        current = current->next;
        count++;
    }
    return current;
}

Node* getNodeWithId(LinkedList* list, int id) {
    Node* current = list->head;
    int count = 0;
    while ((current != NULL) && (current->sensor_id != id)) {
        current = current->next;
        count++;
    }
    return current;
}

Node* getFirst(LinkedList* list) {
    return list->head;
}

Node* getLast(LinkedList* list) {
    return list->tail;
}

void deleteNode(LinkedList* list, const char* timestamp) {
    Node *current = list->head, *previous = NULL;
    while (current != NULL && strcmp(current->timestamp, timestamp) != 0) {
        previous = current;
        current = current->next;
    }
    if (current == NULL) return;  // Không tìm thấy
    if (previous == NULL) {
        list->head = current->next;  // Nếu nút cần xóa là đầu danh sách
    } else {
        previous->next = current->next;
    }
    if (current->next == NULL) {
        list->tail = previous;  // Nếu nút cần xóa là cuối danh sách
    }
    free(current);
}

void deleteNodeWithId(LinkedList* list,int sensor_id){
    Node *current = list->head, *previous = NULL;
    while(current != NULL && (current->sensor_id != sensor_id)){
        previous = current;
        current = current->next;
    }
    if (current == NULL) return;  
    if (previous == NULL) {
        list->head = current->next;  // Nếu nút cần xóa là đầu danh sách
    } else {
        previous->next = current->next;
    }
    if (current->next == NULL) {
        list->tail = previous;  // Nếu nút cần xóa là cuối danh sách
    }
    free(current);
}

void printList(LinkedList* list){
    Node *current = list->head;
    printf("\nList info:\n");
    while(current != NULL){
        printf("room : %d\n",current->sensor_id);
        printf("timestamp : %s\n",current->timestamp);
        printf("temp :%f\n",current->temperature);
        printf("sock : %d\n",current->sock);
        current = current->next;
    }
    printf("---END---\n\n");
    free(current);
}