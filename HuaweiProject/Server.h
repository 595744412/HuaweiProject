#pragma once
#include "definition.h"



//������
class Server
{
	//������ID
	unsigned int id;
	//����������
	static unsigned int count;
	//�÷���������
	ServerType myType;
	//A�ڵ�����
	NodeData nodeA;
	//B�ڵ�����
	NodeData nodeB;
	//��A/B�ڵ��ź˺��ڴ�
	void AddIntoNode(unsigned int cores, unsigned int memory, NodeData& node);
	//��A/B�ڵ�ɾ���˺��ڴ�
	void DeleteIntoNode(unsigned int cores, unsigned int memory, NodeData& node);
public:
	//���캯��
	Server(ServerType serverType);
	//���һ̨������������Ƿ�ɹ�
	bool AddVmware(unsigned int id, bool addToA);
	//ɾ��һ̨������������Ƿ�ɹ�
	bool DeleteVmware(unsigned int id);
	//��ȡID
	unsigned int GetID() { return id; }
	//��ȡA����Ϣ
	NodeData GetA() { return nodeA; }
	//��ȡB����Ϣ
	NodeData GetB() { return nodeB; }
	//��ȡ����������
	ServerType GetServerType() { return myType; }

};



