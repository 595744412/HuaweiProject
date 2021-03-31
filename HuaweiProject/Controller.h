#pragma once
#include "definition.h"


//���Ŀ����ࣨ�����������������ɲ����б�
class Controller
{
private:
	vector<ServerType> ratioServerList;  	      //�����������б����˴������	
	vector<unsigned int> usedServerList;          //�����пռ��ŵķ������б�	
	const unsigned thre = 190;					  //ɾ����������Ƿ���յ�����ڴ��CPU	
	const float threMove = logf(25);		      //�Ƿ�Ǩ�Ƶ���ֵ	
	const float moveRatio = 0.03;                 //ÿ������ƶ��ķ�������ռ��
	unordered_map<string, string> vmwareToServer; //������ͺŶ�Ӧ����������ͺŵ�ӳ��
	vector<unsigned> waitMoveS;                   //������Ϊʹ�ö�̫С����Ҫ��յķ������б�
	vector<unsigned> waitMoveV;                   //������Ϊ������������·�����ʣ��˴�ȹ�����С����ҪǨ�Ƶ�������б�

	void init(void);                              //��ʼ��
	Server& PurchaseServer(string type);          //����һ̨����������ӵ��б����ط�������������
public:
	//���������������б�
	void CreateList();
	void moveFun(const unsigned vmwareID, pair<int, int>& goal, int day);
	bool myFind(unsigned vmwareID, pair<int, int>& goal, int jumpSeq);
	void update(const unsigned serverID);
	void mySort(vector<unsigned>& list);
};

