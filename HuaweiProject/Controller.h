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
public:
	//���������������б�
	void CreateList();
};

