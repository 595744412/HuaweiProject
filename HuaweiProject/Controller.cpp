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
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//第j个请求
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				bool success = false;
				for (unsigned int k = 0; k < usedServerList.size(); k++) {
					//第k台可用虚拟机
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
						dataManager.purchaseList[i][iter->first] += 1;
						Server& server = PurchaseServer(iter->first);
						if (server.AddVmware(request.ID, true) || server.AddVmware(request.ID, false)) {
							break;
						}
					}
				}
				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
				dataManager.addList[i].emplace_back(it);
			}
			else {
				Vmware& vmware = dataManager.vmwareList[request.ID];
				Server& server = dataManager.serverList[vmware.serverID];
				server.DeleteVmware(request.ID);
				usedServerList.emplace_back(server.GetID());
			}
		}
	}
}
