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
	for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
		float ratio = i->second.ratio;
		int mid = 0, inter = 9;
		string target = " ";
		while (mid < dataManager.ratioList.size()) {
			if (dataManager.serverTypeList[dataManager.ratioList[mid]].cores >= i->second.cores &&
				dataManager.serverTypeList[dataManager.ratioList[mid]].memory >= i->second.memory)
				target = dataManager.ratioList[mid];
			if (ratio <= dataManager.serverTypeList[dataManager.ratioList[mid]].ratio &&
				target != " ") {
				break;
			}
			mid++;
		}
		if (mid + inter > dataManager.ratioList.size())
			mid = dataManager.ratioList.size() - inter;
		if (mid - inter < 0)
			mid = inter;
		for (int j = mid - inter; j < mid + inter; j++) {
			if (dataManager.performance[dataManager.ratioList[j]] < dataManager.performance[target] &&
				dataManager.serverTypeList[dataManager.ratioList[j]].cores >= i->second.cores &&
				dataManager.serverTypeList[dataManager.ratioList[j]].memory >= i->second.memory) {
				target = dataManager.ratioList[j];
			}
		}
		vmwareToServer[i->first] = target;
	}
	//float thre = 0.15;
	//for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
	//	string serName;
	//	bool flag = false; //是否按照要求找到了一个服务器
	//	//if (i->second.cores > i->second.memory) {
	//	//	float target = float(i->second.cores) / float(i->second.memory);
	//	//	target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
	//	//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
	//	//		float temp = float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory);
	//	//		if (temp < 0.8)
	//	//			continue;
	//	//		//根据CPU和memory之比选择服务器
	//	//		if ((target <= temp + thre) && (target >= temp - thre)
	//	//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
	//	//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
	//	//			serName = *iter;
	//	//			flag = true;
	//	//			break;
	//	//		}
	//	//	}
	//	//}
	//	//else {
	//	//	float target = float(i->second.memory) / float(i->second.cores);
	//	//	target = target > (1 / dataManager.maxRatioS) ? (1 / dataManager.maxRatioS) : target;
	//	//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
	//	//		float temp = float(dataManager.serverTypeList[*iter].memory) / float(dataManager.serverTypeList[*iter].cores);
	//	//		if (temp < 0.8)
	//	//			continue;
	//	//		//根据memory和CPU之比选择服务器
	//	//		if ((target <= temp + thre) && (target >= temp - thre)
	//	//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
	//	//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
	//	//			serName = *iter;
	//	//			flag = true;
	//	//			break;
	//	//		}
	//	//	}
	//	//}
	//	float target = logf(float(i->second.cores) / float(i->second.memory));
	//	target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
	//	target = target < dataManager.minRatioS ? dataManager.minRatioS : target;
	//	for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
	//		float temp = logf(float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory));
	//		//根据CPU和memory之比选择服务器
	//		if ((target <= temp + thre) && (target >= temp - thre)
	//			&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
	//			&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
	//			serName = *iter;
	//			flag = true;
	//			break;
	//		}
	//	}
	//	if (!flag) {
	//		for (auto iter = dataManager.serverTypeList.begin(); iter != dataManager.serverTypeList.end(); iter++) {
	//			//根据memory和CPU之比选择服务器
	//			if ((iter->second.cores >= i->second.cores) && (iter->second.memory >= i->second.memory)) {
	//				serName = iter->first;
	//				break;
	//			}
	//		}
	//	}
	//	vmwareToServer[i->first] = serName;
	//}
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
* vmwareID:要迁移的虚拟机ID
* goal：迁移后的目标服务器，goal.first是服务器ID，goal.second：0表示迁移到A节点，1表示迁移到B节点,2表示双节点
* days：当前是第几天
*/
void Controller::moveFun(const unsigned vmwareID, pair<int, int>& goal, int days)
{
	unsigned tempServerID = dataManager.vmwareList[vmwareID].serverID;
	if (vmwareID == 56663036)
		int s = 0;
	MoveData move_data = { vmwareID, goal.first, dataManager.vmwareList[vmwareID].myType.isDouble, goal.second == 0 };;
	dataManager.moveList[days].emplace_back(move_data);

	//删除操作
	Server& server2 = dataManager.serverList[tempServerID];
	server2.DeleteVmware(vmwareID);
	//添加服务器变化到列表
#if isVisual
	ChangeData temp = { server2.GetID(),server2.GetA().usedCores,server2.GetA().usedMemory,server2.GetB().usedCores,server2.GetB().usedMemory };
	dataManager.changeList[days].emplace_back(temp);
#endif
	if (find(usedServerList.begin(), usedServerList.end(), server2.GetID()) == usedServerList.end())
		usedServerList.emplace_back(server2.GetID());

	//添加操作
	Server& server = dataManager.serverList[goal.first];
	if (goal.second == 0)
		server.AddVmwareA(vmwareID);
	else if (goal.second == 1)
		server.AddVmwareB(vmwareID);
	else
		server.AddVmwareD(vmwareID);
	//添加服务器变化到列表
#if isVisual
	ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
	dataManager.changeList[days].emplace_back(temp1);
#endif
	if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
		&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
		usedServerList.erase(find(usedServerList.begin(), usedServerList.end(), server.GetID()));
	}
}

