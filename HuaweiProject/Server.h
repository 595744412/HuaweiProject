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
	bool DeleteIntoNode(Vmware& vmware, NodeData& node);
public:
	//ʹ�õĳ̶�
	unsigned usuage;
	//���캯��
	Server(ServerType serverType);
	//���һ̨������������Ƿ�ɹ�
	bool AddVmwareA(unsigned int vmwareid);
	bool AddVmwareB(unsigned int vmwareid);
	bool AddVmwareD(unsigned int vmwareid);
	//ɾ��һ̨������������Ƿ�ɹ�
	bool DeleteVmware(unsigned int vmwareid);
	//��ȡID
	unsigned int GetID() { return id; }
	//��ȡcount
	static unsigned int GetCount() { return count; }
	//��ȡA����Ϣ
	NodeData& GetA() { return nodeA; }
	//��ȡB����Ϣ
	NodeData& GetB() { return nodeB; }
	//��ȡ����������
	ServerType& GetServerType() { return myType; }
	//�Ƿ�Ϊ��
	bool isEmpty();

};



