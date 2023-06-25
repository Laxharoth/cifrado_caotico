#include "list.h"

void insertNode(Node **head, Node **tail, int data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;

    if (*head == NULL) {
        newNode->prev = newNode;
        newNode->next = newNode;
        *head = newNode;
        *tail = newNode;
    } else {
        newNode->prev = *tail;
        newNode->next = *head;
        (*tail)->next = newNode;
        (*head)->prev = newNode;
        *tail = newNode;
    }
}

Node *deleteNode(Node *head, Node **tail) {
    // Verificar si la lista está vacía
    if (head == NULL) {
        printf("La lista está vacía.\n");
        return NULL;
    }

    Node *currentHead = head;
    Node *newHead = head->next;

    // Actualizar los enlaces de la nueva cabeza y la cola
    if (head == *tail) {
        *tail = NULL;
    } else {
        newHead->prev = (*tail);
        (*tail)->next = newHead;
    }

    // Liberar la memoria del nodo eliminado
    free(currentHead);

    return newHead;
}

void josephusLoop(Node **head, Node **tail, unsigned size, unsigned l,
                  unsigned r) {
    Node *nHead = NULL, *nTail = NULL;
    Node *rec = *head, *next;
    unsigned i;
    for (i = 1; i < r; ++i) rec = rec->next;
    while (size--) {
        for (i = 1; i < l; ++i) rec = rec->next;
        next = rec->next;
        if (rec = next) next = NULL;
        rec->prev->next = rec->next;
        rec->next->prev = rec->prev;
        if (nHead == NULL) {
            nHead = nTail = rec;
            continue;
        }
        nTail->next = rec;
        rec->prev = nTail;
        nTail = rec;
        if (next == NULL) break;
        rec = next;
    }
    nTail->next = nHead;
    nHead->prev = nTail;
    *head = nHead;
    *tail = nTail;
}