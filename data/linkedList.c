#include "linkedList.h"
#include <stddef.h>
#include <stdlib.h>

LinkedList *GetNodeByIndex(LinkedList *head, int index)
{
    if (index < 0 || head == NULL)
    {
        return NULL;
    }
    if (index == 0)
    {
        return head;
    }
    return GetNodeByIndex(head->next, index - 1);
}

LinkedList *GetLastNode(LinkedList *head)
{
    if (head == NULL) {
        return NULL;
    }
    if (head->next == NULL) {
        return head;
    }
    return GetLastNode(head->next);
}

int ExistsNode(LinkedList *list, const void *node)
{
    if (list == NULL) {
        return 0;
    }
    if (list->data == node) {
        return 1;
    }
    return ExistsNode(list->next, node);
}

LinkedList *AppendNode(LinkedList *head, LinkedList *node)
{
    node->next = head;
    return node;
}

LinkedList *RemoveNode(LinkedList *head, LinkedList *node)
{
    if (node == NULL) {
        return head;
    }
    if (head == node)
        return head->next;
    
    LinkedList *now = head;
    while (now != NULL) {
        if (now->next == node) {
            now->next = now->next->next;
            return head;
        }
        now = now->next;
    }
    return head;
}

void FreeList(LinkedList *list)
{
    if (list->next != NULL)
        FreeList(list->next);
    free(list);
}
