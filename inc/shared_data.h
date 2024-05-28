// linked_list.h

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct Node {
    char ip[16];
    int sock;
    int connection;
    int sensor_id;
    char timestamp[20];  // Định dạng thời gian có thể là "YYYY-MM-DD HH:MM:SS"
    float temperature;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
    Node* tail;
} LinkedList;

Node* createEmptyNode();
int appendEmptyNode(LinkedList* list); 
Node* createNode(const char* timestamp, float temperature,int sensor_id,int sock,char* ip);
void append(LinkedList* list, const char* timestamp, float temperature,int sensor_id,int sock,char* ip);
Node* getNodeAt(LinkedList* list, int n);
Node* getNodeWithIp(LinkedList* list, char* ip);
Node* getNodeWithSock(LinkedList* list, int sock);
Node* getNodeWithId(LinkedList* list, int id);
Node* getFirst(LinkedList* list);
Node* getLast(LinkedList* list);
void deleteNode(LinkedList* list, const char* timestamp);
void deleteNodeWithId(LinkedList* list,int sensor_id);
void printList(LinkedList* list);

#endif // LINKED_LIST_H
