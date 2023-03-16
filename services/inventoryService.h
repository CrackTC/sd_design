#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H
#include "../data/table.h"

Table *UpdateInventory();   // �������ϵͳ�еĻ����Ƿ���ڲ������ڵĻ���������ϵͳ��
Table *ShowLackInventory(); // չʾ����ȱ�Ļ�����Ϣ

// ��ĳһ��ϵͳ�����ĳһ���ṹ��ĺ���
Table *AddInventory(Table *a);     // �����ϵͳ�����һ������
Table *AddItem(Table *a);          // ���һ����Ʒ����
Table *AddLossInventory(Table *a); // ����һ��������Ϣ

// ���Ҳ�չʾĳһ����Ϣ�ĺ���
Table *ShowSingleInventoryById(Table *a);     // չʾĳһ�������ȫ����Ϣ �Ի�����Ϊ����
Table *ShowSingleInventoryByItemId(Table *a); // չʾͬһ����Ʒ��ŵ�ȫ��������Ϣ ����Ʒ���Ϊ����
Table *ShowAllInventoryByItemName(Table *a); // չʾͬһ����Ʒ���Ƶ�ȫ��������Ϣ ����Ʒ����Ϊ����
Table *ShowSingleItemByItemname(Table *a);   // չʾĳһ����Ʒ��ȫ����Ϣ ����Ʒ����Ϊ����
Table *ShowSingleItemByItemId(Table *a);     // չʾĳһ����Ʒ��ȫ����Ϣ ����Ʒ���Ϊ����
Table *ShowSingleLossInventoryById(Table *a); // չʾһ���������Ϣ �Ի�����Ϊ����

// ɾ��ĳһ��ϵͳ�е�һ���ṹ����Ϣ�ĺ���
Table *DeleteInventoryById(Table *a); // ɾ������ϵͳ�е�ĳһ�������ȫ����Ϣ �Ի�����Ϊ����
Table *DeleteALossInventoryById(Table *a); // ɾ��ĳһ��������Ϣ
Table *DeleteItemByName(Table *a);         // ɾ��һ����Ʒ���� ����Ʒ����Ϊ����
Table *DeleteItemById(Table *a);           // ɾ��һ����Ʒ���� ����Ʒ���Ϊ����

// �޸�ĳһ��ϵͳ��һ���ṹ�����Ϣ�ĺ���
Table *ReviseInventory(Table *a); // �޸Ļ���ϵͳ�е�ĳһ�������ȫ����Ϣ �Ի�����Ϊ����
Table *ReviseAnItemByItemName(Table *a); // �޸�ĳһ����Ʒ��ȫ����Ϣ ����Ʒ����Ϊ����
Table *ReviseAnItemByItemId(Table *a);   // �޸�ĳһ����Ʒ��ȫ����Ϣ ����Ʒ���Ϊ����
Table *ReviseLossInventory(Table *a);    // �޸�ĳһ��������Ϣ

// չʾĳһ��ϵͳ���������ȫ����Ϣ
Table *ShowInventory();     // չʾ���еĻ���
Table *ShowItem();          // չʾ���е���Ʒ
Table *ShowLossInventory(); // չʾ���еĻ�����Ϣ

#endif
