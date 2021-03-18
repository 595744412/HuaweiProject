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

//void Controller::CreateList()
//{
//	//��һ��
//	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
//		//��i��
//		unsigned int purchaseCount = 0;
//		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
//			//��j������
//			RequestType request = dataManager.requestList[i][j];
//			if (request.isAdd) {
//				bool success = false;
//				for (unsigned int k = 0; k < usedServerList.size(); k++) {
//					//��k̨���÷�����
//					Server& server = dataManager.serverList[usedServerList[k]];
//					if(server.AddVmware(request.ID, true) || server.AddVmware(request.ID, false)) {
//						success = true;
//						break;
//					}
//					if (server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory) {
//						if (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory) {
//							usedServerList.erase(usedServerList.begin() + k);
//							k--;
//						}
//					}
//				}
//				if (!success) {
//					unordered_map<string, ServerType>::iterator iter;
//					for (iter = dataManager.serverTypeList.begin(); iter != dataManager.serverTypeList.end(); iter++) {
//						VmwareType vmware = dataManager.vmwareList[request.ID].myType;
//						if (iter->second.cores >= vmware.cores&&iter->second.memory >= vmware.memory) {
//							purchaseCount++;
//							dataManager.purchaseList[i][iter->first] += 1;
//							Server& server = PurchaseServer(iter->first);
//							server.AddVmware(request.ID, true);
//							break;
//						}
//					}
//				}
//				AddData it = { dataManager.vmwareList[request.ID].serverID ,dataManager.vmwareList[request.ID].myType.isDouble,dataManager.vmwareList[request.ID].isNodeA };
//				Server server = dataManager.serverList[dataManager.vmwareList[request.ID].serverID];
//				dataManager.addList[i].emplace_back(it);
//			}
//			else {
//				Vmware& vmware = dataManager.vmwareList[request.ID];
//				Server& server = dataManager.serverList[vmware.serverID];
//				server.DeleteVmware(request.ID);
//				usedServerList.emplace_back(server.GetID());
//			}
//		}
//		//����˳������������ӳ��
//		unsigned int newID = Server::GetCount() - purchaseCount;
//		unordered_map<string, unsigned int> purchaseIDList;
//		for (auto j = dataManager.purchaseList[i].cbegin(); j != dataManager.purchaseList[i].cend(); j++) {
//			purchaseIDList[j->first] = newID;
//			newID += j->second;
//		}
//		for (unsigned int j = Server::GetCount() - purchaseCount; j < Server::GetCount(); j++) {
//			string name = dataManager.serverList[j].GetServerType().name;
//			dataManager.serverIDList[j] = purchaseIDList[name];
//			purchaseIDList[name]++;
//		}
//	}
//}



void Controller::CreateList()
{
	//��ȡ�Լ۱��б�
	dataManager.sortPfm(dataManager.dayCounts);
	//try1.0�汾
	for (unsigned int i = 0; i < dataManager.dayCounts; i++) {
		//��i��
		cout << i << endl;
		unsigned int purchaseCount = 0;
		for (unsigned int j = 0; j < dataManager.requestList[i].size(); j++) {
			//��j������
			RequestType request = dataManager.requestList[i][j];
			if (request.isAdd) {
				bool success = false;
				for (unsigned int k = 0; k < usedServerList.size(); k++) {
					//��k̨���÷�����
					Server& server = dataManager.serverList[usedServerList[k]];
					if (server.AddVmware(request.ID, true) || server.AddVmware(request.ID, false)) {
						success = true;
						break;
					}
					if (server.GetA().unusedCores < leastCore || server.GetA().unusedMemory < leastMemory) {
						if (server.GetB().unusedCores < leastCore || server.GetB().unusedMemory < leastMemory) {
							usedServerList.erase(usedServerList.begin() + k);
							k--;
						}
					}
				}
				if (!success) {
					VmwareType vmware = dataManager.vmwareList[request.ID].myType;
					string serName;
					int serverID = 0;
					bool flag = false; //�Ƿ���Ҫ���ҵ���һ��������
					if (vmware.cores > vmware.memory) {
						double target = vmware.cores / vmware.memory;
						for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
							double temp = dataManager.serverTypeList[*iter].cores / dataManager.serverTypeList[*iter].memory;
							//����CPU��memory֮��ѡ�������
							if ((target <= temp + 1.5)&&(target >= temp - 1.5)
								&&(dataManager.serverTypeList[*iter].cores >= vmware.cores)
								&&(dataManager.serverTypeList[*iter].memory >= vmware.memory)){
								serName = *iter;
								flag = true;
								break;
							}
						}

					}
					else {
						double target = vmware.memory / vmware.cores;
						for (auto iter = dataManager.pfmList.begin(); iter != dataManager.pfmList.end(); iter++) {
							double temp = dataManager.serverTypeList[*iter].memory / dataManager.serverTypeList[*iter].cores;
							//����memory��CPU֮��ѡ�������
							if ((target <= temp + 1.5) && (target >= temp - 1.5)
								&& (dataManager.serverTypeList[*iter].cores >= vmware.cores)
								&& (dataManager.serverTypeList[*iter].memory >= vmware.memory)) {
								serName = *iter;
								flag = true;
								break;
							}
						}
					}
					if (!flag) {
						for (auto iter = dataManager.serverTypeList.begin(); iter != dataManager.serverTypeList.end(); iter++) {
							//����memory��CPU֮��ѡ�������
							if ((iter->second.cores >= vmware.cores)&&(iter->second.memory >= vmware.memory)) {
								serName = iter->first;
								break;
							}
						}
					}
					purchaseCount++;
					dataManager.purchaseList[i][serName] += 1;
					Server& server = PurchaseServer(serName);
					serverID = server.GetID();
					server.AddVmware(request.ID, true);
					usedServerList.emplace_back(serverID);
					
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
		//����˳������������ӳ��
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