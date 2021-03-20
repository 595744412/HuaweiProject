#include "Controller.h"
#include "DataManager.h"
#include "Server.h"

Server& Controller::PurchaseServer(string type)
{
	Server server(dataManager.serverTypeList[type]);
	dataManager.serverList.emplace_back(server);
	usedServerList.emplace_back(server.GetID());
	return dataManager.serverList[server.GetID()];
}

//初始化
void Controller::init(void)
{ 
	dataManager.init(dataManager.dayCounts);
	float thre = 0.15;
	for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
		string serName;
		bool flag = false; //是否按照要求找到了一个服务器
		//if (i->second.cores > i->second.memory) {
		//	float target = float(i->second.cores) / float(i->second.memory);
		//	target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
		//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
		//		float temp = float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory);
		//		if (temp < 0.8)
		//			continue;
		//		//根据CPU和memory之比选择服务器
		//		if ((target <= temp + thre) && (target >= temp - thre)
		//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
		//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
		//			serName = *iter;
		//			flag = true;
		//			break;
		//		}
		//	}
		//}
		//else {
		//	float target = float(i->second.memory) / float(i->second.cores);
		//	target = target > (1 / dataManager.maxRatioS) ? (1 / dataManager.maxRatioS) : target;
		//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
		//		float temp = float(dataManager.serverTypeList[*iter].memory) / float(dataManager.serverTypeList[*iter].cores);
		//		if (temp < 0.8)
		//			continue;
		//		//根据memory和CPU之比选择服务器
		//		if ((target <= temp + thre) && (target >= temp - thre)
		//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
		//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
		//			serName = *iter;
		//			flag = true;
		//			break;
		//		}
		//	}
		//}
		float target = logf(float(i->second.cores) / float(i->second.memory));
		target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
		target = target < dataManager.minRatioS ? dataManager.minRatioS : target;
		for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
			float temp = logf(float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory));
			//根据CPU和memory之比选择服务器
			if ((target <= temp + thre) && (target >= temp - thre)
				&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
				&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
				serName = *iter;
				flag = true;
				break;
			}
		}
		if (!flag) {
			for (auto iter = dataManager.serverTypeList.begin(); iter != dataManager.serverTypeList.end(); iter++) {
				//根据memory和CPU之比选择服务器
				if ((iter->second.cores >= i->second.cores) && (iter->second.memory >= i->second.memory)) {
					serName = iter->first;
					break;
				}
			}
		}
		vmwareToServer[i->first] = serName;
	}
}

//获取ChangeData
void getAddData(Server& server, ChangeData& data)
{
	data.coresA = server.GetServerType().cores - server.GetA().unusedCores;
	data.memoryA = server.GetServerType().memory - server.GetA().unusedMemory;
	data.coresB = server.GetServerType().cores - server.GetB().unusedCores;
	data.memoryB = server.GetServerType().memory - server.GetB().unusedMemory;
	data.serverID = server.GetID();
}

/*迁移操作
* vmwareID:要迁移的服务器ID
* goal：迁移后的目标服务器
* days：当前是第几天
*/
void Controller::moveFun(const unsigned vmwareID, pair<int,int>& goal, int days)
{
	unsigned tempServerID = dataManager.vmwareList[vmwareID].serverID;
	MoveData move_data = { vmwareID, goal.first, dataManager.vmwareList[vmwareID].myType.isDouble, goal.second == 0 };;
	dataManager.moveList[days].emplace_back(move_data);
	//添加操作
	Server& server = dataManager.serverList[usedServerList[goal.first]];
	if (goal.second == 0)
		server.AddVmwareA(vmwareID);
	else if (goal.second == 1)
		server.AddVmwareB(vmwareID);
	else
		server.AddVmwareD(vmwareID);
	//添加服务器变化到列表
#if isVisual
	ChangeData temp1;
	getAddData(server, temp1);
	dataManager.changeList[days].emplace_back(temp1);
#endif
	if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
		&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
		usedServerList.erase(usedServerList.begin() + goal.first);
	}
	//删除操作
	server = dataManager.serverList[tempServerID];
	server.DeleteVmware(vmwareID);
	//添加服务器变化到列表
#if isVisual
	getAddData(server, temp1);
	dataManager.changeList[days].emplace_back(temp1);
#endif
	auto iter = find(usedServerList.begin(), usedServerList.end(), server.GetID());
	if (iter == usedServerList.cend())
		usedServerList.emplace_back(server.GetID());
}

