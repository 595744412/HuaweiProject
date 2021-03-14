#pragma once
#include "definition.h"
class DataManager
{
	//服务器类型map
	unordered_map<string, SeverType> severTypeList;
	//虚拟机类型map
	unordered_map<string, VmwareType> vmwareTypeList;
	//每日请求列表[天数][请求序号]
	vector<RequestType> requestList[1000];

public:
	//读取所有数据
	void ReadAll();
};

