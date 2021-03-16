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
	//����
	unsigned int dayCounts;
	//ÿ�չ���������б�[����][�ͺ�]
	unordered_map<string, unsigned int> purchaseList[1000];
	//������ID��ӳ��
	unordered_map<unsigned int, unsigned int> serverIDList;
	//ÿ��Ǩ��������б�[����][Ǩ�����]
	vector<MoveData> moveList[1000];
	//ÿ�ղ���������б�[����][�������]
	vector<AddData> addList[1000];
	//�����map
	unordered_map<unsigned int, Vmware> vmwareList;
	//�������б�
	vector<Server> serverList;
	//��ȡ��������
	void ReadAll();
	//�������
	void OutputAll();
	//������ӻ���������
	void OutputVisual();
};