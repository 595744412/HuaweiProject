#include "DataManager.h"
/*
   按顺序读取所有数据存在serverTypeList、vmwareTypeList和requestList中，
   对于ADD操作，添加对应（id，虚拟机类型引用）键值对在vmwareList中
*/
void DataManager::ReadAll()
{
	/*FILE *stream;
	freopen_s(&stream, "./training-1.txt", "r", stdin);*/
	int num;
	string serverName, vmwareName, requestName;
	unsigned int cores, memory, price, costPerDay, isDouble;
	char buff;
	//读入服务器信息
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> serverName >> cores >> buff >> memory >> buff >> price >> buff >> costPerDay >> buff;
		serverName = serverName.substr(1, serverName.size() - 2);
		serverTypeList[serverName] = ServerType(serverName, cores/2, memory/2, price, costPerDay);
	}
	//读入虚拟机信息
	cin >> num;
	for (int i = 0; i < num; i++) {
		cin >> vmwareName >> cores >> buff >> memory >> buff >> isDouble >> buff;
		vmwareName = vmwareName.substr(1, vmwareName.size() - 2);
		if (isDouble) {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores/2, memory/2, isDouble == 1);
		}
		else {
			vmwareTypeList[vmwareName] = VmwareType(vmwareName, cores, memory, isDouble == 1);
		}
	}
	//读入请求
	int days;
	cin >> days;
	dayCounts = days;
	for (int i = 0; i < days; i++) {
		cin >> num;
		for (int j = 0; j < num; j++) {
			cin >> buff >> requestName;
			requestName = requestName.substr(0, requestName.size() - 1);
			unsigned int ID;
			if (requestName == "add") {
				cin >> vmwareName >> ID >> buff;
				vmwareName = vmwareName.substr(0, vmwareName.size() - 1);
				requestList[i].emplace_back(true, vmwareName, ID);
				vmwareList[ID].id = ID;
				vmwareList[ID].myType = vmwareTypeList[vmwareName];
			}
			else {
				cin >> ID >> buff;
				requestList[i].emplace_back(false, " ", ID);
			}
		}
	}
}

/*
	把purchaseList、moveList和addList按每天的顺序输出
*/
void DataManager::OutputAll()
{
	for (unsigned int i = 0; i < dayCounts; i++) {
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
	}
}

void DataManager::OutputVisual()
{
	//利用data信息，输出每一天新增服务器列表(服务器ID,服务器型号,单节点核心数,单节点内存数,总成本,每天成本)，每一天服务器容量变化(服务器ID,A节点当前核心,A节点当前内存,B节点当前核心,B节点当前内存)，可以以每一步moveList和requestList为单位输出变化信息（每当有一个服务器发生迁移、删除、添加等操作时需要有一条变化信息）
}
