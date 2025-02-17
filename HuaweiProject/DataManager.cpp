#include "DataManager.h"
#include "Server.h"

/*
   按顺序读取所有数据存在serverTypeList、vmwareTypeList和requestList中，
   对于ADD操作，添加对应（id，虚拟机类型引用）键值对在vmwareList中
*/
void DataManager::initRead()
{
#if isVisual
	freopen_s(&inStream, "./training-1.txt", "r", stdin);
#endif
	int num;
	string serverName, vmwareName, requestName;
	unsigned int cores, memory, price, costPerDay, isDouble;
	char buff;
	//读入服务器信息
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> serverName >> cores >> buff >> memory >> buff >> price >> buff >> costPerDay >> buff;
		serverName = serverName.substr(1, serverName.size() - 2);
		serverTypeList[serverName] = ServerType(serverName, cores / 2, memory / 2, price, costPerDay);
		float temp = logf(float(cores) / float(memory));
		minRatioS = temp > minRatioS ? minRatioS : temp;
		maxRatioS = temp > maxRatioS ? temp : maxRatioS;
		serverTypeList[serverName].ratio = temp;
	}
	//读入虚拟机信息
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> vmwareName >> cores >> buff >> memory >> buff >> isDouble >> buff;
		vmwareName = vmwareName.substr(1, vmwareName.size() - 2);
		if (isDouble) {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores / 2, memory / 2, isDouble == 1);
			minCores = (minCores > cores / 2) ? (cores / 2) : minCores;
			minMemory = (minMemory > memory / 2) ? (memory / 2) : minMemory;
		}
		else {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores, memory, isDouble == 1);
			minCores = (minCores > cores) ? cores : minCores;
			minMemory = (minMemory > memory) ? memory : minMemory;
		}
		vmwareTypeList[vmwareName].ratio = logf(float(cores) / float(memory));
	}
	//读入初始K天的请求
	unsigned T, K;
	cin >> T;
	dayCounts = T;
	cin >> K;
	tempDay = 0;
	while (tempDay < K) {
		cin >> num;
		for (int j = 0; j < num; j++) {
			cin >> buff >> requestName;
			requestName = requestName.substr(0, requestName.size() - 1);
			unsigned int ID;
			if (requestName == "add") {
				cin >> vmwareName >> ID >> buff;
				vmwareName = vmwareName.substr(0, vmwareName.size() - 1);
				requestList[tempDay].emplace_back(true, vmwareName, ID);
				vmwareList[ID].id = ID;
				vmwareList[ID].myType = vmwareTypeList[vmwareName];
			}
			else {
				cin >> ID >> buff;
				requestList[tempDay].emplace_back(false, "None", ID);
			}
		}
		tempDay++;
	}
}
//读入每一天的请求
void DataManager::readRequests()
{
	if (tempDay == dayCounts) {
#if isVisual
		if(inStream)
			fclose(inStream);
#endif
		return;
	}
	int num;
	string vmwareName, requestName;
	char buff;
	cin >> num;
	for (int j = 0; j < num; j++) {
		cin >> buff >> requestName;
		requestName = requestName.substr(0, requestName.size() - 1);
		unsigned int ID;
		if (requestName == "add") {
			cin >> vmwareName >> ID >> buff;
			vmwareName = vmwareName.substr(0, vmwareName.size() - 1);
			requestList[tempDay].emplace_back(true, vmwareName, ID);
			vmwareList[ID].id = ID;
			vmwareList[ID].myType = vmwareTypeList[vmwareName];
		}
		else {
			cin >> ID >> buff;
			requestList[tempDay].emplace_back(false, "None", ID);
		}
	}
	tempDay++;
	return;
}


