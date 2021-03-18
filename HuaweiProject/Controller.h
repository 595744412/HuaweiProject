#pragma once
#include "definition.h"


//核心控制类（本质是利用数据生成操作列表）
class Controller
{
private:
	//服务器类型列表（按核存比升序）
	vector<ServerType> ratioServerList;
	//可能有空间存放的服务器列表
	vector<unsigned int> usedServerList;
	//购买一台服务器并添加到列表，返回服务器对象引用
	Server& PurchaseServer(string type);
	//核心可用阈值
	const unsigned int leastCore = 10;
	//内存可用阈值
	const unsigned int leastMemory = 10;
	//虚拟机型号对应购买服务器型号的映射
	unordered_map<string, string> vmwareToServer;
	void init(void);
public:
	//生成输出所需操作列表
	void CreateList();
};

