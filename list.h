#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *prev;
    struct Node *next;
} Node;

void insertNode(Node **head, Node **tail, int data);
Node *deleteNode(Node *head, Node **tail);
void josephusLoop(Node **head, Node **tail, unsigned size, unsigned l,
                  unsigned r);