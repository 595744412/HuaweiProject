#pragma once
#include "definition.h"


class DataManager
{
public:
	float minRatioS = 100;
	float maxRatioS = 0;
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
	//ÿ�շ����������б�[����][�������]
	vector<Server> newList[1000];
	//ÿ�շ������仯�б�[����][�仯���]
	vector<ChangeData> changeList[1000];
	//�����map
	unordered_map<unsigned int, Vmware> vmwareList;
	//�������б�
	vector<Server> serverList;
	//�������Լ۱������б�performanceList)
	vector<string> pfmList;
	//�Լ۱ȣ�ԽС�Լ۱�Խ�ߣ�
	unordered_map<string, double> performance;
	//����ratio���е�����
	vector<string> ratioList;
	//��ȡ��������
	void ReadAll();
	//�������
	void OutputAll();
	//������ӻ���������
	void OutputVisual();
	//�Է��������Լ۱ȡ�ratio��������
	void init(unsigned int dayCounts);
};