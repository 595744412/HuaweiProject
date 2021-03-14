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
	unordered_map<unsigned int, Vmware&> vmwares;
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
	//是否存储在A核（否存储在B核）
	bool isNodeA = true;
};