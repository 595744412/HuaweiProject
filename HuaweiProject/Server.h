#pragma once
#include "definition.h"

//服务器
class Server
{
	//服务器ID
	unsigned int id;
	//该服务器类型
	ServerType& myType;
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

public:

};

