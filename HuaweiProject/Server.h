#pragma once
#include "definition.h"
extern DataManager dataManager;

//服务器
class Server
{
	//服务器ID
	unsigned int id;
	//该服务器类型
	ServerType& myType;
	//A核数据
	CoreData coreA;
	//B核数据
	CoreData coreB;
	//往A/B核存放核和内存
	void AddIntoCore(unsigned int cores, unsigned int memory, CoreData& core);
	//从A/B核删除核和内存
	void DeleteIntoCore(unsigned int cores, unsigned int memory, CoreData& core);

public:
	//添加一台虚拟机，返回是否成功
	bool AddVmware(unsigned int id, bool addToA);
	//添加一台虚拟机，返回是否成功
	bool DeleteVmware(unsigned int id);
};

