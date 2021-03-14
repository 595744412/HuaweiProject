#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Server.h"
#include "DataManager.h"
using namespace std;

//����������
struct ServerType
{
	//�������ͺ�
	string name;
	//ÿ���ڵ�CPU�������ܺ���/2��
	unsigned int cores;
	//ÿ���ڵ��ڴ棨���ڴ�/2��
	unsigned int memory;
	//Ӳ���ɱ�
	unsigned int price;
	//ÿ���ܺĳɱ�
	unsigned int costPerDay;
};

//���������
struct VmwareType
{
	//������ͺ�
	string name;
	//ռ��CPU����
	unsigned int cores;
	//ռ���ڴ�
	unsigned int memory;
	//�Ƿ�˫�ڵ㲿��
	bool isDouble;
};

//��������
struct RequestType
{
	//�Ƿ�������������Ϊɾ����
	bool isAdd;
	//������ͺ�
	string name;
	//�����ID
	unsigned int ID;
};

//��������������
struct NodeData
{
	//��ռ��CPU����
	unsigned int usedCores;
	//ʣ��CPU����
	unsigned int unusedCores;
	//��ռ���ڴ�
	unsigned int usedMemory;
	//ʣ���ڴ�
	unsigned int unusedMemory;
	//�Ѵ�������
	unordered_map<unsigned int, Vmware&> vmwares;
};

//�������Ϣ
struct Vmware
{
	//�����ID
	unsigned int id;
	//���������
	VmwareType myType;
	//�洢��������ţ�-1Ϊδ���䣩
	unsigned int serverID = -1;
	//�Ƿ�洢��A�ˣ���洢��B�ˣ�
	bool isNodeA = true;
};