#pragma once
#include "definition.h"



//服务器
class Server
{
	//服务器ID
	unsigned int id;
	//服务器数量
	static unsigned int count;
	//该服务器类型
	ServerType myType;
	//A节点数据
	NodeData nodeA;
	//B节点数据
	NodeData nodeB;
	//往A/B节点存放核和内存
	void AddIntoNode(unsigned int cores, unsigned int memory, NodeData& node);
	//从A/B节点删除核和内存
	void DeleteIntoNode(unsigned int cores, unsigned int memory, NodeData& node);
public:
	//构造函数
	Server(ServerType serverType);
	//添加一台虚拟机，返回是否成功
	bool AddVmware(unsigned int id, bool addToA);
	//删除一台虚拟机，返回是否成功
	bool DeleteVmware(unsigned int id);
	//获取ID
	unsigned int GetID() { return id; }
	//获取A核信息
	NodeData GetA() { return nodeA; }
	//获取B核信息
	NodeData GetB() { return nodeB; }
	//获取服务器类型
	ServerType GetServerType() { return myType; }

};



