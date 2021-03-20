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

void Controller::init(void)
{ 
	dataManager.init(dataManager.dayCounts);
	float thre = 1;
	for (auto i = dataManager.vmwareTypeList.cbegin(); i != dataManager.vmwareTypeList.cend(); i++) {
		string serName;
		bool flag = false; //是否按照要求找到了一个服务器
		if (i->second.cores > i->second.memory) {
			float target = float(i->second.cores) / float(i->second.memory);
			target = target > dataManager.maxRatioS ? dataManager.maxRatioS : target;
			for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
				float temp = float(dataManager.serverTypeList[*iter].cores) / float(dataManager.serverTypeList[*iter].memory);
				if (temp < 0.8)
					continue;
				//根据CPU和memory之比选择服务器
				if ((target <= temp + thre) && (target >= temp - thre)
					&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
					&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
					serName = *iter;
					flag = true;
					break;
				}
			}
		}
		else {
			float target = float(i->second.memory) / float(i->second.cores);
			target = target > (1 / dataManager.maxRatioS) ? (1 / dataManager.maxRatioS) : target;
			for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
				float temp = float(dataManager.serverTypeList[*iter].memory) / float(dataManager.serverTypeList[*iter].cores);
				if (temp < 0.8)
					continue;
				//根据memory和CPU之比选择服务器
				if ((target <= temp + thre) && (target >= temp - thre)
					&& (dataManager.serverTypeList[*iter].cores >= i->second.cores)
					&& (dataManager.serverTypeList[*iter].memory >= i->second.memory)) {
					serName = *iter;
					flag = true;
					break;
				}
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

void Controller::CreateList()
{
	init();
	//try1.0版本
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//第i天
		if(i % 50 == 0)
			cout << i << endl;
		unsigned int purchaseCount = 0;
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//第j个请求
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				bool success = false;
				VmwareType type = dataManager.vmwareList[request.ID].myType;
				for (unsigned int k = 0; k < usedServerList.size(); k++) {
					//第k台可用服务器
					Server& server = dataManager.serverList[usedServerList[k]];
					if (type.isDouble) {
						if ((server.GetB().unusedCores >= type.cores) && (server.GetB().unusedMemory >= type.memory)
							&& (server.GetA().unusedCores >= type.cores) && (server.GetA().unusedMemory >= type.memory)) {
							server.AddVmwareD(request.ID);
							success = true;
						}
					}
					else {
						if ((server.GetA().unusedCores >= type.cores) && (server.GetA().unusedMemory >= type.memory)) {
							server.AddVmwareA(request.ID);
							success = true;
						}
						else
							if ((server.GetB().unusedCores >= type.cores) && (server.GetB().unusedMemory >= type.memory)) {
								server.AddVmwareB(request.ID);
								success = true;
							}
					}
					if (success) {
						//添加服务器变化到列表
#if isVisual
						unsigned coresA = server.GetServerType().cores - server.GetA().unusedCores;
						unsigned memoryA = server.GetServerType().memory - server.GetA().unusedMemory;
						unsigned coresB = server.GetServerType().cores - server.GetB().unusedCores;
						unsigned memoryB = server.GetServerType().memory - server.GetB().unusedMemory;
						ChangeData temp1 = { server.GetID(), coresA,memoryA,coresB,memoryB };
						dataManager.changeList[i].emplace_back(temp1);
#endif
						if ((server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory)
							&& (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory))
							usedServerList.erase(usedServerList.begin() + k);
						break;
					}
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
			else {
				Vmware& vmware = dataManager.vmwareList[request.ID];
				Server& server = dataManager.serverList[vmware.serverID];
				server.DeleteVmware(request.ID);
				//添加服务器变化到列表
#if isVisual
				unsigned coresA = server.GetServerType().cores - server.GetA().unusedCores;
				unsigned memoryA = server.GetServerType().memory - server.GetA().unusedMemory;
				unsigned coresB = server.GetServerType().cores - server.GetB().unusedCores;
				unsigned memoryB = server.GetServerType().memory - server.GetB().unusedMemory;
				ChangeData temp1 = { server.GetID(), coresA,memoryA,coresB,memoryB };
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