/*
	把purchaseList、moveList和addList按每天的顺序输出
*/
void DataManager::myWrite(unsigned i)
{
#if isVisual
	if (i == 0) {
		freopen_s(&outStream, "./result.txt", "w", stdout);
	}
	else {
		freopen_s(&outStream, "./result.txt", "a+", stdout);
	}
#endif
	//输出购买服务器
	cout << "(purchase, " << purchaseList[i].size() << ")" << endl;
	for (auto j = purchaseList[i].cbegin(); j != purchaseList[i].cend(); j++) {
		cout << "(" + j->first + ", " << j->second << ")" << endl;
	}
	//输出迁移虚拟机
	cout << "(migration, " << moveList[i].size() << ")" << endl;
	for (auto j = moveList[i].cbegin(); j != moveList[i].cend(); j++) {
		if (j->isDouble) {
			cout << "(" << j->vmwareID << ", " << serverIDList[j->serverID] << ")" << endl;
		}
		else {
			if (j->isNodeA)
				cout << "(" << j->vmwareID << ", " << serverIDList[j->serverID] << ", A)" << endl;
			else
				cout << "(" << j->vmwareID << ", " << serverIDList[j->serverID] << ", B)" << endl;
		}
	}
	//输出创建请求
	for (auto j = addList[i].cbegin(); j != addList[i].cend(); j++) {

		if (j->isDouble) {
			cout << "(" << serverIDList[j->serverID] << ")" << endl;
		}
		else {
			if (j->isNodeA)
				cout << "(" << serverIDList[j->serverID] << ", A)" << endl;
			else
				cout << "(" << serverIDList[j->serverID] << ", B)" << endl;
		}
	}
#if isVisual
	freopen_s(&outStream, "CON", "w", stdout);
	if (i == dayCounts && outStream != NULL)
		fclose(outStream);
#endif	
}
/*
  利用data信息，输出每一天新增服务器列表(服务器ID,服务器型号,单节点核心数,单节点内存数,总成本,每天成本)，
  每一天服务器容量变化(服务器ID,A节点当前核心,A节点当前内存,B节点当前核心,B节点当前内存)，
  可以以每一步moveList和requestList为单位输出变化信息（每当有一个服务器发生迁移、删除、添加等操作时需要有一条变化信息）
*/
void DataManager::OutputVisual()
{
#if isVisual
	FILE* stream;
	freopen_s(&stream, "E:/Programming/visualize/output.txt", "w", stdout);
	cout << dayCounts << endl;
	for (unsigned int i = 0; i < dayCounts; i++) {
		//输出新增服务器
		cout << newList[i].size() << endl;
		for (unsigned int j = 0; j < newList[i].size(); j++) {
			ServerType it = newList[i][j].GetServerType();
			unsigned int serverID = newList[i][j].GetID();
			cout << "(" << serverID << "," << it.name << "," << it.cores << "," << it.memory << "," << it.price << "," << it.costPerDay << ")" << endl;
		}
		//输出服务器变化
		cout << changeList[i].size() << endl;
		for (unsigned int j = 0; j < changeList[i].size(); j++) {
			ChangeData it = changeList[i][j];
			cout << "(" << it.serverID << "," << it.coresA << "," << it.memoryA << "," << it.coresB << "," << it.memoryB << ")" << endl;
		}
	}
	fclose(stream);
#endif 
}


/********************************************************
【函数功能】根据读取到的信息进行初始化，获取performance、pfmList和ratioList
【函数参数】dayCounts：总天数
【注】性价比计算公式：(price + costPerDay * daycounts * α) * (1 / cores + 1 / memory) + β * max(cores /memory，memory / cores）
（按照这个公式计算出来的数值越小性价比越高，α和β表示可调参数）
**********************************************************/
void DataManager::init(unsigned int dayCounts)
{
	//虚拟机存量归零
	vmSize = 0;
	//获取性价比
	for (auto i = serverTypeList.cbegin(); i != serverTypeList.cend(); i++) {
		/*	performance[i->first] = double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
				/ double(i->second.cores) + double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
				/ double(i->second.memory);*/
		performance[i->first] = double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
			/ double(i->second.cores) + double(i->second.price + int(i->second.costPerDay) * int(dayCounts) * 0.8)
			/ double(i->second.memory) + 0 * max(i->second.cores / i->second.memory, i->second.memory / i->second.cores);
		pfmList.emplace_back(i->first);
		ratioList.emplace_back(i->first);
	}
	//冒泡排序，得到性价比递减的服务器列表
	for (int i = 0; i < pfmList.size() - 1; i++) {
		for (int j = 0; j < pfmList.size() - 1 - i; j++) {
			if (performance[pfmList[j]] > performance[pfmList[j + 1]]) {
				string temp = pfmList[j];
				pfmList[j] = pfmList[j + 1];
				pfmList[j + 1] = temp;
			}
		}
	}
	//冒泡排序，得到核存比递增的服务器列表
	for (int i = 0; i < ratioList.size() - 1; i++) {
		for (int j = 0; j < ratioList.size() - 1 - i; j++) {
			if (serverTypeList[ratioList[j]].ratio > serverTypeList[ratioList[j + 1]].ratio) {
				string temp = ratioList[j];
				ratioList[j] = ratioList[j + 1];
				ratioList[j + 1] = temp;
			}
		}
	}
}