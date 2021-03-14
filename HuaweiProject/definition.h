#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

//服务器类型
struct SeverType
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

