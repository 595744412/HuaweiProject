#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Server.h"
#include "DataManager.h"
using namespace std;

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
	ServerType(string _name=" ", unsigned int _cores=0, unsigned int _memory=0, unsigned int _price=0, unsigned int _costPerDay=0) :
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
	VmwareType(string _name=" ", unsigned int _cores=0, unsigned int _memory=0, bool _isDouble=true):
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
	RequestType(bool _isAdd=true, string _name=" ", unsigned int _ID=0) :
		isAdd(_isAdd), name(_name), ID(_ID) {}
};

//服务器单核数据
struct CoreData
{
	//已占用CPU核数
	unsigned int usedCores;
	//剩余CPU核数
	unsigned int unusedCores;
	//已占用内存
	unsigned int usedMemory;
	//剩余内存
	unsigned int unusedMemory;
	//已存放虚拟机id
	vector<unsigned int> vmwares;
};

//虚拟机信息
struct Vmware
{
	//虚拟机ID
	unsigned int id;
	//虚拟机类型
	VmwareType myType;
	//存储服务器编号（-1为未分配）
	int serverID = -1;
	//是否存储在A核（否存储在B核）
	bool isCoreA = true;
};