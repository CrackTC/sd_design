#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H
#include "../data/table.h"

Table *UpdateInventory(Table *input);   // �������ϵͳ�еĻ����Ƿ���ڲ������ڵĻ���������ϵͳ��
Table *ShowLackInventory(Table *input); // չʾ����ȱ�Ļ�����Ϣ

// ��ĳһ��ϵͳ�����ĳһ���ṹ��ĺ���
Table *AddInventory(Table *input);     // �����ϵͳ�����һ������
Table *AddItem(Table *input);          // ���һ����Ʒ����
Table *AddLossInventory(Table *input); // ����һ��������Ϣ

// ���Ҳ�չʾĳһ����Ϣ�ĺ���
Table *ShowSingleInventoryById(Table *input);     // չʾĳһ�������ȫ����Ϣ �Ի�����Ϊ����
Table *ShowSingleInventoryByItemId(Table *input); // չʾͬһ����Ʒ��ŵ�ȫ��������Ϣ ����Ʒ���Ϊ����
Table *ShowAllInventoryByItemName(Table *input); // չʾͬһ����Ʒ���Ƶ�ȫ��������Ϣ ����Ʒ����Ϊ����
Table *ShowSingleItemByItemname(Table *input);   // չʾĳһ����Ʒ��ȫ����Ϣ ����Ʒ����Ϊ����
Table *ShowSingleItemByItemId(Table *input);     // չʾĳһ����Ʒ��ȫ����Ϣ ����Ʒ���Ϊ����
Table *ShowSingleLossById(Table *input); // չʾһ���������Ϣ �Ի�����Ϊ����

// ɾ��ĳһ��ϵͳ�е�һ���ṹ����Ϣ�ĺ���
Table *DeleteInventoryById(Table *input); // ɾ������ϵͳ�е�ĳһ�������ȫ����Ϣ �Ի�����Ϊ����
Table *DeleteSingleLossInventoryById(Table *input); // ɾ��ĳһ��������Ϣ
Table *DeleteItemById(Table *input);           // ɾ��һ����Ʒ���� ����Ʒ���Ϊ����

// �޸�ĳһ��ϵͳ��һ���ṹ�����Ϣ�ĺ���
Table *ReviseInventory(Table *input); // �޸Ļ���ϵͳ�е�ĳһ�������ȫ����Ϣ �Ի�����Ϊ����
Table *ReviseAnItemByItemName(Table *input); // �޸�ĳһ����Ʒ��ȫ����Ϣ ����Ʒ����Ϊ����
Table *ReviseAnItemByItemId(Table *input);   // �޸�ĳһ����Ʒ��ȫ����Ϣ ����Ʒ���Ϊ����
Table *ReviseLossInventory(Table *input);    // �޸�ĳһ��������Ϣ

// չʾĳһ��ϵͳ���������ȫ����Ϣ
Table *ShowInventory(Table *input);     // չʾ���еĻ���
Table *ShowItem(Table *input);          // չʾ���е���Ʒ
Table *ShowLossInventory(Table *input); // չʾ���еĻ�����Ϣ

#endif
