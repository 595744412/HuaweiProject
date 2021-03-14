#pragma once
#include "Server.h"
class DataManager
{
	//����������map
	unordered_map<string, ServerType> serverTypeList;
	//���������map
	unordered_map<string, VmwareType> vmwareTypeList;
	//ÿ�������б�[����][�������]
	vector<RequestType> requestList[1000];
	//�����map
	unordered_map<unsigned int, VmwareType&> vmwareList;
	//������map
	unordered_map<unsigned int, Server> vmwareList;
public:
	//��ȡ��������
	void ReadAll();
};

