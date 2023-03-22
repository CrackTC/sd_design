/*
 * FileName: data/journal.h
 * Author: 陈若珂
 * Description: 包含日志条目的相关定义和声明
 * */

#ifndef JOURNAL_H
#define JOURNAL_H

#include "linkedList.h"
#include "operation.h"
#include "time.h"

/*
 * Name: JournalEntry
 * Description: 表示一个日志条目
 * */
typedef struct JournalEntry JournalEntry;

/*
 * Name: NewJournalEntry
 * Description: 创建一个新的日志条目
 * Arguments:
 *     staffId: 员工编号
 *     time: 时间
 *     operation: 操作
 *     arguments: 操作相关参数
 * */
JournalEntry *NewJournalEntry(int staffId, Time *time, Operation operation, const char **arguments, int argumentCount);

/*
 * Name: FreeJournalEntry
 * Description: 释放指定的JournalEntry占用的空间
 * Arguments:
 *     entry: 一个指针，指向要释放空间的JournalEntry对象
 * */
void FreeJournalEntry(JournalEntry *entry);

/*
 * Name: ExplodeArguments
 * Description: 将拼接的参数信息分割
 * Arguments:
 *     joinedArgument: 拼接的参数信息
 *     argumentCount: 参数个数
 * Returns: 分割后的参数信息
 * */
char **ExplodeArguments(const char *joinedArgument, int argumentCount);

/*
 * Name: JoinArguments
 * Description: 将分割的参数信息拼接
 * Arguments:
 *     arguments: 分割的参数信息
 *     argumentCount: 参数个数
 * Returns: 拼接后的参数信息
 * */
char *JoinArguments(char **arguments, int argumentCount);

/*
 * Name: GetAllJournals
 * Description: 获取所有日志条目
 * Returns: 一个链表，包含所有日志条目
 *          若不存在日志条目，返回NULL
 * */
LinkedList *GetAllJournals();

/*
 * Name: GetJournalsByStaffId
 * Description: 获取具有指定员工编号的所有日志条目
 * Arguments:
 *     staffId: 指定的员工编号
 * Returns: 一个链表，包含所有符合条件的日志条目
 *          若不存在这样的日志条目，返回NULL
 * */
LinkedList *GetJournalsByStaffId(int staffId);

/*
 * Name: GetJournalsByOperation
 * Description: 获取具有指定操作的所有日志条目
 * Arguments:
 *     operation: 指定的操作
 * Returns: 一个链表，包含所有符合条件的日志条目
 *          若不存在这样的日志条目，返回NULL
 * */
LinkedList *GetJournalsByOperation(Operation operation);

/*
 * Name: GetJournalEntryXXX
 * Description: 获取日志条目的某个成员
 * Arguments:
 *     entry: 指向JournalEntry的指针
 * Returns: 对应的值
 * */
int GetJournalEntryStaffId(const JournalEntry *entry);
Time GetJournalEntryTime(const JournalEntry *entry);
Operation GetJournalEntryOperation(const JournalEntry *entry);
char **GetJournalEntryArguments(const JournalEntry *entry);
int GetJournalEntryArgumentCount(const JournalEntry *entry);

/*
 * Name: AppendJournalEntry
 * Description: 向系统中追加一个JournalEntry对象
 * Arguments:
 *     entry: 一个指针，指向要追加的对象
 * Returns: 若成功追加，返回0
 *          否则，返回1
 * */
int AppendJournalEntry(JournalEntry *entry);

/*
 * Name: JournalSave
 * Description: 保存对日志条目的所有更改
 * */
void JournalSave();

#endif
