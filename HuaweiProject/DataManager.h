#pragma once
#include "definition.h"


class DataManager
{
public:
	//服务器类型map
	unordered_map<string, ServerType> serverTypeList;
	//虚拟机类型map
	unordered_map<string, VmwareType> vmwareTypeList;
	//每日请求列表[天数][请求序号]
	vector<RequestType> requestList[1000];
	//天数
	unsigned int dayCounts;
	//每日购买服务器列表[天数][型号]
	unordered_map<string, unsigned int> purchaseList[1000];
	//服务器ID重映射
	unordered_map<unsigned int, unsigned int> serverIDList;
	//每日迁移虚拟机列表[天数][迁移序号]
	vector<MoveData> moveList[1000];
	//每日部署虚拟机列表[天数][部署序号]
	vector<AddData> addList[1000];
	//虚拟机map
	unordered_map<unsigned int, Vmware> vmwareList;
	//服务器列表
	vector<Server> serverList;
	//读取所有数据
	void ReadAll();
	//输出操作
	void OutputAll();
	//输出可视化所需数据
	void OutputVisual();
};