#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include<algorithm>
#define isVisual false
using namespace std;

class DataManager;
class Server;
class Controller;
extern DataManager dataManager;

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
	//ƽ���ɱ�
	float cost;
	//CPU/�ڴ��
	float ratio;
	//���˴��
	static float maxratio;
	//��С�˴��
	static float minratio;
	//ServerType(string _name = " ", unsigned int _cores = 1, unsigned int _memory = 1, unsigned int _price = 1, unsigned int _costPerDay = 1) :
	//	name(_name), cores(_cores), memory(_memory), price(_price), costPerDay(_costPerDay) {
	//	cost = (float)(_price + _costPerDay * valueCoeff) / (_cores + _memory);
	//	ratio = (float)_cores / _memory;
	//	if (ratio > maxratio) {
	//		maxratio = ratio;
	//	}
	//	else if (ratio < minratio) {
	//		minratio = ratio;
	//	}
	//}
	ServerType(string _name = " ", unsigned int _cores = 1, unsigned int _memory = 1, unsigned int _price = 1, unsigned int _costPerDay = 1) :
		name(_name), cores(_cores), memory(_memory), price(_price), costPerDay(_costPerDay) {}
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
	//CPU/�ڴ��
	float ratio;
	//VmwareType(string _name = " ", unsigned int _cores = 0, unsigned int _memory = 0, bool _isDouble = true) :
	//	name(_name), cores(_cores), memory(_memory), isDouble(_isDouble) {
	//	ratio = (float)_cores / _memory;
	//	if (ratio > maxratio) {
	//		maxratio = ratio;
	//	}
	//	else if (ratio < minratio) {
	//		minratio = ratio;
	//	}
	//}
	VmwareType(string _name = " ", unsigned int _cores = 0, unsigned int _memory = 0, bool _isDouble = true) :
		name(_name), cores(_cores), memory(_memory), isDouble(_isDouble) {}
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
	RequestType(bool _isAdd = true, string _name = " ", unsigned int _ID = 0) :
		isAdd(_isAdd), name(_name), ID(_ID) {}
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
	//�Ƿ�洢��A�ڵ㣨��洢��B�ڵ㣩
	bool isNodeA = true;
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
	vector<unsigned int> vmwares;
	//�˴��
	float ratio;
};

//Ǩ�Ʋ�������
struct MoveData
{
	//�����id
	unsigned int vmwareID;
	//������id
	unsigned int serverID;
	//�Ƿ�˫�ڵ㲿��
	bool isDouble;
	//�Ƿ�Ǩ��ڵ�A����Ǩ��ڵ�B��
	bool isNodeA;
};

//�����������������
struct AddData
{
	//������id
	unsigned int serverID;
	//�Ƿ�˫�ڵ㲿��
	bool isDouble;
	//�Ƿ���ڵ�A������ڵ�B��
	bool isNodeA;
};

//��¼�������仯��Ϣ
struct ChangeData
{
	//������id
	unsigned int serverID;
	//A�ڵ���Ϣ
	unsigned coresA;
	unsigned memoryA;
	//B�ڵ���Ϣ
	unsigned coresB;
	unsigned memoryB;
};