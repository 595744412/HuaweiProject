#pragma once
#include "definition.h"


class DataManager
{
public:
	unsigned minCores = 100;	//��������������е���СCPU����
	unsigned minMemory = 100;   //��������������е���С�ڴ���
	float minRatioS = 100;      //���з����������е���С�˴�ȵ���Ȼ����
	float maxRatioS = -100;     //���з����������е����˴�ȵ���Ȼ����
	int vmSize;	                //��ǰ���������
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
	//������������Լ۱ȣ�ԽС�Լ۱�Խ�ߣ�
	unordered_map<string, double> performance;
	//���պ˴���������еķ������б�
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