/*
* 遍历所有可用服务器的列表，尝试找出一个最适合当前虚拟机的服务器
* goal:找到的目标服务器放在这
* jumpSeq：要跳过的服务器ID，默认-1
*/
void Controller::process(unsigned vmwareID, pair<int, int>& goal, bool& move, int jumpSeq = -1)
{
	float delta = 1000;
	int emptySever = -1;
	VmwareType& type = dataManager.vmwareList[vmwareID].myType;
	for (auto iter = usedServerList.cbegin(); iter != usedServerList.cend(); iter++) {
		if (*iter == jumpSeq)
			continue;
		//第k台可用服务器
		Server& server = dataManager.serverList[*iter];
		float target = type.ratio;
		int surCoresA = server.GetA().unusedCores - type.cores;
		int surMemoryA = server.GetA().unusedMemory - type.memory;
		int surCoresB = server.GetB().unusedCores - type.cores;
		int surMemoryB = server.GetB().unusedMemory - type.memory;
		if (type.isDouble) {
			if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
				if (server.GetA().usedCores == 0 && server.GetB().usedCores == 0 && server.GetA().usedMemory == 0 && server.GetB().usedMemory == 0) {
					emptySever = *iter;
					continue;
				}
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				float mDelta = fabs(ratioA - target) > fabs(ratioB - target) ? fabs(ratioA - target) : fabs(ratioB - target);
				if (mDelta < delta) {
					delta = mDelta;
					goal = pair<int, int>(*iter, 2);
					// surCoresA = surCoresA ? surCoresA : 1;
					// surMemoryA = surMemoryA ? surMemoryA : 1;
					// surCoresB = surCoresB ? surCoresB : 1;
					// surMemoryB = surMemoryB ? surMemoryB : 1;
					// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20) ||
					// 	fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
					// 	move = true;
					// else
					// 	move = false;
				}
			}
		}
		else {
			if ((surCoresA >= 0) && (surMemoryA >= 0)) {
				if (server.GetA().usedCores == 0 && server.GetB().usedCores == 0 && server.GetA().usedMemory == 0 && server.GetB().usedMemory == 0) {
					emptySever = *iter;
					continue;
				}
				float ratioA = logf(float(server.GetA().unusedCores) / float(server.GetA().unusedMemory));
				if (fabs(ratioA - target) < delta) {
					delta = fabs(ratioA - target);
					goal = pair<int, int>(*iter, 0);
					// surCoresA = surCoresA ? surCoresA : 1;
					// surMemoryA = surMemoryA ? surMemoryA : 1;
					// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20))
					// 	move = true;
					// else
					// 	move = false;
				}
			}
			if ((surCoresB >= 0) && (surMemoryB >= 0)) {
				if (server.GetA().usedCores == 0 && server.GetB().usedCores == 0 && server.GetA().usedMemory == 0 && server.GetB().usedMemory == 0) {
					emptySever = *iter;
					continue;
				}
				float ratioB = logf(float(server.GetB().unusedCores) / float(server.GetB().unusedMemory));
				if (fabs(ratioB - target) < delta) {
					delta = fabs(ratioB - target);
					goal = pair<int, int>(*iter, 1);
					// surCoresB = surCoresB ? surCoresB : 1;
					// surMemoryB = surMemoryB ? surMemoryB : 1;
					// if (fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
					// 	move = true;
					// else
					// 	move = false;
				}
			}
		}

		// if (type.isDouble) {
		// 	if ((surCoresA >= 0) && (surCoresB >= 0) && (surMemoryA >= 0) && (surMemoryB >= 0)) {
		// 		surCoresA = surCoresA ? surCoresA : 1;
		// 		surMemoryA = surMemoryA ? surMemoryA : 1;
		// 		surCoresB = surCoresB ? surCoresB : 1;
		// 		surMemoryB = surMemoryB ? surMemoryB : 1;
		// 		float ratioA = logf(float(surCoresA * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryA));
		// 		float ratioB = logf(float(surCoresB * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryB));
		// 		float mDelta = fabs(ratioA) > fabs(ratioB) ? fabs(ratioA) : fabs(ratioB);
		// 		if (mDelta < delta) {
		// 			delta = mDelta;
		// 			goal = pair<int, int>(*iter, 2);
		// 			// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20) ||
		// 			// 	fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
		// 			// 	move = true;
		// 			// else
		// 			// 	move = false;
		// 		}
		// 	}
		// }
		// else {
		// 	if ((surCoresA >= 0) && (surMemoryA >= 0)) {
		// 		surCoresA = surCoresA ? surCoresA : 1;
		// 		surMemoryA = surMemoryA ? surMemoryA : 1;
		// 		float ratioA = logf(float(surCoresA * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryA));
		// 		if (fabs(ratioA) < delta) {
		// 			delta = fabs(ratioA);
		// 			goal = pair<int, int>(*iter, 0);
		// 			// if (fabs(logf(float(surCoresA) / float(surMemoryA))) > logf(20))
		// 			// 	move = true;
		// 			// else
		// 			// 	move = false;
		// 		}
		// 	}
		// 	if ((surCoresB >= 0) && (surMemoryB >= 0)) {
		// 		surCoresB = surCoresB ? surCoresB : 1;
		// 		surMemoryB = surMemoryB ? surMemoryB : 1;
		// 		float ratioB = logf(float(surCoresB * server.GetServerType().memory) / float(server.GetServerType().cores * surMemoryB));
		// 		if (fabs(ratioB) < delta) {
		// 			delta = fabs(ratioB);
		// 			goal = pair<int, int>(*iter, 1);
		// 			// if (fabs(logf(float(surCoresB) / float(surMemoryB))) > logf(20))
		// 			// 	move = true;
		// 			// else
		// 			// 	move = false;
		// 		}
		// 	}
		// }
	// }
	}

	if (emptySever != -1 && goal == pair<int, int>(-1, -1)) {
		goal = pair<int, int>(emptySever, (type.isDouble == 1) + 1);
	}
}

