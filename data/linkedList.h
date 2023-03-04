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
typedef struct LinkedList {
    // 指向要存储的数据的指针
    void const *data;

    // 指向下一链表结点的指针
    struct LinkedList *next;
} LinkedList;

/*
 * Name: LinkedListNodePointer
 * Description: 表示一个指向链表结点的指针
 * */
typedef LinkedList *LinkedListPointer;

/*
 * Name: GetNodeByIndex
 * Description: 使用索引获取链表结点
 * Arguments:
 *     head: 链表的头指针
 *     index: 索引
 * Returns: 指向链表的第index个结点的指针
 *          若index超出范围，则返回NULL
 * */
LinkedListPointer GetNodeByIndex(const LinkedListPointer head, int index);

/*
 * Name: GetLastNode
 * Description: 获取链表最后一个结点
 * Arguments:
 *     head: 链表的头指针
 * Returns: 链表的最后一个结点
 *          若head为NULL，返回NULL
 * */
LinkedListPointer GetNodeByIndex(const LinkedListPointer head, int index);

/*
 * Name: NodePredicate
 * Description: 表示作用于链表结点的谓词，用于指示给定的结点是否符合某些条件
 * Arguments:
 *     pointer: 指向链表结点的指针
 * Returns: 若结点符合条件，返回1
 *          否则，返回0
 * */
typedef int (*NodePredicate)(LinkedListPointer pointer);

/*
 * Name: GetNodeByPredicate
 * Description: 使用指定的谓词获取链表结点
 * Arguments:
 *     head: 链表的头指针
 *     predicate: 谓词
 * Returns: 指向符合predicate条件的第一个结点
 *          若不存在这样的结点，返回NULL
 * */
LinkedListPointer GetNodeByPredicate(const LinkedListPointer head, NodePredicate predicate);


/*
 * Name: AppendNode
 * Description: 将结点添加到链表尾
 * Arguments:
 *     head: 链表的头指针
 *     node: 要添加的结点
 * Returns: 添加结点后链表的头指针
 *          若head为NULL，则返回node
 * */
LinkedListPointer AppendNode(LinkedListPointer head, LinkedListPointer node);

#endif
