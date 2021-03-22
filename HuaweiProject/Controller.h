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
	const unsigned int leastCore = 2;
	//�ڴ������ֵ
	const unsigned int leastMemory = 2;
	//ɾ����������Ƿ���յ�����ڴ��CPU
	const unsigned thre = 190;
	//������ͺŶ�Ӧ����������ͺŵ�ӳ��
	unordered_map<string, string> vmwareToServer;
	vector<unsigned> waitMoveS; //������ҪǨ�Ƶķ������б�
	vector<unsigned> waitMoveV;  //������ҪǨ�Ƶ���������б�
	void init(void);
public:
	//���������������б�
	void CreateList();
	void moveFun(const unsigned vmwareID, pair<int, int>& goal, int days);
	void process(unsigned vmwareID, pair<int, int>& goal, bool& move, int jumpSeq);
};

