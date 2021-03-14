#pragma once
#include "definition.h"
extern DataManager dataManager;

//������
class Server
{
	//������ID
	unsigned int id;
	//�÷���������
	ServerType& myType;
	//A������
	CoreData coreA;
	//B������
	CoreData coreB;
	//��A/B�˴�ź˺��ڴ�
	void AddIntoCore(unsigned int cores, unsigned int memory, CoreData& core);
	//��A/B��ɾ���˺��ڴ�
	void DeleteIntoCore(unsigned int cores, unsigned int memory, CoreData& core);

public:
	//���һ̨������������Ƿ�ɹ�
	bool AddVmware(unsigned int id, bool addToA);
	//���һ̨������������Ƿ�ɹ�
	bool DeleteVmware(unsigned int id);
};

