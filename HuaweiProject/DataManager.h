#pragma once
#include "definition.h"
class DataManager
{
	//����������map
	unordered_map<string, SeverType> severTypeList;
	//���������map
	unordered_map<string, VmwareType> vmwareTypeList;
	//ÿ�������б�[����][�������]
	vector<RequestType> requestList[1000];

public:
	//��ȡ��������
	void ReadAll();
};