/*
* 遍历所有可用服务器的列表，尝试找出一个最适合当前虚拟机的服务器
* goal:找到的目标服务器放在这
* jumpSeq：要跳过的服务器ID
*/
void Controller::process(VmwareType& type, pair<int, int>& goal, bool& move, int jumpSeq = -1)
{
	float delta = 1000;
	for (unsigned int k = 0; k < usedServerList.size(); k++) {
		if (jumpSeq == int(k)) {
			break;
		}
		//第k台可用服务器
		Server& server = dataManager.serverList[usedServerList[k]];
		float target = logf(float(type.cores) / float(type.memory));
		int surCoresA = server.GetA().unusedCores - type.cores;
		int surMemoryA = server.GetA().unusedMemory - type.memory;
		int surCoresB = server.GetB().unusedCores - type.cores;
		int surMemoryB = server.GetB().unusedMemory - type.memory;
		if (type.isDouble) {
			if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				float mDelta = fabs(ratioA - target) > fabs(ratioB - target) ? fabs(ratioA - target) : fabs(ratioB - target);
				if (mDelta < delta) {
					delta = mDelta;
					goal = pair<int, int>(k, 2);
					surCoresA = surCoresA ? surCoresA : 1;
					surMemoryA = surMemoryA ? surMemoryA : 1;
					surCoresB = surCoresB ? surCoresB : 1;
					surMemoryB = surMemoryB ? surMemoryB : 1;
					if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20) ||
						fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
						move = true;
				}
			}
		}
		else {
			if ((surCoresA >= 0) && (surMemoryA >= 0)) {
				float ratioA = float(surCoresA) / float(surMemoryA);
				if (fabs(ratioA - target) < delta) {
					delta = fabs(ratioA - target);
					goal = pair<int, int>(k, 0);
					surCoresA = surCoresA ? surCoresA : 1;
					surMemoryA = surMemoryA ? surMemoryA : 1;
					if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20))
						move = true;
				}
			}
			if ((surCoresB >= 0) && (surMemoryB >= 0)) {
				float ratioB = float(surCoresB) / float(surMemoryB);
				if (fabs(ratioB - target) < delta) {
					delta = fabs(ratioB - target);
					goal = pair<int, int>(k, 1);
					surCoresB = surCoresB ? surCoresB : 1;
					surMemoryB = surMemoryB ? surMemoryB : 1;
					if (fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
						move = true;
				}
			}
		}
	}
}

void Controller::CreateList()
{
	init();
	//try1.0版本
	vector<unsigned> waitMove;  //储存需要迁移的虚拟机的列表
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//第i天
		if (i % 50 == 0)
			cout << i << endl;
		unsigned int purchaseCount = 0;
		//进行迁移
		//for (int j = 0, moveNum = 1; j < waitMove.size(); j++) {
		//	if (moveNum > int(dataManager.vmSize * 0.005))
		//		break;
		//	int tempServerID = dataManager.vmwareList[waitMove[j]].serverID;
		//	VmwareType& type = dataManager.vmwareList[waitMove[j]].myType;
		//	pair<int, int> goal = pair<int, int>(-1, -1);
		//	bool move = false;
		//	//遍历所有可用服务器的列表，尝试找出一个最好的
		//	process(type, goal, move, tempServerID);
		//	//迁移
		//	if (!move && goal != pair<int, int>(-1, -1)) {
		//		moveNum++;
		//		moveFun(waitMove[j], goal, i);
		//	}
		//}
		//处理每个请求
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//第j个请求
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				dataManager.vmSize++;
				bool success = false;
				VmwareType type = dataManager.vmwareList[request.ID].myType;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false; //是否迁移
				//遍历所有可用服务器的列表，尝试找出一个最好的
				process(type, goal, move);
				//如果找到了一个可行的
				if (goal != pair<int, int>(-1, -1)) {
					if (move)
						waitMove.emplace_back(request.ID);
					Server& server = dataManager.serverList[usedServerList[goal.first]];
					if (goal.second == 0)
						server.AddVmwareA(request.ID);
					else if (goal.second == 1)
						server.AddVmwareB(request.ID);
					else
						server.AddVmwareD(request.ID);
					//添加服务器变化到列表
#if isVisual
					ChangeData temp1;
					getAddData(server, temp1);
					dataManager.changeList[i].emplace_back(temp1);
#endif
					if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
						&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
						usedServerList.erase(usedServerList.begin() + goal.first);
					}
					success = true;
				}
				if (!success) {
					string serName = vmwareToServer[type.name];
					purchaseCount++;
					dataManager.purchaseList[i][serName] += 1;
					Server& server = PurchaseServer(serName);
					if (type.isDouble)
						server.AddVmwareD(request.ID);
					else
						server.AddVmwareA(request.ID);
					usedServerList.emplace_back(server.GetID());
					//添加新增服务器到列表
#if isVisual
					dataManager.newList[i].emplace_back(server);
#endif
				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				//cout << dataManager.vmwareList[request.ID].serverID << endl;
				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				dataManager.addList[i].emplace_back(it);
			}
			//删除操作
			else {
				dataManager.vmSize--;
				Server& server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				server.DeleteVmware(request.ID);
				//添加服务器变化到列表
#if isVisual
				ChangeData temp1;
				getAddData(server, temp1);
				dataManager.changeList[i].emplace_back(temp1);
#endif
				usedServerList.emplace_back(server.GetID());
			}
		}
		//建立顺序服务器序号重映射
		unsigned int newID = Server::GetCount() - purchaseCount;
		unordered_map<string, unsigned int> purchaseIDList;
		for (auto j = dataManager.purchaseList[i].cbegin(); j != dataManager.purchaseList[i].cend(); j++) {
			purchaseIDList[j->first] = newID;
			newID += j->second;
		}
		for (unsigned int j = Server::GetCount() - purchaseCount; j < Server::GetCount(); j++) {
			string name = dataManager.serverList[j].GetServerType().name;
			dataManager.serverIDList[j] = purchaseIDList[name];
			purchaseIDList[name]++;
		}
	}
}