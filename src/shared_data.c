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
    newNode->client_sock = 0;  // Giá trị mặc định
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

Node* createNode(const char* timestamp, float temperature,int sensor_id,int client_sock) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->client_sock = client_sock;
    newNode->sensor_id = sensor_id;
    strcpy(newNode->timestamp, timestamp);
    newNode->temperature = temperature;
    newNode->next = NULL;
    return newNode;
}

void append(LinkedList* list, const char* timestamp, float temperature,int sensor_id,int client_sock) {
    Node* newNode = createNode(timestamp, temperature,sensor_id,client_sock);
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
    printf("List info:\n");
    while(current != NULL){
        printf("timestamp : %s\n",current->timestamp);
        printf("temp :%f\n",current->temperature);
        current = current->next;
    }
    printf("---END---\n");
    free(current);
}