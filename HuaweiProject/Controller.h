#pragma once
#include "definition.h"

//核心控制类（本质是利用数据生成操作列表）
class Controller
{
private:
	//可能有空间存放的服务器列表
	vector<unsigned int> usedServerList;
	//购买一台服务器并添加到列表，返回服务器对象引用
	Server& PurchaseServer(string type);
	//核心可用阈值
	const unsigned int leastCore = 10;
	//内存可用阈值
	const unsigned int leastMemory = 10;
public:
	//生成输出所需操作列表
	void CreateList();
};

