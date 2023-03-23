#ifndef JOURNALH
#define JOURNALH

#include "../data/journal.h"
#include "../data/linkedList.h"
#include "../data/table.h"

// LinkedList* GetOneJournalInLinkedList(int staffid, Operation operation, Time time);

/*写一条日志*/
Table *AddJournal(Table *table, int staffid, Operation operation);

/*读某一段时间内的某操作的全部记录
** 这里参数Table*a 要有一个初始时间，一个末尾时间*/
Table *GetAllJournalOfOneOperationInExactTime(Table *table);

/*读取某操作的所有日志*/
Table *GetAllJournalOfOneOperation(Table *table);

/*读取某员工的所有日志*/
Table *GetAllJournalOfOneStaff(Table *table);

/*读取某员工的某操作所有日志*/
Table *GetAllJournalOfOneStaffOfOneOperation(Table *table);

/*读取全部日志*/
Table *GetAllJournal(__attribute__((unused)) Table *table);

/*读取一条日志（根据员工id，操作，时间）*/
Table *GetOneJournalByIdOperationTime(Table *table);

/*将数字字符串转化为int型整数*/
int atoi(const char *ch);

/*读取‘=’之前的字符*/
char *getStringBefore(char *ch, char *string);

#endif
