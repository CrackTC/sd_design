#include "design/linkedList.h"
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
    if (head == NULL)
    {
        return NULL;
    }
    if (head->next == NULL)
    {
        return head;
    }
    return GetLastNode(head->next);
}

int ExistsNode(LinkedList *list, const void *node)
{
    if (list == NULL)
    {
        return 0;
    }
    if (list->data == node)
    {
        return 1;
    }
    return ExistsNode(list->next, node);
}

LinkedList *AppendData(LinkedList *head, void *data)
{
    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = data;
    node->next = NULL;
    if (head == NULL)
    {
        return node;
    }
    GetLastNode(head)->next = node;
    return head;
}

LinkedList *RemoveNode(LinkedList *head, void *data)
{
    if (data == NULL)
    {
        return head;
    }
    if (head->data == data)
        return head->next;

    LinkedList *now = head;
    while (now->next != NULL)
    {
        if (now->next->data == data)
        {
            now->next = now->next->next;
            return head;
        }
        now = now->next;
    }
    return head;
}

void FreeList(LinkedList *list)
{
    if (list == NULL)
        return;
    if (list->next != NULL)
        FreeList(list->next);
    free(list);
}
