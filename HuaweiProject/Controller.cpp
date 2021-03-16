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

void Controller::CreateList()
{
	//第一版
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//第i天
		unsigned int purchaseCount = 0;
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//第j个请求
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				bool success = false;
				for (unsigned int k = 0; k < usedServerList.size(); k++) {
					//第k台可用服务器
					Server& server = dataManager.serverList[usedServerList[k]];
					if(server.AddVmware(request.ID, true) || server.AddVmware(request.ID, false)) {
						success = true;
						break;
					}
					usedServerList.erase(usedServerList.begin() + k);
				}
				if (!success) {
					unordered_map<string, ServerType>::iterator iter;
					for (iter = dataManager.serverTypeList.begin(); iter != dataManager.serverTypeList.end(); iter++) {
						VmwareType vmware = dataManager.vmwareList[request.ID].myType;
						if (iter->second.cores >= vmware.cores&&iter->second.memory >= vmware.memory) {
							purchaseCount++;
							dataManager.purchaseList[i][iter->first] += 1;
							Server& server = PurchaseServer(iter->first);
							server.AddVmware(request.ID, true);
							break;
						}
					}
				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
				dataManager.addList[i].emplace_back(it);
			}
			else {
				Vmware& vmware = dataManager.vmwareList[request.ID];
				Server& server = dataManager.serverList[vmware.serverID];
				server.DeleteVmware(request.ID);
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
