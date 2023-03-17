#ifndef JOURNALH
#define JOURNALH
#include "table.h"
#include"journalchen.h"
#include"Linkedlist.h"


//LinkedList* GetOneJournalInLinkedList(int staffid, Operation operation, Time time);

/*дһ����־*/
Table* AddJournal(Table* table, int staffid, Operation operation);
/*��ĳһ��ʱ���ڵ�ĳ������ȫ����¼
** �������Table*a Ҫ��һ����ʼʱ�䣬һ��ĩβʱ��*/
Table* GetAllJournalOfOneOperationInExactTime(Table* time,Operation operation);
/*��ȡĳ������������־*/
Table* GetAllJournalOfOneOperation(Operation operation);
/*��ȡĳԱ����������־*/
Table* GetAllJournalOfOneStaff(int staffid);
/*��ȡĳԱ����ĳ����������־*/
Table* GetAllJournalOfOneStaffOfOneOperation(int staffid, Operation operation);
/*��ȡȫ����־*/
Table* GetAllJournal(Table*);
/*��ȡһ����־������Ա��id��������ʱ�䣩*/
Table* GetOneJournalByIdOperationTime(int staffid, Operation operation, Time time);

/*�������ַ���ת��Ϊint������*/
int atoi(const  char* ch);
/*��int����ת��Ϊ�ַ���*/
const char* itoa(int a);

/*��ȡ��=��֮ǰ���ַ�*/
char* getStringBefore(char*);

#endif
