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
	//虚拟机map
	unordered_map<unsigned int, Vmware> vmwareList;
	//服务器列表
	vector<Server> serverList;
	//读取所有数据
	void ReadAll(const string& filePath);
};

