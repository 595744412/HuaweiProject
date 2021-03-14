#pragma once
#include "definition.h"


class DataManager
{
public:
	//����������map
	unordered_map<string, ServerType> serverTypeList;
	//���������map
	unordered_map<string, VmwareType> vmwareTypeList;
	//ÿ�������б�[����][�������]
	vector<RequestType> requestList[1000];
	//�����map
	unordered_map<unsigned int, Vmware> vmwareList;
	//������map
	unordered_map<unsigned int, Server> vmwareList;
	//��ȡ��������
	void ReadAll(const string& filePath);
};