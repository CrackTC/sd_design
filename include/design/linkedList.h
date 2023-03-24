/*
 * FileName: data/linkedList.h
 * Author: 陈若珂
 * Description: 包含链表的基本表示和操作的声明
 * */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/*
 * Name: LinkedListNode
 * Description: 表示一个链表结点
 * */
typedef struct LinkedList
{
    // 指向要存储的数据的指针
    void *data;

    // 指向下一链表结点的指针
    struct LinkedList *next;
} LinkedList;

/*
 * Name: GetNodeByIndex
 * Description: 使用索引获取链表结点
 * Arguments:
 *     head: 链表的头指针
 *     index: 索引
 * Returns: 指向链表的第index个结点的指针
 *          若index超出范围，则返回NULL
 * */
LinkedList *GetNodeByIndex(LinkedList *head, int index);

/*
 * Name: GetLastNode
 * Description: 获取链表最后一个结点
 * Arguments:
 *     head: 链表的头指针
 * Returns: 链表的最后一个结点
 *          若head为NULL，返回NULL
 * */
LinkedList *GetLastNode(LinkedList *head);

/*
 * Name: ExistsNode
 * Description: 判断指定链表是否存在包含指定数据的结点
 * Arguments:
 *     list: 指定的列表
 *     node: 指定的数据
 * Returns: 若存在这样的结点，返回1，否则返回0
 * */
int ExistsNode(LinkedList *list, const void *node);

/*
 * Name: AppendData
 * Description: 将结点添加到链表尾
 * Arguments:
 *     head: 链表的头指针
 *     data: 要添加的结点所包含的数据
 * Returns: 添加结点后链表的头指针
 *          若head为NULL，则返回node
 * */
LinkedList *AppendData(LinkedList *head, void *data);

/*
 * Name: RemoveNode
 * Description: 删除包含指定数据的链表结点
 * Arguments:
 *     head: 链表的头指针
 *     data: 指定的数据
 * Returns: 删除指定结点后的链表
 *
 * */
LinkedList *RemoveNode(LinkedList *head, void *data);

/*
 * Name: FreeList
 * Desciption: 释放指定的链表占用的空间
 * Arguments:
 *     list: 要释放空间的链表的头指针
 * */
void FreeList(LinkedList *list);

#endif
