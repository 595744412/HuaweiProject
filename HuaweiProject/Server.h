#pragma once
#include "definition.h"

//������
class Server
{
	//������ID
	unsigned int id;
	//�÷���������
	ServerType& myType;
	//��ռ��CPU����
	unsigned int usedCores;
	//ʣ��CPU����
	unsigned int unusedCores;
	//��ռ���ڴ�
	unsigned int usedMemory;
	//ʣ���ڴ�
	unsigned int unusedMemory;
	//�Ѵ�������id
	vector<unsigned int> vmwares;

public:

};

