#ifndef JOURNALH
#define JOURNALH
#include "table.h"
#include"journalchen.h"
#include"Linkedlist.h"


//LinkedList* GetOneJournalInLinkedList(int staffid, Operation operation, Time time);

/*写一条日志*/
Table* AddJournal(Table* table, int staffid, Operation operation);
/*读某一段时间内的某操作的全部记录
** 这里参数Table*a 要有一个初始时间，一个末尾时间*/
Table* GetAllJournalOfOneOperationInExactTime(Table* time,Operation operation);
/*读取某操作的所有日志*/
Table* GetAllJournalOfOneOperation(Operation operation);
/*读取某员工的所有日志*/
Table* GetAllJournalOfOneStaff(int staffid);
/*读取某员工的某操作所有日志*/
Table* GetAllJournalOfOneStaffOfOneOperation(int staffid, Operation operation);
/*读取全部日志*/
Table* GetAllJournal(Table*);
/*读取一条日志（根据员工id，操作，时间）*/
Table* GetOneJournalByIdOperationTime(int staffid, Operation operation, Time time);

/*将数字字符串转化为int型整数*/
int atoi(const  char* ch);
/*将int整数转化为字符串*/
const char* itoa(int a);

/*读取‘=’之前的字符*/
char* getStringBefore(char*);

#endif
