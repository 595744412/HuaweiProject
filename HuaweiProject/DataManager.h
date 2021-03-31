#pragma once
#include "definition.h"


class DataManager
{
public:
	unsigned minCores = 100;							//所有虚拟机类型中的最小CPU核数
	unsigned minMemory = 100;							//所有虚拟机类型中的最小内存数
	float minRatioS = 100;								//所有服务器类型中的最小核存比的自然对数
	float maxRatioS = -100;								//所有服务器类型中的最大核存比的自然对数
	int vmSize;											//当前虚拟机存量
	unsigned dayCounts;                                 //总天数
	unsigned tempDay;									//当前读到了第几天的信息
#if isVisual
	FILE* outStream;									//输出流
	FILE* inStream;										//输入流
#endif

	unordered_map<string, ServerType> serverTypeList;   //服务器类型map
	unordered_map<string, VmwareType> vmwareTypeList;	//虚拟机类型map
	vector<RequestType> requestList[1000];				//每日请求列表[天数][请求序号]

	//每日购买服务器列表[天数][型号]
	unordered_map<string, unsigned int> purchaseList[1000];
	//服务器ID重映射
	unordered_map<unsigned int, unsigned int> serverIDList;
	//每日迁移虚拟机列表[天数][迁移序号]
	vector<MoveData> moveList[1000];
	//每日部署虚拟机列表[天数][部署序号]
	vector<AddData> addList[1000];
	//每日服务器新增列表[天数][新增序号]
	vector<Server> newList[1000];
	//每日服务器变化列表[天数][变化序号]
	vector<ChangeData> changeList[1000];
	//虚拟机map
	unordered_map<unsigned int, Vmware> vmwareList;
	//服务器列表
	vector<Server> serverList;
	//服务器性价比有序列表（performanceList)
	vector<string> pfmList;
	//储存服务器的性价比（越小性价比越高）
	unordered_map<string, double> performance;
	//按照核存比升序排列的服务器列表
	vector<string> ratioList;
	//读取初始数据
	void initRead();
	//输出第i天的信息
	void myWrite(unsigned i); 
	//输出可视化所需数据
	void OutputVisual();
	//对服务器的性价比、ratio进行排序
	void init(unsigned int dayCounts);
	//读取一天的请求
	void readRequests();              
};