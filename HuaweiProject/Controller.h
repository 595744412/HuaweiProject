#pragma once
#include "definition.h"


//���Ŀ����ࣨ�����������������ɲ����б�
class Controller
{
private:
	//�����������б����˴������
	vector<ServerType> ratioServerList;
	//�����пռ��ŵķ������б�
	vector<unsigned int> usedServerList;
	//����һ̨����������ӵ��б����ط�������������
	Server& PurchaseServer(string type);
	//���Ŀ�����ֵ
	const unsigned int leastCore = 5;
	//�ڴ������ֵ
	const unsigned int leastMemory = 5;
	//������ͺŶ�Ӧ����������ͺŵ�ӳ��
	unordered_map<string, string> vmwareToServer;
	void init(void);
public:
	//���������������б�
	void CreateList();
	void moveFun(const unsigned vmwareID, pair<int, int>& goal, int days);
	void process(VmwareType& type, pair<int, int>& goal, bool& move, int jumpSeq);
};

