#ifndef STAFFH
#define STAFFH

#include "../data/linkedList.h"
#include "../data/staff.h"
#include "../data/table.h"

/*Ա����¼*/
Table *LogIn(Table *); // ��������а�����password��Id�����ص�Table����Success��1/0��

/* ����һ��Ա�� */
Table *AddStaff();
/* ɾ��һ��Ա�� */
Table *RemoveStaff(Table *); // ����Table* ��Ҫ���� Ҫɾ��Ա����Ա������
/*��ѯһ��Ա������Ϣ*/
Table *GetItemOfOneStaff(Table *); // ����Table* ��Ҫ���� Ҫ��ѯԱ����Ա������
/*��ѯ����Ա������Ϣ*/
Table *GetItemOfAllStaff();
/*�ı�һ��Ա����������Ϣ*/
Table *UpdateStaff(Table *); // ����Table* ��Ҫ���� Ҫ�ı�Ա����Ա������

/*����Ա����Ϣ��ͷTitle*/
Table *CreateStaffTableTitle(void);

/*�������ַ���ת��Ϊint������*/
int atoi(const char *ch);
/*��int����ת��Ϊ�ַ���*/
const char *itoa(int a);

#endif
