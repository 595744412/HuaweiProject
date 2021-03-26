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
	//ɾ����������Ƿ���յ�����ڴ��CPU
	const unsigned thre = 190;
	//�Ƿ�Ǩ�Ƶ���ֵ
	const float threMove = logf(25);
	//������ͺŶ�Ӧ����������ͺŵ�ӳ��
	unordered_map<string, string> vmwareToServer;
	vector<unsigned> waitMoveS;  //������Ϊʹ�ö�̫С����Ҫ��յķ������б�
	vector<unsigned> waitMoveV;  //������Ϊ������������·�����ʣ��˴�ȹ�����С����ҪǨ�Ƶ�������б�
	void init(void);
public:
	//���������������б�
	void CreateList();
	void moveFun(const unsigned vmwareID, pair<int, int>& goal, int day);
	bool myFind(unsigned vmwareID, pair<int, int>& goal, int jumpSeq);
	void update(const unsigned serverID);
	void mySort(vector<unsigned>& list);
};