void Controller::CreateList()
{
	init();
	//try1.0版本
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//第i天
		unsigned int purchaseCount = 0;
		//进行迁移
		int moveNum = 1;
		int sd = 0;
		for (auto iter = waitMoveS.begin(); iter != waitMoveS.end();) {
			bool success = true;
			int asd = dataManager.serverList[*iter].GetA().vmwares.size();
			int as = 0;
			for (int jk = 0; jk < dataManager.serverList[*iter].GetA().vmwares.size(); jk++) {
				as++;
				if (as == 4 && i == 91)
					int dasjk = 0;
				if (moveNum > int(dataManager.vmSize * 0.005))
					break;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false;
				//遍历所有可用服务器的列表，尝试找出一个最好的
				process(dataManager.serverList[*iter].GetA().vmwares[jk], goal, move, *iter);
				//if (goal == pair<int, int>(-1, -1) || move == true) {
				if (goal == pair<int, int>(-1, -1)) {
					success = false;
					break;
				}
				else {
					moveNum++;
					moveFun(dataManager.serverList[*iter].GetA().vmwares[jk--], goal, i);
				}
			}
			if (!success) {
				iter++;
				continue;
			}
			for (int jk = 0; jk < dataManager.serverList[*iter].GetB().vmwares.size(); jk++) {
				if (moveNum > int(dataManager.vmSize * 0.005))
					break;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false;
				//遍历所有可用服务器的列表，尝试找出一个最好的
				process(dataManager.serverList[*iter].GetB().vmwares[jk], goal, move, *iter);
				//if (goal == pair<int, int>(-1, -1) || move == true) {
				if (goal == pair<int, int>(-1, -1)) {
					success = false;
					break;
				}
				else {
					moveNum++;
					moveFun(dataManager.serverList[*iter].GetB().vmwares[jk--], goal, i);
				}
			}
			if (success) {
				iter = waitMoveS.erase(iter);
			}
			else {
				iter++;
			}
		}
		//处理每个请求
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//第j个请求
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				dataManager.vmSize++;
				VmwareType type = dataManager.vmwareList[request.ID].myType;
				pair<int, int> goal = pair<int, int>(-1, -1);
				bool move = false; //是否迁移
				//遍历所有可用服务器的列表，尝试找出一个最好的
				process(request.ID, goal, move);
				//如果找到了一个可行的
				if (goal != pair<int, int>(-1, -1)) {
					Server& server = dataManager.serverList[goal.first];
					if (goal.second == 0)
						server.AddVmwareA(request.ID);
					else if (goal.second == 1)
						server.AddVmwareB(request.ID);
					else
						server.AddVmwareD(request.ID);
					if ((server.GetA().usedCores > thre || server.GetA().usedMemory > thre ||
						server.GetB().usedCores > thre || server.GetB().usedMemory > thre) && !move) {
						//waitMoveV.erase(request.ID);
						auto it1 = find(waitMoveS.begin(), waitMoveS.end(), goal.first);
						if (it1 != waitMoveS.end())
							waitMoveS.erase(it1);
					}
					else {
						//waitMoveV.insert(request.ID);
						if (find(waitMoveS.begin(), waitMoveS.end(), goal.first) == waitMoveS.end())
							waitMoveS.emplace_back(goal.first);
					}
					//添加服务器变化到列表
#if isVisual
					ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
					dataManager.changeList[i].emplace_back(temp1);
#endif
					if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
						&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory)) {
						usedServerList.erase(find(usedServerList.begin(), usedServerList.end(), goal.first));
					}
				}
				//如果没有找到可行的就添加新的服务器
				else {
					string serName = vmwareToServer[type.name];
					purchaseCount++;
					dataManager.purchaseList[i][serName] += 1;
					Server& server = PurchaseServer(serName);
					if (type.isDouble)
						server.AddVmwareD(request.ID);
					else
						server.AddVmwareA(request.ID);
					//添加新增服务器到列表
#if isVisual
					dataManager.newList[i].emplace_back(server);
#endif
				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				dataManager.addList[i].emplace_back(it);
			}
			//删除操作
			else {
				dataManager.vmSize--;
				int ID = dataManager.vmwareList[request.ID].serverID;
				Server& server = dataManager.serverList[ID];
				if (server.GetID() != ID)
					int dsa = 1;
				server.DeleteVmware(request.ID);
				//添加服务器变化到列表
#if isVisual
				ChangeData temp1 = { server.GetID(),server.GetA().usedCores,server.GetA().usedMemory,server.GetB().usedCores,server.GetB().usedMemory };
				dataManager.changeList[i].emplace_back(temp1);
#endif
				if (find(usedServerList.begin(), usedServerList.end(), server.GetID()) == usedServerList.end())
					usedServerList.emplace_back(server.GetID());
				//满足服务器的清空条件时加入待迁移列表
				if ((server.GetA().usedCores <= thre && server.GetA().usedMemory <= thre &&
					server.GetB().usedCores <= thre && server.GetB().usedMemory <= thre)) {
					if (find(waitMoveS.begin(), waitMoveS.end(), server.GetID()) == waitMoveS.end())
						waitMoveS.emplace_back(server.GetID());
					/*for (auto jk = server.GetA().vmwares.cbegin(); jk != server.GetA().vmwares.cend(); jk++) {
						waitMoveV.insert(*jk);
					}
					for (auto jk = server.GetB().vmwares.cbegin(); jk != server.GetB().vmwares.cend(); jk++) {
						waitMoveV.insert(*jk);
					}*/
				}
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