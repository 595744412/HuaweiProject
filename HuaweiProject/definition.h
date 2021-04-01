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

//服务器类型
struct ServerType
{
	//服务器型号
	string name;
	//每个节点CPU核数（总核数/2）
	unsigned int cores;
	//每个节点内存（总内存/2）
	unsigned int memory;
	//硬件成本
	unsigned int price;
	//每日能耗成本
	unsigned int costPerDay;
	//平均成本
	float cost;
	//CPU/内存比
	float ratio;
	//最大核存比
	static float maxratio;
	//最小核存比
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


//虚拟机类型
struct VmwareType
{
	//虚拟机型号
	string name;
	//占用CPU核数
	unsigned int cores;
	//占用内存
	unsigned int memory;
	//是否双节点部署
	bool isDouble;
	//CPU/内存比
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


//请求类型
struct RequestType
{
	//是否添加虚拟机（否为删除）
	bool isAdd;
	//虚拟机型号
	string name;
	//虚拟机ID
	unsigned int ID;
	RequestType(bool _isAdd = true, string _name = " ", unsigned int _ID = 0) :
		isAdd(_isAdd), name(_name), ID(_ID) {}
};

//虚拟机信息
struct Vmware
{
	//虚拟机ID
	unsigned int id;
	//虚拟机类型
	VmwareType myType;
	//存储服务器编号（-1为未分配）
	unsigned int serverID = -1;
	//是否存储在A节点（否存储在B节点）
	bool isNodeA = true;
};

//服务器单核数据
struct NodeData
{
	//已占用CPU核数
	unsigned int usedCores;
	//剩余CPU核数
	unsigned int unusedCores;
	//已占用内存
	unsigned int usedMemory;
	//剩余内存
	unsigned int unusedMemory;
	//已存放虚拟机
	vector<unsigned int> vmwares;
	//核存比
	float ratio;
};

//迁移操作数据
struct MoveData
{
	//虚拟机id
	unsigned int vmwareID;
	//服务器id
	unsigned int serverID;
	//是否双节点部署
	bool isDouble;
	//是否迁入节点A（否迁入节点B）
	bool isNodeA;
};

//部署虚拟机操作数据
struct AddData
{
	//服务器id
	unsigned int serverID;
	//是否双节点部署
	bool isDouble;
	//是否部署节点A（否部署节点B）
	bool isNodeA;
};

//记录服务器变化信息
struct ChangeData
{
	//服务器id
	unsigned int serverID;
	//A节点信息
	unsigned coresA;
	unsigned memoryA;
	//B节点信息
	unsigned coresB;
	unsigned memoryB;
};