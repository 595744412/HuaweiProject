#pragma once
#include "definition.h"

//���Ŀ����ࣨ�����������������ɲ����б�
class Controller
{
private:
	//�����пռ��ŵķ������б�
	vector<unsigned int> usedServerList;
	//����һ̨����������ӵ��б����ط�������������
	Server& PurchaseServer(string type);
	//���Ŀ�����ֵ
	const unsigned int leastCore = 10;
	//�ڴ������ֵ
	const unsigned int leastMemory = 10;
public:
	//���������������б�
	void CreateList();
};

